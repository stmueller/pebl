#include "FontCache.h"
#include <iostream>

namespace FontCache {

//
// FontCacheKey comparison operators
//

bool FontCacheKey::operator<(const FontCacheKey& other) const {
    // Compare filename first (most discriminating)
    if (filename != other.filename) {
        return filename < other.filename;
    }
    // Then style
    if (style != other.style) {
        return style < other.style;
    }
    // Finally size
    return size < other.size;
}

bool FontCacheKey::operator==(const FontCacheKey& other) const {
    return filename == other.filename &&
           style == other.style &&
           size == other.size;
}

//
// FontCacheManager singleton access
//

FontCacheManager& FontCacheManager::GetInstance() {
    static FontCacheManager instance;
    return instance;
}

//
// FontCacheManager::GetFont - Get or create font
//

TTF_Font* FontCacheManager::GetFont(const FontCacheKey& key, const std::string& full_path) {
    // Look for font in cache
    auto it = mCache.find(key);

    if (it != mCache.end()) {
        // Cache hit - font already loaded, increment ref count
        it->second.ref_count++;
        mCacheHits++;
        return it->second.ttf_font;
    }

    // Cache miss - need to load font from disk
    mCacheMisses++;

    // Load font directly using TTF_OpenFont (no RWops/buffer needed)
    // Modern systems can handle many open file descriptors
    TTF_Font* new_font = TTF_OpenFont(full_path.c_str(), key.size);

    if (!new_font) {
        std::cerr << "[FontCache] ERROR: Failed to load font: " << full_path
                  << " - " << TTF_GetError() << std::endl;
        return nullptr;
    }

    // Set font style (bold, italic, etc.)
    TTF_SetFontStyle(new_font, key.style);

    // Add to cache with ref count = 1
    mCache.insert(std::make_pair(key, CachedTTFFont(new_font)));

    return new_font;
}

//
// FontCacheManager::ReleaseFont - Release font reference
//

void FontCacheManager::ReleaseFont(const FontCacheKey& key) {
    auto it = mCache.find(key);

    if (it == mCache.end()) {
        // This shouldn't happen if PlatformFont is correctly managing keys
        std::cerr << "[FontCache] WARNING: Attempted to release non-cached font: "
                  << key.filename << " (style=" << key.style << ", size=" << key.size << ")"
                  << std::endl;
        return;
    }

    // Decrement ref count
    it->second.ref_count--;

    #ifdef FONT_CACHE_DEBUG
    std::cout << "[FontCache] RELEASE: " << key.filename
              << " (style=" << key.style << ", size=" << key.size << ")"
              << " refs=" << it->second.ref_count << std::endl;
    #endif

    // If no more references, close font and remove from cache
    if (it->second.ref_count <= 0) {
        if (it->second.ttf_font) {
            TTF_CloseFont(it->second.ttf_font);
        }

        #ifdef FONT_CACHE_DEBUG
        std::cout << "[FontCache] DELETED: " << key.filename
                  << " (style=" << key.style << ", size=" << key.size << ")"
                  << " cache_size=" << (mCache.size() - 1) << std::endl;
        #endif

        mCache.erase(it);
    }
}

//
// FontCacheManager::GetStats - Get cache statistics
//

void FontCacheManager::GetStats(int& unique_fonts, int& total_refs,
                                 int& cache_hits, int& cache_misses) {
    unique_fonts = mCache.size();
    total_refs = 0;

    // Sum up all reference counts
    for (const auto& pair : mCache) {
        total_refs += pair.second.ref_count;
    }

    cache_hits = mCacheHits;
    cache_misses = mCacheMisses;
}

//
// FontCacheManager::PrintCache - Print cache contents for debugging
//

void FontCacheManager::PrintCache() {
    std::cout << "\n=== Font Cache Contents ===" << std::endl;
    std::cout << "Unique fonts in cache: " << mCache.size() << std::endl;
    std::cout << "Total cache hits: " << mCacheHits << std::endl;
    std::cout << "Total cache misses: " << mCacheMisses << std::endl;

    if (mCache.empty()) {
        std::cout << "(cache is empty)" << std::endl;
    } else {
        std::cout << "\nCached fonts:" << std::endl;
        for (const auto& pair : mCache) {
            std::cout << "  " << pair.first.filename
                      << " (style=" << pair.first.style
                      << ", size=" << pair.first.size
                      << ") refs=" << pair.second.ref_count << std::endl;
        }
    }
    std::cout << "==========================\n" << std::endl;
}

//
// FontCacheManager destructor - Clean up any remaining fonts
//

FontCacheManager::~FontCacheManager() {
    #ifdef FONT_CACHE_DEBUG
    if (!mCache.empty()) {
        std::cout << "[FontCache] Destructor: Cleaning up " << mCache.size()
                  << " remaining fonts" << std::endl;
    }
    #endif

    // Close all remaining fonts
    for (auto& pair : mCache) {
        if (pair.second.ttf_font) {
            TTF_CloseFont(pair.second.ttf_font);
        }

        #ifdef FONT_CACHE_DEBUG
        std::cout << "[FontCache] Destructor: Closed " << pair.first.filename
                  << " (refs=" << pair.second.ref_count << ")" << std::endl;
        #endif
    }

    mCache.clear();
}

} // namespace FontCache
