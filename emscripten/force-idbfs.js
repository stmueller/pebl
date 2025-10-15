// Force IDBFS to be included in the build
// This library file ensures IDBFS is not tree-shaken out

mergeInto(LibraryManager.library, {
  // Dummy function to force IDBFS inclusion
  force_idbfs_load: function() {
    // Reference IDBFS to prevent tree-shaking
    if (typeof IDBFS !== 'undefined') {
      return 1;
    }
    return 0;
  }
});

// Also register IDBFS filesystem at module initialization
if (typeof FS !== 'undefined' && typeof IDBFS !== 'undefined') {
  try {
    if (!FS.filesystems.IDBFS) {
      FS.filesystems.IDBFS = IDBFS;
    }
  } catch (e) {
    console.warn('Could not register IDBFS:', e);
  }
}
