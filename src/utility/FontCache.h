#ifndef __FONTCACHE_H__
#define __FONTCACHE_H__

#include <string>
#include <map>
#include "SDL.h"
#include "SDL_ttf.h"

namespace FontCache {

/// Cache key for font lookup (excludes color - that's the optimization!)
/// Multiple PFont objects with same {filename, style, size} but different colors
/// can share a single TTF_Font object since SDL_TTF applies colors at render time.
struct FontCacheKey {
    std::string filename;  // Font filename (e.g., "DejaVuSans.ttf")
    int style;             // Font style (PFS_Normal, PFS_Bold, PFS_Italic, etc.)
    int size;              // Point size (e.g., 24)

    /// Comparison operator for std::map ordering
    bool operator<(const FontCacheKey& other) const;

    /// Equality operator for debugging/testing
    bool operator==(const FontCacheKey& other) const;
};

/// Cached TTF_Font with reference counting
/// When ref_count reaches 0, TTF_Font is closed and removed from cache
struct CachedTTFFont {
    TTF_Font* ttf_font;  // Shared SDL_TTF font object
    int ref_count;       // Number of PlatformFont objects using this font

    CachedTTFFont(TTF_Font* f) : ttf_font(f), ref_count(1) {}
};

/// Singleton font cache manager
/// Eliminates duplicate TTF_Font objects for fonts that differ only in color
///
/// Example:
///   font_red  = MakeFont("DejaVuSans.ttf", normal, 24, red, white)
///   font_blue = MakeFont("DejaVuSans.ttf", normal, 24, blue, white)
///
///   Without cache: 2 separate TTF_Font objects (~66KB)
///   With cache: 1 shared TTF_Font object (~33KB) - 50% memory savings
///
/// The cache automatically manages TTF_Font lifecycle via reference counting.
class FontCacheManager {
public:
    /// Get singleton instance
    static FontCacheManager& GetInstance();

    /// Get or create TTF_Font for given key
    /// @param key Font cache key (filename, style, size)
    /// @param full_path Full filesystem path to font file
    /// @return TTF_Font pointer (may be shared), or nullptr on error
    ///
    /// If font already in cache, increments ref count and returns existing font.
    /// Otherwise, loads font from disk, adds to cache, and returns new font.
    TTF_Font* GetFont(const FontCacheKey& key, const std::string& full_path);

    /// Release font reference
    /// @param key Font cache key
    ///
    /// Decrements ref count. If ref count reaches 0, closes TTF_Font and
    /// removes from cache. Should be called from PlatformFont destructor.
    void ReleaseFont(const FontCacheKey& key);

    /// Get cache statistics for debugging
    /// @param unique_fonts Number of unique TTF_Font objects in cache
    /// @param total_refs Total reference count across all fonts
    /// @param cache_hits Number of cache hits since program start
    /// @param cache_misses Number of cache misses since program start
    void GetStats(int& unique_fonts, int& total_refs, int& cache_hits, int& cache_misses);

    /// Print cache contents to stdout (for debugging)
    void PrintCache();

private:
    /// Private constructor (singleton pattern)
    FontCacheManager() : mCacheHits(0), mCacheMisses(0) {}

    /// Destructor cleans up any remaining fonts
    ~FontCacheManager();

    /// Prevent copying (singleton)
    FontCacheManager(const FontCacheManager&) = delete;
    FontCacheManager& operator=(const FontCacheManager&) = delete;

    /// Font cache: key -> cached font with ref count
    std::map<FontCacheKey, CachedTTFFont> mCache;

    /// Statistics
    int mCacheHits;    // Number of successful cache lookups
    int mCacheMisses;  // Number of new font loads
};

} // namespace FontCache

#endif // __FONTCACHE_H__
