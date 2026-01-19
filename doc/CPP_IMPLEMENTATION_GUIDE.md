# C++ Launcher Implementation Guide

## Quick Start

This guide helps C++ developers implement the PEBL Native Launcher study management system.

## Documentation Overview

Read these documents in order:

1. **THIS DOCUMENT** - Quick start and overview
2. **NATIVE_LAUNCHER_STUDY_SYSTEM.md** - Complete system design
3. **STUDY_SYSTEM_IMPLEMENTATION_PLAN.md** - Development phases and timeline
4. **Format Specifications:**
   - STUDY_FORMAT_SPECIFICATION.md
   - CHAIN_FORMAT_SPECIFICATION.md
   - CHAINPAGE_SPECIFICATION.md
   - NAMING_CONVENTIONS.md

## What's Already Done

### PEBL Components (Complete)
- ✅ **ChainPage.pbl** - Displays instruction/consent/completion pages
- ✅ Located at: `pebl-lib/ChainPage.pbl`
- ✅ Tested and working
- ✅ Just needs to be called with JSON config

### Documentation (Complete)
- ✅ JSON schemas for validation (`schemas/`)
- ✅ Example study (`examples/example-study/`)
- ✅ Format specifications (this directory)
- ✅ System design document
- ✅ Implementation plan

### What Needs Implementation

Everything in the C++ launcher:
- Study management (create, open, edit)
- Chain management (create, edit, execute)
- Snapshot creation/import
- Parameter editor
- All UI components

## Technology Stack

### Recommended

- **Language:** C++ (C++17 or later)
- **GUI Framework:** Qt 5 or Qt 6
  - Cross-platform (Windows/Linux/Mac)
  - Mature and well-documented
  - Good JSON support (QJsonDocument)
  - Native file dialogs
  - Process management (QProcess)
- **JSON Library:** Qt's built-in QJson or nlohmann/json
- **HTTP:** Qt Network (for upload feature)
- **ZIP:** Qt's QuaZip or libzip

### Alternative: wxWidgets

If not using Qt:
- **GUI:** wxWidgets
- **JSON:** nlohmann/json or RapidJSON
- **HTTP:** libcurl
- **ZIP:** libzip

## Project Structure

```
launcher/
├── src/
│   ├── main.cpp
│   ├── models/
│   │   ├── Study.h/cpp
│   │   ├── Chain.h/cpp
│   │   ├── ChainItem.h/cpp
│   │   └── Test.h/cpp
│   ├── parsers/
│   │   ├── StudyInfoParser.h/cpp
│   │   ├── ChainParser.h/cpp
│   │   └── ParameterParser.h/cpp
│   ├── ui/
│   │   ├── MainWindow.h/cpp
│   │   ├── StudyManager.h/cpp
│   │   ├── StudyEditor.h/cpp
│   │   ├── ChainEditor.h/cpp
│   │   └── ParameterEditor.h/cpp
│   ├── execution/
│   │   ├── ChainRunner.h/cpp
│   │   └── ProcessManager.h/cpp
│   └── utils/
│       ├── FileUtils.h/cpp
│       ├── PathUtils.h/cpp
│       └── Validator.h/cpp
├── resources/
│   ├── icons/
│   └── ui/
└── tests/
    └── ...
```

## Core Data Structures

### Study Class

```cpp
class Study {
public:
    std::string name;
    std::string description;
    int version;
    std::string author;
    std::string studyToken;
    std::string createdDate;
    std::string modifiedDate;
    std::vector<Test> tests;
    std::string path;  // Filesystem path

    static Study LoadFromDirectory(const std::string& path);
    bool Save();
    void AddTest(const Test& test);
    void RemoveTest(const std::string& testName);
    std::vector<Chain> GetChains();
};
```

### Chain Class

```cpp
class Chain {
public:
    std::string name;
    std::string description;
    std::vector<ChainItem> items;
    std::string filePath;

    static Chain LoadFromFile(const std::string& path);
    bool Save();
    void AddItem(const ChainItem& item);
    void RemoveItem(int index);
    void MoveItem(int fromIndex, int toIndex);
};
```

### ChainItem Class

