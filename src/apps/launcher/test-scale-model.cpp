// test-scale-model.cpp - Quick test of ScaleDefinition/ScaleManager
// Compile: g++ -std=c++17 -I../../../libs -o test-scale test-scale-model.cpp ScaleDefinition.cpp ScaleManager.cpp
// Run: ./test-scale

#include "ScaleDefinition.h"
#include "ScaleManager.h"
#include <iostream>

int main() {
    std::cout << "Testing Scale Data Model\n";
    std::cout << "========================\n\n";

    // Test 1: ScaleManager
    std::cout << "Test 1: Creating ScaleManager...\n";
    ScaleManager manager("../../../battery");

    std::cout << "Test 2: Scanning for available scales...\n";
    auto scales = manager.GetAvailableScales();
    std::cout << "Found " << scales.size() << " scales:\n";
    for (const auto& scale : scales) {
        std::cout << "  - " << scale << "\n";
    }
    std::cout << "\n";

    // Test 3: Load Grit scale
    std::cout << "Test 3: Loading Grit scale...\n";
    auto grit = manager.LoadScale("grit");
    if (grit) {
        std::cout << "✓ Successfully loaded Grit scale\n";
        std::cout << "  Code: " << grit->GetScaleInfo().code << "\n";
        std::cout << "  Name: " << grit->GetScaleInfo().name << "\n";
        std::cout << "  Questions: " << grit->GetQuestions().size() << "\n";
        std::cout << "  Dimensions: " << grit->GetDimensions().size() << "\n";

        // Show first 3 questions
        std::cout << "\n  First 3 questions:\n";
        for (size_t i = 0; i < std::min(size_t(3), grit->GetQuestions().size()); i++) {
            const auto& q = grit->GetQuestions()[i];
            std::string text = grit->GetTranslation("en", q.text_key);
            if (text.empty()) text = "[no translation]";
            std::cout << "    " << q.id << " [" << q.type << "] (coding: " << q.coding << "): "
                     << text.substr(0, 50);
            if (text.length() > 50) std::cout << "...";
            std::cout << "\n";
        }

        // Validate
        std::cout << "\n  Validating...\n";
        std::string errors;
        if (grit->Validate(errors)) {
            std::cout << "  ✓ Validation passed\n";
        } else {
            std::cout << "  ✗ Validation failed:\n" << errors << "\n";
        }
    } else {
        std::cout << "✗ Failed to load Grit scale\n";
        return 1;
    }

    std::cout << "\n";

    // Test 4: Load CRT scale
    std::cout << "Test 4: Loading CRT scale...\n";
    auto crt = manager.LoadScale("crt");
    if (crt) {
        std::cout << "✓ Successfully loaded CRT scale\n";
        std::cout << "  Code: " << crt->GetScaleInfo().code << "\n";
        std::cout << "  Name: " << crt->GetScaleInfo().name << "\n";
        std::cout << "  Questions: " << crt->GetQuestions().size() << "\n";
    } else {
        std::cout << "✗ Failed to load CRT scale\n";
    }

    std::cout << "\n========================\n";
    std::cout << "All tests completed successfully!\n";

    return 0;
}
