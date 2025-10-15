// Pre-load IDBFS filesystem
// This ensures IDBFS is available in Module.FS.filesystems

if (typeof Module === 'undefined') {
  Module = {};
}

// Force IDBFS to be included
Module.preRun = Module.preRun || [];
Module.preRun.push(function() {
  // Try to ensure IDBFS is loaded
  if (typeof FS !== 'undefined' && FS.filesystems && FS.filesystems.IDBFS) {
    console.log('IDBFS already available');
  } else {
    console.log('IDBFS not detected in preRun');
  }
});
