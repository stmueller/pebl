#include "ZipExtractor.h"
#include <zip.h>
#include <sys/stat.h>
#include <fstream>
#include <cstring>
#include <iostream>

#ifdef _WIN32
#include <direct.h>
#define PATH_SEPARATOR '\\'
#define mkdir(path, mode) _mkdir(path)
#else
#include <sys/types.h>
#define PATH_SEPARATOR '/'
#endif

// Extract entire ZIP to destination
ZipExtractor::Result ZipExtractor::ExtractAll(const std::string& zipPath,
                                              const std::string& destPath) {
    int error = 0;
    zip_t* archive = zip_open(zipPath.c_str(), ZIP_RDONLY, &error);

    if (!archive) {
        zip_error_t ziperror;
        zip_error_init_with_code(&ziperror, error);
        std::string msg = "Failed to open ZIP: " + std::string(zip_error_strerror(&ziperror));
        zip_error_fini(&ziperror);
        return Result(false, msg);
    }

    // Get number of files in archive
    zip_int64_t numEntries = zip_get_num_entries(archive, 0);
    if (numEntries < 0) {
        zip_close(archive);
        return Result(false, "Failed to get entry count");
    }

    // Create destination directory if it doesn't exist
    if (!CreateDirectories(destPath)) {
        zip_close(archive);
        return Result(false, "Failed to create destination directory: " + destPath);
    }

    // Extract each file
    for (zip_int64_t i = 0; i < numEntries; i++) {
        // Get file stats
        struct zip_stat st;
        zip_stat_init(&st);
        if (zip_stat_index(archive, i, 0, &st) != 0) {
            zip_close(archive);
            return Result(false, "Failed to stat file at index " + std::to_string(i));
        }

        std::string filename = st.name;

        // Skip directories (they end with /)
        if (filename.back() == '/') {
            // Create directory
            std::string dirPath = destPath + PATH_SEPARATOR + filename;
            CreateDirectories(dirPath);
            continue;
        }

        // Build destination path
        std::string destFile = destPath + PATH_SEPARATOR + filename;

        // Create parent directories if needed
        size_t lastSlash = destFile.find_last_of("/\\");
        if (lastSlash != std::string::npos) {
            std::string dirPath = destFile.substr(0, lastSlash);
            if (!CreateDirectories(dirPath)) {
                zip_close(archive);
                return Result(false, "Failed to create directory: " + dirPath);
            }
        }

        // Open file in ZIP
        zip_file_t* file = zip_fopen_index(archive, i, 0);
        if (!file) {
            zip_close(archive);
            return Result(false, "Failed to open file in ZIP: " + filename);
        }

        // Read file contents
        std::vector<char> buffer(st.size);
        zip_int64_t bytesRead = zip_fread(file, buffer.data(), st.size);
        zip_fclose(file);

        if (bytesRead != static_cast<zip_int64_t>(st.size)) {
            zip_close(archive);
            return Result(false, "Failed to read file: " + filename);
        }

        // Write to destination
        std::ofstream out(destFile, std::ios::binary);
        if (!out) {
            zip_close(archive);
            return Result(false, "Failed to create file: " + destFile);
        }
        out.write(buffer.data(), bytesRead);
        out.close();
    }

    zip_close(archive);
    return Result(true);
}

// Extract single file from ZIP
ZipExtractor::Result ZipExtractor::ExtractFile(const std::string& zipPath,
                                              const std::string& fileInZip,
                                              const std::string& destPath) {
    int error = 0;
    zip_t* archive = zip_open(zipPath.c_str(), ZIP_RDONLY, &error);

    if (!archive) {
        return Result(false, "Failed to open ZIP");
    }

    // Find file by name
    zip_int64_t index = zip_name_locate(archive, fileInZip.c_str(), 0);
    if (index < 0) {
        zip_close(archive);
        return Result(false, "File not found in ZIP: " + fileInZip);
    }

    // Get file stats
    struct zip_stat st;
    zip_stat_init(&st);
    if (zip_stat_index(archive, index, 0, &st) != 0) {
        zip_close(archive);
        return Result(false, "Failed to stat file");
    }

    // Open and read file
    zip_file_t* file = zip_fopen_index(archive, index, 0);
    if (!file) {
        zip_close(archive);
        return Result(false, "Failed to open file");
    }

    std::vector<char> buffer(st.size);
    zip_int64_t bytesRead = zip_fread(file, buffer.data(), st.size);
    zip_fclose(file);
    zip_close(archive);

    if (bytesRead != static_cast<zip_int64_t>(st.size)) {
        return Result(false, "Failed to read file");
    }

    // Create parent directories for destination
    size_t lastSlash = destPath.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        std::string dirPath = destPath.substr(0, lastSlash);
        if (!CreateDirectories(dirPath)) {
            return Result(false, "Failed to create directory: " + dirPath);
        }
    }

    // Write to destination
    std::ofstream out(destPath, std::ios::binary);
    if (!out) {
        return Result(false, "Failed to create file: " + destPath);
    }
    out.write(buffer.data(), bytesRead);
    out.close();

    return Result(true);
}