```cpp
enum class ItemType {
    Instruction,
    Consent,
    Completion,
    Test
};

class ChainItem {
public:
    ItemType type;

    // For page items (instruction/consent/completion)
    std::string title;
    std::string content;

    // For test items
    std::string testName;
    std::string paramVariant;
    std::string language;

    json ToChainPageConfig();  // For page items
    std::string BuildTestCommand(const std::string& subjectID);  // For test items
};
```

### Test Class

```cpp
struct ParameterVariant {
    std::string description;
    std::string file;  // Path to .par.json or empty for default
};

class Test {
public:
    std::string testName;
    std::string testPath;
    bool included;
    std::map<std::string, ParameterVariant> parameterVariants;

    bool Exists();
    std::vector<std::string> GetAvailableLanguages();
};
```

## JSON Parsing

### Using Qt

```cpp
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

Study Study::LoadFromDirectory(const QString& path) {
    Study study;
    study.path = path.toStdString();

    // Read study-info.json
    QString filePath = path + "/study-info.json";
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        throw std::runtime_error("Cannot open study-info.json");
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject obj = doc.object();

    // Parse fields
    study.name = obj["study_name"].toString().toStdString();
    study.version = obj["version"].toInt();
    study.description = obj["description"].toString().toStdString();

    // Parse tests array
    QJsonArray testsArray = obj["tests"].toArray();
    for (const QJsonValue& value : testsArray) {
        Test test = Test::FromJson(value.toObject());
        study.tests.push_back(test);
    }

    return study;
}
```

### Using nlohmann/json

```cpp
#include <nlohmann/json.hpp>
using json = nlohmann::json;

Study Study::LoadFromDirectory(const std::string& path) {
    Study study;
    study.path = path;

    // Read file
    std::ifstream file(path + "/study-info.json");
    json data = json::parse(file);

    // Parse fields
    study.name = data["study_name"];
    study.version = data["version"];
    study.description = data.value("description", "");

    // Parse tests
    for (const auto& testData : data["tests"]) {
        Test test = Test::FromJson(testData);
        study.tests.push_back(test);
    }

    return study;
}
```

## Chain Execution

### ChainRunner Example

```cpp
class ChainRunner {
private:
    Study& study;
    Chain& chain;
    std::string subjectID;
    std::string tempDir;

public:
    ChainRunner(Study& s, Chain& c, const std::string& sid)
        : study(s), chain(c), subjectID(sid) {
        tempDir = CreateTempDirectory();
    }

    ~ChainRunner() {
        CleanupTempDirectory(tempDir);
    }

    bool Run() {
        for (const auto& item : chain.items) {
            bool success = false;

            switch (item.type) {
                case ItemType::Instruction:
                case ItemType::Consent:
                case ItemType::Completion:
                    success = ExecutePageItem(item);
                    break;

                case ItemType::Test:
                    success = ExecuteTestItem(item);
                    break;
            }

            if (!success) {
                // Handle error - show dialog, log, etc.
                return false;
            }
        }

        return true;
    }

private:
    bool ExecutePageItem(const ChainItem& item) {
        // 1. Create temp JSON config
        std::string configFile = CreateChainPageConfig(item);

        // 2. Build command
        std::string command = GetPEBLPath() + " ChainPage.pbl -v " + configFile;

        // 3. Execute
        int exitCode = ExecuteCommand(command);

        // 4. Cleanup
        std::filesystem::remove(configFile);

        return (exitCode == 0);
    }

    bool ExecuteTestItem(const ChainItem& item) {
        // Build command
        std::string testPath = study.path + "/" + GetTest(item.testName).testPath;
        std::string mainFile = testPath + "/" + item.testName + ".pbl";

        std::string command = GetPEBLPath() + " " + mainFile;
        command += " -v subnum=" + subjectID;

        // Add parameter variant
        if (item.paramVariant != "default") {
            std::string parFile = GetParameterFile(item.testName, item.paramVariant);
            if (!parFile.empty()) {
                command += " -v pfile=" + parFile;
            }
        }

        // Add language
        if (!item.language.empty()) {
            command += " -v language=" + item.language;
        }

        // Execute
        int exitCode = ExecuteCommand(command);

        return (exitCode == 0);
    }

    std::string CreateChainPageConfig(const ChainItem& item) {
        json config;
        config["title"] = item.title;
        config["content"] = item.content;
        config["page_type"] = ItemTypeToString(item.type);

        std::string filename = tempDir + "/chainpage-" + GenerateUUID() + ".json";

        std::ofstream out(filename);
        out << config.dump(2);
        out.close();

        return filename;
    }
};
```

