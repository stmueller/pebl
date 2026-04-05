// OpenScalesBrowser.cpp - Browse and download scales from OpenScales repository
// Copyright (c) 2026 Shane T. Mueller
// Licensed under GPL

#include "OpenScalesBrowser.h"
#include <imgui.h>
#include <json.hpp>
#include <curl/curl.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <set>
#include <filesystem>
#include <ctime>
#include <cstring>

namespace fs = std::filesystem;
using json = nlohmann::json;

// Manifest sources — fetched in order, merged into one catalog
const OpenScalesBrowser::ManifestSource OpenScalesBrowser::MANIFESTS[] = {
    { "/manifest.json",            "openscales", "/scales/openscales/" },
    { "/manifest_phenx.json",      "phenx",      "/scales/phenx/" },
    { "/manifest_restricted.json", "restricted",  "/scales/restricted/" },
};
const int OpenScalesBrowser::NUM_MANIFESTS = sizeof(MANIFESTS) / sizeof(MANIFESTS[0]);

// ── curl callback ────────────────────────────────────────────────────────────

static size_t CurlWriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    std::string* str = static_cast<std::string*>(userp);
    str->append(static_cast<char*>(contents), size * nmemb);
    return size * nmemb;
}

// ── Constructor ──────────────────────────────────────────────────────────────

OpenScalesBrowser::OpenScalesBrowser()
    : mVisible(false)
    , mSelectedDomain(-1)
    , mSelectedIndex(-1)
    , mFetching(false)
{
    std::memset(mFilterText, 0, sizeof(mFilterText));
}

// ── Public interface ─────────────────────────────────────────────────────────

void OpenScalesBrowser::Show(const std::string& scalesDir) {
    mScalesDir = scalesDir;
    mVisible = true;
    mSelectedIndex = -1;
    mStatusMessage = "";

    // Determine cache path (next to scales dir)
    mCachePath = scalesDir + "/../openscales_manifest.json";

    // Load cached manifest if available
    if (mCatalog.empty()) {
        LoadCachedManifest();
    }
    UpdateLocalStatus();
}

// ── HTTP fetch ───────────────────────────────────────────────────────────────

std::string OpenScalesBrowser::FetchURL(const std::string& url) {
    std::string response;
    CURL* curl = curl_easy_init();
    if (!curl) return "";

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "PEBL-Launcher/2.4");

    CURLcode res = curl_easy_perform(curl);
    long httpCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK || httpCode != 200) {
        return "";
    }
    return response;
}

bool OpenScalesBrowser::FetchManifest() {
    mStatusMessage = "Fetching catalogs from OpenScales...";
    mFetching = true;
    mCatalog.clear();

    int totalFetched = 0;
    int totalFailed = 0;

    // Fetch each manifest and merge
    json allEntries = json::array();

    for (int m = 0; m < NUM_MANIFESTS; m++) {
        std::string url = std::string(BASE_URL) + MANIFESTS[m].path;
        std::string body = FetchURL(url);

        if (body.empty()) {
            totalFailed++;
            printf("  Failed to fetch %s\n", url.c_str());
            continue;
        }

        try {
            json arr = json::parse(body);
            if (arr.is_array()) {
                // Tag each entry with its repo
                for (auto& entry : arr) {
                    entry["_repo"] = MANIFESTS[m].repo;
                    entry["_scale_dir"] = MANIFESTS[m].scaleDir;
                    allEntries.push_back(entry);
                }
                totalFetched += (int)arr.size();
            }
        } catch (...) {
            totalFailed++;
            printf("  Failed to parse %s\n", url.c_str());
        }
    }

    mFetching = false;

    if (allEntries.empty()) {
        mStatusMessage = "Failed to fetch any catalogs. Check your internet connection.";
        return false;
    }

    // Cache merged result to disk
    std::string merged = allEntries.dump(2);
    try {
        std::ofstream out(mCachePath);
        out << merged;
        out.close();
    } catch (...) {}

    ParseManifest(merged);
    UpdateLocalStatus();
    mStatusMessage = "Catalog updated: " + std::to_string(mCatalog.size()) + " scales"
                   + (totalFailed > 0 ? " (" + std::to_string(totalFailed) + " source(s) unavailable)" : "")
                   + ".";
    return true;
}

