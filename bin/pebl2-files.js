
  var Module = typeof Module != 'undefined' ? Module : {};

  Module['expectedDataFileDownloads'] ??= 0;
  Module['expectedDataFileDownloads']++;
  (() => {
    // Do not attempt to redownload the virtual filesystem data when in a pthread or a Wasm Worker context.
    var isPthread = typeof ENVIRONMENT_IS_PTHREAD != 'undefined' && ENVIRONMENT_IS_PTHREAD;
    var isWasmWorker = typeof ENVIRONMENT_IS_WASM_WORKER != 'undefined' && ENVIRONMENT_IS_WASM_WORKER;
    if (isPthread || isWasmWorker) return;
    var isNode = globalThis.process?.versions?.node && globalThis.process?.type != 'renderer';
    async function loadPackage(metadata) {

      var PACKAGE_PATH = '';
      if (typeof window === 'object') {
        PACKAGE_PATH = window['encodeURIComponent'](window.location.pathname.substring(0, window.location.pathname.lastIndexOf('/')) + '/');
      } else if (typeof process === 'undefined' && typeof location !== 'undefined') {
        // web worker
        PACKAGE_PATH = encodeURIComponent(location.pathname.substring(0, location.pathname.lastIndexOf('/')) + '/');
      }
      var PACKAGE_NAME = 'bin/pebl2.data';
      var REMOTE_PACKAGE_BASE = 'pebl2.data';
      var REMOTE_PACKAGE_NAME = Module['locateFile']?.(REMOTE_PACKAGE_BASE, '') ?? REMOTE_PACKAGE_BASE;
      var REMOTE_PACKAGE_SIZE = metadata['remote_package_size'];

      async function fetchRemotePackage(packageName, packageSize) {
        if (isNode) {
          var fsPromises = require('fs/promises');
          var contents = await fsPromises.readFile(packageName);
          return contents.buffer;
        }
        Module['dataFileDownloads'] ??= {};
        try {
          var response = await fetch(packageName);
        } catch (e) {
          throw new Error(`Network Error: ${packageName}`, {e});
        }
        if (!response.ok) {
          throw new Error(`${response.status}: ${response.url}`);
        }

        const chunks = [];
        const headers = response.headers;
        const total = Number(headers.get('Content-Length') ?? packageSize);
        let loaded = 0;

        Module['setStatus']?.('Downloading data...');
        const reader = response.body.getReader();

        while (1) {
          var {done, value} = await reader.read();
          if (done) break;
          chunks.push(value);
          loaded += value.length;
          Module['dataFileDownloads'][packageName] = {loaded, total};

          let totalLoaded = 0;
          let totalSize = 0;

          for (const download of Object.values(Module['dataFileDownloads'])) {
            totalLoaded += download.loaded;
            totalSize += download.total;
          }

          Module['setStatus']?.(`Downloading data... (${totalLoaded}/${totalSize})`);
        }

        const packageData = new Uint8Array(chunks.map((c) => c.length).reduce((a, b) => a + b, 0));
        let offset = 0;
        for (const chunk of chunks) {
          packageData.set(chunk, offset);
          offset += chunk.length;
        }
        return packageData.buffer;
      }

      var fetchPromise;
      var fetched = Module['getPreloadedPackage']?.(REMOTE_PACKAGE_NAME, REMOTE_PACKAGE_SIZE);

      if (!fetched) {
        // Note that we don't use await here because we want to execute the
        // the rest of this function immediately.
        fetchPromise = fetchRemotePackage(REMOTE_PACKAGE_NAME, REMOTE_PACKAGE_SIZE);
      }

    async function runWithFS(Module) {

      function assert(check, msg) {
        if (!check) throw new Error(msg);
      }
Module['FS_createPath']("/", "usr", true, true);
Module['FS_createPath']("/usr", "local", true, true);
Module['FS_createPath']("/usr/local", "share", true, true);
Module['FS_createPath']("/usr/local/share", "pebl2", true, true);
Module['FS_createPath']("/usr/local/share/pebl2", "media", true, true);
Module['FS_createPath']("/usr/local/share/pebl2/media", "fonts", true, true);
Module['FS_createPath']("/usr/local/share/pebl2/media", "images", true, true);
Module['FS_createPath']("/usr/local/share/pebl2/media", "text", true, true);
Module['FS_createPath']("/usr/local/share/pebl2", "pebl-lib", true, true);

      for (var file of metadata['files']) {
        var name = file['filename']
        Module['addRunDependency'](`fp ${name}`);
      }

      async function processPackageData(arrayBuffer) {
        assert(arrayBuffer, 'Loading data file failed.');
        assert(arrayBuffer.constructor.name === ArrayBuffer.name, 'bad input to processPackageData');
        var byteArray = new Uint8Array(arrayBuffer);
        var curr;
        // Reuse the bytearray from the XHR as the source for file reads.
          for (var file of metadata['files']) {
            var name = file['filename'];
            var data = byteArray.subarray(file['start'], file['end']);
            // canOwn this data in the filesystem, it is a slice into the heap that will never change
          Module['FS_createDataFile'](name, null, data, true, true, true);
          Module['removeRunDependency'](`fp ${name}`);
          }
          Module['removeRunDependency']('datafile_bin/pebl2.data');
      }
      Module['addRunDependency']('datafile_bin/pebl2.data');

      Module['preloadResults'] ??= {};

      Module['preloadResults'][PACKAGE_NAME] = {fromCache: false};
      if (!fetched) {
        fetched = await fetchPromise;
      }
      processPackageData(fetched);

    }
    if (Module['calledRun']) {
      runWithFS(Module);
    } else {
      (Module['preRun'] ??= []).push(runWithFS); // FS is not initialized yet, wait for it
    }

    }
    loadPackage({"files": [{"filename": "/test.pbl", "start": 0, "end": 500}, {"filename": "/usr/local/share/pebl2/media/fonts/COPYRIGHT.TXT", "start": 500, "end": 6454}, {"filename": "/usr/local/share/pebl2/media/fonts/DejaVuSans.ttf", "start": 6454, "end": 628734}, {"filename": "/usr/local/share/pebl2/media/fonts/DejaVuSansMono.ttf", "start": 628734, "end": 950258}, {"filename": "/usr/local/share/pebl2/media/fonts/DejaVuSerif.ttf", "start": 950258, "end": 1279166}, {"filename": "/usr/local/share/pebl2/media/images/buttonmiddle.png", "start": 1279166, "end": 1279392}, {"filename": "/usr/local/share/pebl2/media/images/buttonsideL.png", "start": 1279392, "end": 1279787}, {"filename": "/usr/local/share/pebl2/media/images/buttonsideR.png", "start": 1279787, "end": 1280182}, {"filename": "/usr/local/share/pebl2/media/images/down.png", "start": 1280182, "end": 1280761}, {"filename": "/usr/local/share/pebl2/media/images/frowney-big.png", "start": 1280761, "end": 1285909}, {"filename": "/usr/local/share/pebl2/media/images/frowney-small.png", "start": 1285909, "end": 1286828}, {"filename": "/usr/local/share/pebl2/media/images/launcher-bg.jpg", "start": 1286828, "end": 1805741}, {"filename": "/usr/local/share/pebl2/media/images/pebl.bmp", "start": 1805741, "end": 1851395}, {"filename": "/usr/local/share/pebl2/media/images/pebl.jpg", "start": 1851395, "end": 1853881}, {"filename": "/usr/local/share/pebl2/media/images/pebl.png", "start": 1853881, "end": 1863144}, {"filename": "/usr/local/share/pebl2/media/images/pebl2.png", "start": 1863144, "end": 1892362}, {"filename": "/usr/local/share/pebl2/media/images/pebl2.xcf", "start": 1892362, "end": 2006088}, {"filename": "/usr/local/share/pebl2/media/images/plus-sm.png", "start": 2006088, "end": 2014336}, {"filename": "/usr/local/share/pebl2/media/images/plus.png", "start": 2014336, "end": 2151229}, {"filename": "/usr/local/share/pebl2/media/images/plus.svg", "start": 2151229, "end": 2184538}, {"filename": "/usr/local/share/pebl2/media/images/pull.png", "start": 2184538, "end": 2184997}, {"filename": "/usr/local/share/pebl2/media/images/smiley-big.png", "start": 2184997, "end": 2190524}, {"filename": "/usr/local/share/pebl2/media/images/smiley-small.png", "start": 2190524, "end": 2191396}, {"filename": "/usr/local/share/pebl2/media/images/thumb.png", "start": 2191396, "end": 2192237}, {"filename": "/usr/local/share/pebl2/media/images/up.png", "start": 2192237, "end": 2192829}, {"filename": "/usr/local/share/pebl2/media/images/x-sm.png", "start": 2192829, "end": 2203691}, {"filename": "/usr/local/share/pebl2/media/images/x.png", "start": 2203691, "end": 2377725}, {"filename": "/usr/local/share/pebl2/media/images/x.svg", "start": 2377725, "end": 2411140}, {"filename": "/usr/local/share/pebl2/media/text/Consonants.txt", "start": 2411140, "end": 2411398}, {"filename": "/usr/local/share/pebl2/media/text/DigitNames.txt", "start": 2411398, "end": 2411610}, {"filename": "/usr/local/share/pebl2/media/text/Digits.txt", "start": 2411610, "end": 2411789}, {"filename": "/usr/local/share/pebl2/media/text/Letters.txt", "start": 2411789, "end": 2412064}, {"filename": "/usr/local/share/pebl2/media/text/Lowercase.txt", "start": 2412064, "end": 2412289}, {"filename": "/usr/local/share/pebl2/media/text/LowercaseConsonants.txt", "start": 2412289, "end": 2412514}, {"filename": "/usr/local/share/pebl2/media/text/LowercaseVowels.txt", "start": 2412514, "end": 2412703}, {"filename": "/usr/local/share/pebl2/media/text/Uppercase.txt", "start": 2412703, "end": 2412928}, {"filename": "/usr/local/share/pebl2/media/text/UppercaseConsonants.txt", "start": 2412928, "end": 2413153}, {"filename": "/usr/local/share/pebl2/media/text/UppercaseVowels.txt", "start": 2413153, "end": 2413342}, {"filename": "/usr/local/share/pebl2/media/text/Vowels.txt", "start": 2413342, "end": 2413532}, {"filename": "/usr/local/share/pebl2/media/text/torontowordpool.txt", "start": 2413532, "end": 2420916}, {"filename": "/usr/local/share/pebl2/pebl-lib/Debug.pbl", "start": 2420916, "end": 2422244}, {"filename": "/usr/local/share/pebl2/pebl-lib/Design.pbl", "start": 2422244, "end": 2440623}, {"filename": "/usr/local/share/pebl2/pebl-lib/EM.pbl", "start": 2440623, "end": 2451121}, {"filename": "/usr/local/share/pebl2/pebl-lib/Graphics.pbl", "start": 2451121, "end": 2479460}, {"filename": "/usr/local/share/pebl2/pebl-lib/HTML.pbl", "start": 2479460, "end": 2480675}, {"filename": "/usr/local/share/pebl2/pebl-lib/Math.pbl", "start": 2480675, "end": 2491661}, {"filename": "/usr/local/share/pebl2/pebl-lib/Taguchi.pbl", "start": 2491661, "end": 2493651}, {"filename": "/usr/local/share/pebl2/pebl-lib/Transfer.pbl", "start": 2493651, "end": 2495191}, {"filename": "/usr/local/share/pebl2/pebl-lib/UI.pbl", "start": 2495191, "end": 2539141}, {"filename": "/usr/local/share/pebl2/pebl-lib/Utility.pbl", "start": 2539141, "end": 2581050}, {"filename": "/usr/local/share/pebl2/pebl-lib/cm.txt", "start": 2581050, "end": 2581497}, {"filename": "/usr/local/share/pebl2/pebl-lib/combinedatafiles.pbl", "start": 2581497, "end": 2595611}, {"filename": "/usr/local/share/pebl2/pebl-lib/converttranslations.pbl", "start": 2595611, "end": 2596237}, {"filename": "/usr/local/share/pebl2/pebl-lib/customlauncher.pbl", "start": 2596237, "end": 2616558}, {"filename": "/usr/local/share/pebl2/pebl-lib/expbuilder.pbl", "start": 2616558, "end": 2635642}, {"filename": "/usr/local/share/pebl2/pebl-lib/followdebug.pbl", "start": 2635642, "end": 2636537}, {"filename": "/usr/local/share/pebl2/pebl-lib/out.html", "start": 2636537, "end": 2636963}, {"filename": "/usr/local/share/pebl2/pebl-lib/sample1.css", "start": 2636963, "end": 2639472}, {"filename": "/usr/local/share/pebl2/pebl-lib/translatetest.pbl", "start": 2639472, "end": 2647879}], "remote_package_size": 2647879});

  })();
