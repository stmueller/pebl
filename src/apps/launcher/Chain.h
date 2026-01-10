// Chain.h - PEBL Chain data model
// Copyright (c) 2026 Shane T. Mueller
// Licensed under GPL

#ifndef CHAIN_H
#define CHAIN_H

#include <string>
#include <vector>
#include <memory>

// Chain item types
enum class ItemType {
    Instruction,
    Consent,
    Completion,
    Test
};

// Convert ItemType to string
std::string ItemTypeToString(ItemType type);
ItemType StringToItemType(const std::string& str);

// Chain item
struct ChainItem {
    ItemType type;

    // For page items (instruction/consent/completion)
    std::string title;
    std::string content;

    // For test items
    std::string testName;
    std::string paramVariant;
    std::string language;
    int randomGroup;  // 0 = no randomization, >0 = randomization group ID

    ChainItem() : type(ItemType::Instruction), paramVariant("default"), randomGroup(0) {}
    ChainItem(ItemType t) : type(t), paramVariant("default"), randomGroup(0) {}

    // Create ChainPage.pbl JSON config for page items
    std::string CreateChainPageConfig(const std::string& tempDir) const;

    // Build PEBL command for test items
    std::string BuildTestCommand(const std::string& studyPath,
                                 const std::string& subjectID) const;

    // Get display name for this item
    std::string GetDisplayName() const;

    // Check if this is a page item (instruction/consent/completion)
    bool IsPageItem() const {
        return type == ItemType::Instruction ||
               type == ItemType::Consent ||
               type == ItemType::Completion;
    }
};

// Chain data model
class Chain {
public:
    Chain();
    ~Chain();

    // Load chain from JSON file
    static std::shared_ptr<Chain> LoadFromFile(const std::string& path);

    // Save chain to JSON file
    bool Save();

    // Create new empty chain
    static std::shared_ptr<Chain> CreateNew(const std::string& path,
                                            const std::string& name,
                                            const std::string& description = "");

    // Getters
    const std::string& GetName() const { return mName; }
    const std::string& GetDescription() const { return mDescription; }
    const std::string& GetFilePath() const { return mFilePath; }
    const std::vector<ChainItem>& GetItems() const { return mItems; }
    int GetItemCount() const { return static_cast<int>(mItems.size()); }

    // Setters
    void SetName(const std::string& name) { mName = name; }
    void SetDescription(const std::string& desc) { mDescription = desc; }

    // Item management
    void AddItem(const ChainItem& item);
    void InsertItem(int index, const ChainItem& item);
    bool RemoveItem(int index);
    bool MoveItem(int fromIndex, int toIndex);
    ChainItem* GetItem(int index);
    const ChainItem* GetItem(int index) const;

    // Validation
    struct ValidationResult {
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        bool IsValid() const { return errors.empty(); }
    };
    ValidationResult Validate(const class Study* study = nullptr) const;

private:
    bool LoadFromJSON(const std::string& jsonPath);
    bool SaveToJSON(const std::string& jsonPath);

    std::string mFilePath;       // Path to chain JSON file
    std::string mName;
    std::string mDescription;
    std::vector<ChainItem> mItems;
};

#endif // CHAIN_H