## First-Run Initialization

### Workspace Setup

```cpp
class WorkspaceManager {
public:
    static bool IsFirstRun() {
        std::string workspace = GetWorkspacePath();
        return !std::filesystem::exists(workspace);
    }

    static void RunFirstTimeSetup() {
        std::string workspace = GetWorkspacePath();
        std::string install = GetInstallPath();

        // Create directories
        CreateDirectories(workspace);

        // Copy resources
        CopyResources(install, workspace);

        // Create README
        CreateReadme(workspace);

        // Show welcome dialog
        ShowWelcomeDialog();
    }

private:
    static std::string GetWorkspacePath() {
        #ifdef _WIN32
            std::string docs = GetEnvironmentVariable("USERPROFILE") + "\\Documents";
        #else
            std::string docs = GetEnvironmentVariable("HOME") + "/Documents";
        #endif
        return docs + "/pebl-exp.2.3";
    }

    static void CreateDirectories(const std::string& workspace) {
        std::filesystem::create_directories(workspace + "/my_studies");
        std::filesystem::create_directories(workspace + "/snapshots");
        std::filesystem::create_directories(workspace + "/documentation");
        std::filesystem::create_directories(workspace + "/demo");
        std::filesystem::create_directories(workspace + "/tutorial");
        std::filesystem::create_directories(workspace + "/data");
    }

    static void CopyResources(const std::string& install, const std::string& workspace) {
        // Copy manual
        std::filesystem::copy(
            install + "/doc/manual.pdf",
            workspace + "/manual.pdf"
        );

        // Copy directories
        std::filesystem::copy(
            install + "/documentation",
            workspace + "/documentation",
            std::filesystem::copy_options::recursive
        );

        // Similar for demo, tutorial
    }
};
```

### Main Entry Point

```cpp
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Check first run
    if (WorkspaceManager::IsFirstRun()) {
        WorkspaceManager::RunFirstTimeSetup();
    }

    // Create and show main window
    MainWindow window;
    window.show();

    return app.exec();
}
```

## Snapshot Creation

### Excluding Data Directories

```cpp
class SnapshotCreator {
public:
    static bool CreateSnapshot(const Study& study, const std::string& outputPath) {
        // Create snapshot directory
        std::filesystem::create_directories(outputPath);

        // Copy study-info.json
        std::filesystem::copy_file(
            study.path + "/study-info.json",
            outputPath + "/study-info.json"
        );

        // Copy chains directory
        std::filesystem::copy(
            study.path + "/chains",
            outputPath + "/chains",
            std::filesystem::copy_options::recursive
        );

        // Copy tests (excluding data directories)
        CopyTestsExcludingData(study.path + "/tests", outputPath + "/tests");

        return true;
    }

private:
    static void CopyTestsExcludingData(const std::string& src, const std::string& dst) {
        std::filesystem::create_directories(dst);

        for (const auto& entry : std::filesystem::directory_iterator(src)) {
            std::string name = entry.path().filename().string();
            std::string srcPath = entry.path().string();
            std::string dstPath = dst + "/" + name;

            if (entry.is_directory()) {
                // Copy directory recursively, excluding "data"
                CopyDirectoryExcluding(srcPath, dstPath, {"data"});
            } else {
                std::filesystem::copy_file(srcPath, dstPath);
            }
        }
    }

    static void CopyDirectoryExcluding(
        const std::string& src,
        const std::string& dst,
        const std::vector<std::string>& exclude
    ) {
        std::filesystem::create_directories(dst);

        for (const auto& entry : std::filesystem::directory_iterator(src)) {
            std::string name = entry.path().filename().string();

            // Skip excluded directories
            if (std::find(exclude.begin(), exclude.end(), name) != exclude.end()) {
                continue;
            }

            std::string srcPath = entry.path().string();
            std::string dstPath = dst + "/" + name;

            if (entry.is_directory()) {
                CopyDirectoryExcluding(srcPath, dstPath, exclude);
            } else {
                std::filesystem::copy_file(srcPath, dstPath);
            }
        }
    }
};
```

