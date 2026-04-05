// OpenScalesBrowser.h - Browse and download scales from OpenScales repository
// Copyright (c) 2026 Shane T. Mueller
// Licensed under GPL

#ifndef OPENSCALES_BROWSER_H
#define OPENSCALES_BROWSER_H

#include <string>
#include <vector>
#include <functional>

struct OpenScalesEntry {
    std::string code;
    std::string name;
    std::string domain;
    std::string description;
    std::string license;
    std::string url;
    int n_items;
    std::vector<std::string> languages;
    std::string repo;    // which repository (openscales, private, phenx, restricted)
    bool isLocal;        // already downloaded to workspace
};

class OpenScalesBrowser {
public:
    OpenScalesBrowser();

    // Show the browser dialog. scalesDir is workspace/scales/ path.
    void Show(const std::string& scalesDir);

    // Render the browser UI (call each frame when visible)
    void Render();

    // Is the dialog currently visible?
    bool IsVisible() const { return mVisible; }

    // Callback after a scale is downloaded (to refresh scale list)
    void SetOnDownload(std::function<void(const std::string&)> cb) { mOnDownload = cb; }

private:
    bool mVisible;
    std::string mScalesDir;
    std::vector<OpenScalesEntry> mCatalog;
    std::vector<std::string> mDomains;  // unique domains for filter
    int mSelectedDomain;    // -1 = all
    int mSelectedIndex;     // selected in filtered list
    char mFilterText[256];
    std::string mStatusMessage;
    bool mFetching;
    std::string mCachePath;

    std::function<void(const std::string&)> mOnDownload;

    // HTTP
    static std::string FetchURL(const std::string& url);
    bool FetchManifest();
    bool DownloadScale(const std::string& code);

    // Helpers
    void LoadCachedManifest();
    void ParseManifest(const std::string& json);
    void UpdateDomainList();
    void UpdateLocalStatus();
    std::vector<int> GetFilteredIndices();

    // OpenScales URLs
    static constexpr const char* BASE_URL = "https://openscales.net";
    struct ManifestSource {
        const char* path;      // URL path relative to BASE_URL
        const char* repo;      // repo name for display
        const char* scaleDir;  // scale directory path on server
    };
    static const ManifestSource MANIFESTS[];
    static const int NUM_MANIFESTS;
};

#endif // OPENSCALES_BROWSER_H
