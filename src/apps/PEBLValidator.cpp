//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
////////////////////////////////////////////////////////////////////////////////
//    Name:       src/apps/PEBLValidator.cpp
//    Purpose:    PEBL syntax validator (minimal version)
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2025 Shane T. Mueller <smueller@obereed.net>
//    License:    GPL 2
//
//    This tool validates PEBL code syntax and function calls without executing it.
//
//    Features:
//    - Syntax validation (parse tree generation)
//    - Function validation (detects undefined function calls)
//    - Standard library loading (same as runtime)
//
////////////////////////////////////////////////////////////////////////////////

#include "../base/Variant.h"      // Defines pInt, pDouble
#include "../base/PNode.h"         // Defines PNode
#include "../base/grammar.tab.hpp" // Parser

#include "../utility/PEBLPath.h"
#include "../utility/PEBLUtility.h"
#include "../utility/PError.h"     // Defines PCallStack

#include <iostream>
#include <string>
#include <vector>

using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::vector;

// Prototype for parser function defined in grammar.y
extern PNode * parse(const char* filename);

// Include headers for static member definitions
#include "../base/FunctionMap.h"
#include "../base/VariableMap.h"
#include "../base/Evaluator.h"
#include "../base/Loader.h"
#include "../devices/PEventLoop.h"

// Global variables normally defined in PEBL.cpp
// These are required by various PEBL subsystems
void * myEnv = NULL;
Evaluator * myEval = NULL;

// Define static members of Evaluator class
FunctionMap Evaluator::mFunctionMap;
VariableMap Evaluator::gGlobalVariableMap;
PEBLPath Evaluator::gPath;
const PNode * Evaluator::gEvalNode = NULL;
PCallStack Evaluator::gCallStack;
PEventLoop * Evaluator::mEventLoop = NULL;

// List of standard PEBL library files
// This matches PEBL.cpp lines 290-297 (Transfer.pbl uses HTTP stub functions when PEBL_HTTP disabled)
static const char* STANDARD_LIBRARIES[] = {
    "Design.pbl",
    "Utility.pbl",
    "Math.pbl",
    "Graphics.pbl",
    "UI.pbl",
    "HTML.pbl",
    "Layout.pbl",
    "Transfer.pbl",
    NULL  // Null terminator
};

// Global error tracking
bool gSyntaxValid = true;
vector<string> gErrors;
vector<string> gWarnings;

void PrintUsage() {
    cout << "PEBL Validator - Syntax and Function Checker\n";
    cout << "Usage: pebl-validator <file.pbl> [options]\n\n";
    cout << "Options:\n";
    cout << "  --json          Output results as JSON\n";
    cout << "  --help          Show this help message\n\n";
    cout << "Exit codes:\n";
    cout << "  0 = Valid syntax and all functions defined\n";
    cout << "  1 = Invalid syntax or undefined functions\n";
    cout << "  2 = Usage error\n\n";
    cout << "Features:\n";
    cout << "  - Parses PEBL syntax without execution\n";
    cout << "  - Validates all function calls against standard libraries\n";
    cout << "  - Detects undefined functions\n";
}

void PrintJSONResults(const string& filename) {
    cout << "{\n";
    cout << "  \"file\": \"" << filename << "\",\n";
    cout << "  \"syntax_valid\": " << (gSyntaxValid ? "true" : "false") << ",\n";
    cout << "  \"errors\": [\n";
    for (size_t i = 0; i < gErrors.size(); i++) {
        // Escape quotes in error messages
        string msg = gErrors[i];
        size_t pos = 0;
        while ((pos = msg.find('"', pos)) != string::npos) {
            msg.replace(pos, 1, "\\\"");
            pos += 2;
        }
        cout << "    \"" << msg << "\"";
        if (i < gErrors.size() - 1) cout << ",";
        cout << "\n";
    }
    cout << "  ],\n";
    cout << "  \"warnings\": [\n";
    for (size_t i = 0; i < gWarnings.size(); i++) {
        // Escape quotes in warning messages
        string msg = gWarnings[i];
        size_t pos = 0;
        while ((pos = msg.find('\"', pos)) != string::npos) {
            msg.replace(pos, 1, "\\\"");
            pos += 2;
        }
        cout << "    \"" << msg << "\"";
        if (i < gWarnings.size() - 1) cout << ",";
        cout << "\n";
    }
    cout << "  ]\n";
    cout << "}\n";
}

void PrintTextResults(const string& filename) {
    cout << "PEBL Validator Results\n";
    cout << "======================\n";
    cout << "File: " << filename << "\n\n";

    if (gSyntaxValid) {
        cout << "✓ Syntax: VALID\n";
    } else {
        cout << "✗ Syntax: INVALID\n";
    }

    if (!gErrors.empty()) {
        cout << "\nErrors:\n";
        for (const auto& err : gErrors) {
            cout << "  ✗ " << err << "\n";
        }
    }

    if (!gWarnings.empty()) {
        cout << "\nWarnings:\n";
        for (const auto& warn : gWarnings) {
            cout << "  ⚠ " << warn << "\n";
        }
    }

}