## Validation

### Study Validation

```cpp
struct ValidationResult {
    std::vector<std::string> errors;
    std::vector<std::string> warnings;

    bool IsValid() const { return errors.empty(); }
};

class StudyValidator {
public:
    static ValidationResult Validate(const Study& study) {
        ValidationResult result;

        // Check required fields
        if (study.name.empty()) {
            result.errors.push_back("Study name is required");
        }

        if (study.version < 1) {
            result.errors.push_back("Version must be >= 1");
        }

        // Check optional but recommended
        if (study.description.empty()) {
            result.warnings.push_back("No description provided");
        }

        if (study.author.empty()) {
            result.warnings.push_back("No author specified");
        }

        // Check tests
        for (const auto& test : study.tests) {
            ValidateTest(test, study.path, result);
        }

        return result;
    }

private:
    static void ValidateTest(
        const Test& test,
        const std::string& studyPath,
        ValidationResult& result
    ) {
        std::string testPath = studyPath + "/" + test.testPath;

        if (!std::filesystem::exists(testPath)) {
            result.errors.push_back("Test directory not found: " + test.testPath);
        }

        if (test.parameterVariants.find("default") == test.parameterVariants.end()) {
            result.errors.push_back("Test " + test.testName + " missing 'default' variant");
        }
    }
};
```

## UI Components (Qt Example)

### Study Manager

```cpp
class StudyManagerDialog : public QDialog {
    Q_OBJECT

public:
    StudyManagerDialog(QWidget* parent = nullptr) {
        setupUI();
        loadStudies();
    }

private slots:
    void onNewStudy() {
        // Show create study dialog
        CreateStudyDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted) {
            loadStudies();
        }
    }

    void onOpenStudy() {
        QTableWidgetItem* item = studyTable->currentItem();
        if (item) {
            std::string studyPath = item->data(Qt::UserRole).toString().toStdString();
            StudyEditorDialog editor(studyPath, this);
            editor.exec();
        }
    }

    void onImportSnapshot() {
        QString path = QFileDialog::getExistingDirectory(
            this,
            "Select Snapshot Directory",
            QDir::homePath()
        );

        if (!path.isEmpty()) {
            importSnapshot(path.toStdString());
        }
    }

private:
    QTableWidget* studyTable;

    void setupUI() {
        // Create table
        studyTable = new QTableWidget(this);
        studyTable->setColumnCount(3);
        studyTable->setHorizontalHeaderLabels({"Study Name", "Modified", "Version"});

        // Create buttons
        QPushButton* newButton = new QPushButton("New Study", this);
        QPushButton* openButton = new QPushButton("Open", this);
        QPushButton* importButton = new QPushButton("Import Snapshot", this);

        connect(newButton, &QPushButton::clicked, this, &StudyManagerDialog::onNewStudy);
        connect(openButton, &QPushButton::clicked, this, &StudyManagerDialog::onOpenStudy);
        connect(importButton, &QPushButton::clicked, this, &StudyManagerDialog::onImportSnapshot);

        // Layout
        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->addWidget(studyTable);

        QHBoxLayout* buttonLayout = new QHBoxLayout();
        buttonLayout->addWidget(newButton);
        buttonLayout->addWidget(openButton);
        buttonLayout->addWidget(importButton);
        layout->addLayout(buttonLayout);
    }

    void loadStudies() {
        studyTable->clearContents();
        studyTable->setRowCount(0);

        std::string studiesPath = GetWorkspacePath() + "/my_studies";
        for (const auto& entry : std::filesystem::directory_iterator(studiesPath)) {
            if (entry.is_directory()) {
                addStudyToTable(entry.path().string());
            }
        }
    }

    void addStudyToTable(const std::string& path) {
        try {
            Study study = Study::LoadFromDirectory(path);

            int row = studyTable->rowCount();
            studyTable->insertRow(row);

            studyTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(study.name)));
            studyTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(study.modifiedDate)));
            studyTable->setItem(row, 2, new QTableWidgetItem(QString::number(study.version)));

            studyTable->item(row, 0)->setData(Qt::UserRole, QString::fromStdString(path));
        } catch (const std::exception& e) {
            // Log error, skip invalid study
        }
    }
};
```