bool OpenScalesBrowser::DownloadScale(const std::string& code) {
    // Find the repo for this scale
    std::string scaleDir = "/scales/openscales/";  // default
    for (const auto& e : mCatalog) {
        if (e.code == code && !e.repo.empty()) {
            // Look up the scale directory for this repo
            for (int m = 0; m < NUM_MANIFESTS; m++) {
                if (e.repo == MANIFESTS[m].repo) {
                    scaleDir = MANIFESTS[m].scaleDir;
                    break;
                }
            }
            break;
        }
    }
    std::string url = std::string(BASE_URL) + scaleDir + code + "/" + code + ".osd";
    mStatusMessage = "Downloading " + code + "...";

    std::string body = FetchURL(url);
    if (body.empty()) {
        mStatusMessage = "Failed to download " + code + ".";
        return false;
    }

    // Validate it's valid JSON
    try {
        json::parse(body);
    } catch (...) {
        mStatusMessage = "Downloaded file is not valid JSON.";
        return false;
    }

    // Save to workspace/scales/{code}/{code}.osd
    std::string dir = mScalesDir + "/" + code;
    std::string path = dir + "/" + code + ".osd";

    try {
        fs::create_directories(dir);
        std::ofstream out(path);
        out << body;
        out.close();
    } catch (const std::exception& e) {
        mStatusMessage = std::string("Failed to save: ") + e.what();
        return false;
    }

    mStatusMessage = "Downloaded " + code + " successfully.";

    // Update local status
    for (auto& entry : mCatalog) {
        if (entry.code == code) {
            entry.isLocal = true;
            break;
        }
    }

    // Notify callback
    if (mOnDownload) {
        mOnDownload(code);
    }

    return true;
}

// ── Cache / parse ────────────────────────────────────────────────────────────

void OpenScalesBrowser::LoadCachedManifest() {
    if (!fs::exists(mCachePath)) {
        mStatusMessage = "No cached catalog. Click 'Refresh' to download.";
        return;
    }

    try {
        std::ifstream in(mCachePath);
        std::stringstream buf;
        buf << in.rdbuf();
        ParseManifest(buf.str());
        mStatusMessage = "Loaded cached catalog: " + std::to_string(mCatalog.size()) + " scales.";
    } catch (...) {
        mStatusMessage = "Failed to read cached catalog.";
    }
}

void OpenScalesBrowser::ParseManifest(const std::string& jsonStr) {
    mCatalog.clear();

    try {
        json arr = json::parse(jsonStr);
        if (!arr.is_array()) return;

        for (const auto& entry : arr) {
            OpenScalesEntry e;
            e.code = entry.value("code", "");
            e.name = entry.value("name", "");
            e.domain = entry.value("domain", "");
            e.description = entry.value("description", "");
            e.license = entry.value("license", "");
            e.url = entry.value("url", "");
            e.n_items = entry.value("n_items", 0);
            if (e.n_items == 0) {
                e.n_items = entry.value("items_count", 0);
            }
            e.repo = entry.value("_repo", entry.value("repo", "openscales"));
            e.isLocal = false;

            if (entry.contains("languages") && entry["languages"].is_array()) {
                for (const auto& lang : entry["languages"]) {
                    e.languages.push_back(lang.get<std::string>());
                }
            }

            if (!e.code.empty()) {
                mCatalog.push_back(e);
            }
        }
    } catch (...) {
        mStatusMessage = "Failed to parse catalog JSON.";
    }

    UpdateDomainList();
}

void OpenScalesBrowser::UpdateDomainList() {
    std::set<std::string> domSet;
    for (const auto& e : mCatalog) {
        if (!e.domain.empty()) domSet.insert(e.domain);
    }
    mDomains.assign(domSet.begin(), domSet.end());
    std::sort(mDomains.begin(), mDomains.end());
}

void OpenScalesBrowser::UpdateLocalStatus() {
    for (auto& e : mCatalog) {
        std::string path = mScalesDir + "/" + e.code + "/" + e.code + ".osd";
        e.isLocal = fs::exists(path);
    }
}

std::vector<int> OpenScalesBrowser::GetFilteredIndices() {
    std::vector<int> result;
    std::string filterLower(mFilterText);
    std::transform(filterLower.begin(), filterLower.end(), filterLower.begin(), ::tolower);

    for (int i = 0; i < (int)mCatalog.size(); i++) {
        const auto& e = mCatalog[i];

        // Domain filter
        if (mSelectedDomain >= 0 && mSelectedDomain < (int)mDomains.size()) {
            if (e.domain != mDomains[mSelectedDomain]) continue;
        }

        // Text filter
        if (!filterLower.empty()) {
            std::string haystack = e.code + " " + e.name + " " + e.description + " " + e.domain;
            std::transform(haystack.begin(), haystack.end(), haystack.begin(), ::tolower);
            if (haystack.find(filterLower) == std::string::npos) continue;
        }

        result.push_back(i);
    }
    return result;
}

// ── ImGui Render ─────────────────────────────────────────────────────────────