int main(int argc, char *argv[]) {
    // Disable GUI error dialogs for validator (errors still go to stderr)
    PError::gShowErrorDialogs = false;

    // Enable validator mode (throw exceptions instead of exiting on errors)
    PError::gValidatorMode = true;

    if (argc < 2) {
        PrintUsage();
        return 2;
    }

    string filename;
    bool json_output = false;

    // Parse command-line arguments
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "--help") {
            PrintUsage();
            return 0;
        } else if (arg == "--json") {
            json_output = true;
        } else if (filename.empty()) {
            filename = arg;
        } else {
            cerr << "Error: Unknown argument: " << arg << "\n";
            PrintUsage();
            return 2;
        }
    }

    if (filename.empty()) {
        cerr << "Error: No file specified\n";
        PrintUsage();
        return 2;
    }

    // Check if file exists using PEBLUtility
    if (!PEBLUtility::FileExists(filename)) {
        gSyntaxValid = false;
        gErrors.push_back("File not found: " + filename);
        if (json_output) {
            PrintJSONResults(filename);
        } else {
            PrintTextResults(filename);
        }
        return 1;
    }

    // Initialize path system (needed to find library files)
    std::list<std::string> files;
    files.push_back("pebl-validator");  // Executable name
    files.push_back(filename);  // User's file
    Evaluator::gPath.Initialize(files);

    // Parse the user's file
    PNode * head = NULL;
    PNode * tmp = NULL;
    Loader * myLoader = NULL;

    try {
        cerr << "Parsing user file: " << filename << endl;
        head = parse(filename.c_str());

        if (!head) {
            gSyntaxValid = false;
            gErrors.push_back("Parse failed - syntax error in file");
        } else {
            // Parse and combine standard library files (like PEBL.cpp lines 341-370)
            for (int i = 0; STANDARD_LIBRARIES[i] != NULL; i++) {
                string libpath = Evaluator::gPath.FindFile(STANDARD_LIBRARIES[i]);
                if (libpath != "" && !Evaluator::gPath.IsDirectory(libpath)) {
                    cerr << "Loading library: " << STANDARD_LIBRARIES[i] << endl;
                    tmp = parse(libpath.c_str());
                    if (tmp) {
                        // Combine library with existing parse tree
                        head = new OpNode(PEBL_FUNCTIONS, head, tmp, "INTERNAL PEBL STRUCTURE", -1);
                    }
                }
            }

            // Now validate functions using Loader (like PEBL.cpp lines 179-188)
            cerr << "Validating functions..." << endl;
            myLoader = new Loader();
            myLoader->LoadUserFunctions((OpNode*)head);

            // Validate Start() function has exactly one parameter
            PNode* startFunc = myLoader->GetMainPEBLFunction();
            if (startFunc) {
                OpNode* startOpNode = dynamic_cast<OpNode*>(startFunc);
                if (startOpNode) {
                    // For PEBL_FUNCTION nodes, left child is parameter list
                    PNode* paramList = startOpNode->GetLeft();
                    if (!paramList) {
                        // Start() defined with no parameters: define Start()
                        gErrors.push_back("Start() function must have exactly one parameter to receive command-line arguments (e.g., define Start(p))");
                        gSyntaxValid = false;
                    }
                }
            } else {
                gWarnings.push_back("No Start() function found - PEBL scripts should define a Start() function as the entry point");
            }

            myLoader->FindFunctions(head);

            // LoadLibraryFunctions() will throw exception on undefined functions in validator mode
            try {
                myLoader->LoadLibraryFunctions();
            } catch (const std::runtime_error& e) {
                // Catch validation errors (undefined functions, etc.)
                gSyntaxValid = false;
                gErrors.push_back(e.what());
            }

            // Clean up
            ((OpNode*)head)->DestroyFunctionTree();
            delete head;
            delete myLoader;
        }
    } catch (const std::exception& e) {
        gSyntaxValid = false;
        gErrors.push_back(string("Parse exception: ") + e.what());
        if (head) {
            ((OpNode*)head)->DestroyFunctionTree();
            delete head;
        }
        if (myLoader) delete myLoader;
    } catch (...) {
        gSyntaxValid = false;
        gErrors.push_back("Validation failed - unknown error");
        if (head) {
            ((OpNode*)head)->DestroyFunctionTree();
            delete head;
        }
        if (myLoader) delete myLoader;
    }

    // Output results
    if (json_output) {
        PrintJSONResults(filename);
    } else {
        PrintTextResults(filename);
    }

    // Return exit code
    return gSyntaxValid ? 0 : 1;
}