## Process Management

### Executing PEBL

```cpp
class PEBLExecutor {
public:
    static int Execute(const std::string& command, bool waitForFinish = true) {
        #ifdef _WIN32
            return ExecuteWindows(command, waitForFinish);
        #else
            return ExecuteUnix(command, waitForFinish);
        #endif
    }

private:
    #ifdef _WIN32
    static int ExecuteWindows(const std::string& command, bool waitForFinish) {
        STARTUPINFO si = {sizeof(si)};
        PROCESS_INFORMATION pi;

        if (!CreateProcess(
            NULL,
            const_cast<char*>(command.c_str()),
            NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi
        )) {
            return -1;
        }

        if (waitForFinish) {
            WaitForSingleObject(pi.hProcess, INFINITE);
            DWORD exitCode;
            GetExitCodeProcess(pi.hProcess, &exitCode);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            return exitCode;
        }

        return 0;
    }
    #else
    static int ExecuteUnix(const std::string& command, bool waitForFinish) {
        return system(command.c_str());
    }
    #endif
};
```

### Using Qt (Preferred)

```cpp
class PEBLExecutor {
public:
    static int Execute(const QString& command) {
        QProcess process;
        process.start(command);
        process.waitForFinished(-1);  // Wait indefinitely
        return process.exitCode();
    }
};
```

## Testing Strategy

### Unit Tests

```cpp
#include <gtest/gtest.h>

TEST(StudyTest, LoadValidStudy) {
    Study study = Study::LoadFromDirectory("examples/example-study");

    EXPECT_EQ(study.name, "Example Cognitive Battery");
    EXPECT_EQ(study.version, 1);
    EXPECT_EQ(study.tests.size(), 2);
}

TEST(ChainTest, LoadValidChain) {
    Chain chain = Chain::LoadFromFile("examples/example-study/chains/main-chain.json");

    EXPECT_EQ(chain.name, "main-chain");
    EXPECT_EQ(chain.items.size(), 6);
}

TEST(SnapshotTest, ExcludesDataDirectories) {
    // Create test study with data
    // Create snapshot
    // Verify no data directories in snapshot
}
```

## Development Phases

Follow the implementation plan in `STUDY_SYSTEM_IMPLEMENTATION_PLAN.md`:

1. **Phase 1-2:** File formats and validation (DONE)
2. **Phase 3:** First-run initialization
3. **Phase 4:** Study management core
4. **Phase 5-7:** UI components
5. **Phase 8:** Chain execution
6. **Phase 9:** Snapshot management
7. **Phase 10:** Parameter editor
8. **Phase 11:** Online platform integration
9. **Phase 12:** Polish and testing

## Resources

### Example Code Location

- **Example study:** `examples/example-study/`
- **ChainPage.pbl:** `pebl-lib/ChainPage.pbl`
- **Schemas:** `schemas/`

### External Libraries

- **Qt:** https://www.qt.io/
- **nlohmann/json:** https://github.com/nlohmann/json
- **Google Test:** https://github.com/google/googletest

### PEBL Documentation

- **Manual:** `doc/manual.pdf`
- **Website:** https://pebl.sourceforge.net

## Getting Help

### Questions About Format

Refer to format specification documents in this directory.

### Questions About Design

See `NATIVE_LAUNCHER_STUDY_SYSTEM.md` for complete system design.

### Questions About PEBL

- Check PEBL manual
- Look at battery test examples
- Run tests with PEBL interpreter to understand behavior

## Next Steps

1. Set up development environment (Qt/C++)
2. Create basic project structure
3. Implement JSON parsers (Study, Chain)
4. Create data model classes
5. Build study manager UI
6. Implement first-run initialization
7. Follow implementation plan phases

---

**Document Version:** 1.0
**Last Updated:** 2026-01-09
**For:** C++ Launcher Development Team
