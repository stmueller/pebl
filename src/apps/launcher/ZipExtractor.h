#ifndef ZIP_EXTRACTOR_H
#define ZIP_EXTRACTOR_H

#include <string>
#include <vector>

/**
 * ZipExtractor - Cross-platform ZIP file extraction utility
 *
 * Uses libzip to extract PEBL snapshot packages and validate their contents.
 * Supports reading files without full extraction for validation dialogs.
 */
class ZipExtractor {
public:
    // Result structure for operations
    struct Result {
        bool success;
        std::string error;

        Result(bool s = true, const std::string& e = "")
            : success(s), error(e) {}
    };

    /**
     * Extract entire ZIP archive to destination directory
     * Creates destination directory if it doesn't exist
     *
     * @param zipPath Path to ZIP file
     * @param destPath Destination directory (will be created)
     * @return Result with success status and error message if failed
     */
    static Result ExtractAll(const std::string& zipPath,
                            const std::string& destPath);

    /**
     * Extract single file from ZIP to destination path
     *
     * @param zipPath Path to ZIP file
     * @param fileInZip Path of file within ZIP (e.g., "study-info.json")
     * @param destPath Destination file path
     * @return Result with success status and error message if failed
     */
    static Result ExtractFile(const std::string& zipPath,
                             const std::string& fileInZip,
                             const std::string& destPath);

    /**
     * Read file contents from ZIP without extracting
     * Useful for validation and preview before extraction
     *
     * @param zipPath Path to ZIP file
     * @param fileInZip Path of file within ZIP
     * @param outContents String to receive file contents
     * @return Result with success status and error message if failed
     */
    static Result ReadFile(const std::string& zipPath,
                          const std::string& fileInZip,
                          std::string& outContents);

    /**
     * List all files and directories in ZIP
     *
     * @param zipPath Path to ZIP file
     * @param outFiles Vector to receive list of file paths
     * @return Result with success status and error message if failed
     */
    static Result ListContents(const std::string& zipPath,
                              std::vector<std::string>& outFiles);

    /**
     * Check if ZIP contains a specific file
     *
     * @param zipPath Path to ZIP file
     * @param fileInZip Path of file to check for
     * @return true if file exists in ZIP, false otherwise
     */
    static bool ContainsFile(const std::string& zipPath,
                            const std::string& fileInZip);

    /**
     * Validate ZIP file integrity
     * Checks if file is a valid ZIP and not corrupted
     *
     * @param zipPath Path to ZIP file
     * @return Result with success status and error message if invalid
     */
    static Result Validate(const std::string& zipPath);

    /**
     * Validate that ZIP is a PEBL snapshot package
     * Checks for required files: study-info.json, tests/ directory
     *
     * @param zipPath Path to ZIP file
     * @return Result with success status and error message if not valid snapshot
     */
    static Result ValidateSnapshot(const std::string& zipPath);

private:
    /**
     * Create directories recursively
     * Similar to mkdir -p
     *
     * @param path Directory path to create
     * @return true if successful or already exists, false on error
     */
    static bool CreateDirectories(const std::string& path);
};

#endif // ZIP_EXTRACTOR_H