void OpenScalesBrowser::Render() {
    if (!mVisible) return;

    ImGui::SetNextWindowSize(ImVec2(900, 600), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Browse OpenScales", &mVisible, ImGuiWindowFlags_NoCollapse)) {
        ImGui::End();
        return;
    }

    // ── Top bar: domain filter + text filter + refresh ───────────────────────
    ImGui::PushItemWidth(150);
    const char* domainPreview = (mSelectedDomain < 0) ? "All Domains" : mDomains[mSelectedDomain].c_str();
    if (ImGui::BeginCombo("##Domain", domainPreview)) {
        if (ImGui::Selectable("All Domains", mSelectedDomain < 0)) {
            mSelectedDomain = -1;
        }
        for (int i = 0; i < (int)mDomains.size(); i++) {
            if (ImGui::Selectable(mDomains[i].c_str(), mSelectedDomain == i)) {
                mSelectedDomain = i;
                mSelectedIndex = -1;
            }
        }
        ImGui::EndCombo();
    }
    ImGui::PopItemWidth();

    ImGui::SameLine();
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - 100);
    ImGui::InputTextWithHint("##Filter", "Search scales...", mFilterText, sizeof(mFilterText));
    ImGui::PopItemWidth();

    ImGui::SameLine();
    if (ImGui::Button("Refresh")) {
        FetchManifest();
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Download latest scale catalog from OpenScales");
    }

    ImGui::Separator();

    // ── Get filtered list ────────────────────────────────────────────────────
    auto filtered = GetFilteredIndices();

    // ── Left panel: scale list ───────────────────────────────────────────────
    float listWidth = 350;
    ImGui::BeginChild("ScaleListPanel", ImVec2(listWidth, -ImGui::GetFrameHeightWithSpacing() - 4), true);

    ImGui::Text("%d scales", (int)filtered.size());
    ImGui::Separator();

    if (ImGui::BeginTable("ScaleTable", 3,
        ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_Resizable)) {
        ImGui::TableSetupColumn("Code", ImGuiTableColumnFlags_WidthFixed, 70);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Items", ImGuiTableColumnFlags_WidthFixed, 40);
        ImGui::TableHeadersRow();

        for (int fi = 0; fi < (int)filtered.size(); fi++) {
            int idx = filtered[fi];
            const auto& e = mCatalog[idx];

            ImGui::TableNextRow();
            ImGui::PushID(idx);

            // Highlight locally available scales
            if (e.isLocal) {
                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0,
                    ImGui::GetColorU32(ImVec4(0.15f, 0.35f, 0.15f, 0.3f)));
            }

            ImGui::TableSetColumnIndex(0);
            bool selected = (mSelectedIndex == idx);
            if (ImGui::Selectable(e.code.c_str(), selected,
                ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap)) {
                mSelectedIndex = idx;
            }

            ImGui::TableSetColumnIndex(1);
            ImGui::TextUnformatted(e.name.c_str());

            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%d", e.n_items);

            ImGui::PopID();
        }

        ImGui::EndTable();
    }

    ImGui::EndChild();

    // ── Right panel: details ─────────────────────────────────────────────────
    ImGui::SameLine();
    ImGui::BeginChild("ScaleDetailPanel", ImVec2(0, -ImGui::GetFrameHeightWithSpacing() - 4), true);

    if (mSelectedIndex >= 0 && mSelectedIndex < (int)mCatalog.size()) {
        const auto& e = mCatalog[mSelectedIndex];

        ImGui::TextWrapped("%s", e.name.c_str());
        ImGui::TextDisabled("%s", e.code.c_str());
        ImGui::Spacing();

        if (!e.domain.empty()) {
            ImGui::Text("Domain: %s", e.domain.c_str());
        }
        if (!e.repo.empty() && e.repo != "openscales") {
            ImGui::SameLine();
            ImGui::TextDisabled("  [%s]", e.repo.c_str());
        }
        ImGui::Text("Items: %d", e.n_items);

        if (!e.languages.empty()) {
            std::string langs;
            for (size_t i = 0; i < e.languages.size(); i++) {
                if (i) langs += ", ";
                langs += e.languages[i];
            }
            ImGui::Text("Languages: %s", langs.c_str());
        }

        if (!e.license.empty()) {
            ImGui::Text("License: %s", e.license.c_str());
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (!e.description.empty()) {
            ImGui::TextWrapped("%s", e.description.c_str());
            ImGui::Spacing();
        }

        if (!e.url.empty()) {
            ImGui::TextDisabled("URL: %s", e.url.c_str());
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (e.isLocal) {
            ImGui::TextColored(ImVec4(0.3f, 0.8f, 0.3f, 1.0f), "Already downloaded");
            if (ImGui::Button("Re-download")) {
                DownloadScale(e.code);
            }
        } else {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.5f, 0.8f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.6f, 0.9f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.15f, 0.4f, 0.7f, 1.0f));
            if (ImGui::Button("Download Scale", ImVec2(-1, 30))) {
                DownloadScale(e.code);
            }
            ImGui::PopStyleColor(3);
        }
    } else {
        ImGui::TextDisabled("Select a scale to see details.");
    }

    ImGui::EndChild();

    // ── Status bar ───────────────────────────────────────────────────────────
    if (!mStatusMessage.empty()) {
        ImGui::TextDisabled("%s", mStatusMessage.c_str());
    }

    ImGui::End();
}