// Read single file from ZIP without extracting
ZipExtractor::Result ZipExtractor::ReadFile(const std::string& zipPath,
                                           const std::string& fileInZip,
                                           std::string& outContents) {
    int error = 0;
    zip_t* archive = zip_open(zipPath.c_str(), ZIP_RDONLY, &error);

    if (!archive) {
        return Result(false, "Failed to open ZIP");
    }

    // Find file by name
    zip_int64_t index = zip_name_locate(archive, fileInZip.c_str(), 0);
    if (index < 0) {
        zip_close(archive);
        return Result(false, "File not found in ZIP: " + fileInZip);
    }

    // Get file stats
    struct zip_stat st;
    zip_stat_init(&st);
    if (zip_stat_index(archive, index, 0, &st) != 0) {
        zip_close(archive);
        return Result(false, "Failed to stat file");
    }

    // Open and read file
    zip_file_t* file = zip_fopen_index(archive, index, 0);
    if (!file) {
        zip_close(archive);
        return Result(false, "Failed to open file");
    }

    std::vector<char> buffer(st.size + 1); // +1 for null terminator
    zip_int64_t bytesRead = zip_fread(file, buffer.data(), st.size);
    zip_fclose(file);
    zip_close(archive);

    if (bytesRead != static_cast<zip_int64_t>(st.size)) {
        return Result(false, "Failed to read file");
    }

    buffer[st.size] = '\0';
    outContents = std::string(buffer.data());

    return Result(true);
}

// List all files in ZIP
ZipExtractor::Result ZipExtractor::ListContents(const std::string& zipPath,
                                               std::vector<std::string>& outFiles) {
    int error = 0;
    zip_t* archive = zip_open(zipPath.c_str(), ZIP_RDONLY, &error);

    if (!archive) {
        return Result(false, "Failed to open ZIP");
    }

    zip_int64_t numEntries = zip_get_num_entries(archive, 0);
    if (numEntries < 0) {
        zip_close(archive);
        return Result(false, "Failed to get entry count");
    }

    outFiles.clear();
    for (zip_int64_t i = 0; i < numEntries; i++) {
        const char* name = zip_get_name(archive, i, 0);
        if (name) {
            outFiles.push_back(name);
        }
    }

    zip_close(archive);
    return Result(true);
}

// Check if ZIP contains specific file
bool ZipExtractor::ContainsFile(const std::string& zipPath,
                               const std::string& fileInZip) {
    int error = 0;
    zip_t* archive = zip_open(zipPath.c_str(), ZIP_RDONLY, &error);

    if (!archive) {
        return false;
    }

    zip_int64_t index = zip_name_locate(archive, fileInZip.c_str(), 0);
    zip_close(archive);

    return index >= 0;
}

// Validate ZIP file
ZipExtractor::Result ZipExtractor::Validate(const std::string& zipPath) {
    int error = 0;
    zip_t* archive = zip_open(zipPath.c_str(), ZIP_RDONLY | ZIP_CHECKCONS, &error);

    if (!archive) {
        zip_error_t ziperror;
        zip_error_init_with_code(&ziperror, error);
        std::string msg = std::string(zip_error_strerror(&ziperror));
        zip_error_fini(&ziperror);
        return Result(false, "Invalid or corrupted ZIP: " + msg);
    }

    zip_close(archive);
    return Result(true);
}

// Validate PEBL snapshot
ZipExtractor::Result ZipExtractor::ValidateSnapshot(const std::string& zipPath) {
    // First validate as ZIP
    Result validZip = Validate(zipPath);
    if (!validZip.success) {
        return validZip;
    }

    // Check for required files
    if (!ContainsFile(zipPath, "study-info.json")) {
        return Result(false, "Not a PEBL snapshot: missing study-info.json");
    }

    // Check for tests directory (at least one file starting with "tests/")
    std::vector<std::string> files;
    Result listResult = ListContents(zipPath, files);
    if (!listResult.success) {
        return listResult;
    }

    bool hasTests = false;
    for (const auto& file : files) {
        if (file.find("tests/") == 0) {
            hasTests = true;
            break;
        }
    }

    if (!hasTests) {
        return Result(false, "Not a PEBL snapshot: missing tests/ directory");
    }

    return Result(true);
}

// Helper: Create directories recursively
bool ZipExtractor::CreateDirectories(const std::string& path) {
    if (path.empty()) {
        return false;
    }

    struct stat st;
    if (stat(path.c_str(), &st) == 0) {
        // Path exists - check if it's a directory
        return S_ISDIR(st.st_mode);
    }

    // Find parent directory
    size_t lastSlash = path.find_last_of("/\\");
    if (lastSlash != std::string::npos && lastSlash > 0) {
        std::string parent = path.substr(0, lastSlash);
        if (!CreateDirectories(parent)) {
            return false;
        }
    }

    // Create this directory
#ifdef _WIN32
    return mkdir(path.c_str()) == 0;
#else
    return mkdir(path.c_str(), 0755) == 0;
#endif
}
