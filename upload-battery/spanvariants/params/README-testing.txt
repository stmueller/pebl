TESTING PARAMETER FILES FOR SPANVARIANTS
=========================================

These .par.json files make it easy to test specific variants without running the full battery.

USAGE:
------
Run PEBL with the -v option to load a parameter file:

  bin/pebl2 battery/spanvariants/spanvariants.pbl -v parfile=battery/spanvariants/params/test-forward-digit.par.json

Or from the spanvariants directory:

  ../../bin/pebl2 spanvariants.pbl -v parfile=params/test-forward-digit.par.json


AVAILABLE TEST FILES:
---------------------

1. test-forward-digit.par.json
   - Tests only forward digit span
   - Quick: upperbound=6, perlength=2
   - Good for basic functionality testing

2. test-letter-spans.par.json
   - Tests forward and backward letter span
   - Uses letters from letter_set parameter
   - Tests audio support for letters

3. test-ordering.par.json
   - Tests all three ordering tasks (digit, letter, mixed)
   - Quick version with reduced bounds

4. test-mixed-spans.par.json
   - Tests forward and backward mixed digit/letter spans
   - Tests the most complex stimulus type

5. test-video-only.par.json
   - Tests with UseAudio=0 (video presentation only)
   - Good for testing without audio files

6. test-audio-only.par.json
   - Tests with UseAudio=2 (audio-only presentation)
   - Harder task, tests auditory working memory

7. test-staircase.par.json
   - Tests staircase (adaptive) method (version=3)
   - Only 10 trials, quickly finds span

8. test-all-quick.par.json
   - Runs all 9 variants with reduced parameters
   - Only 1 trial per length, shorter ISI/ITI
   - Good for full system testing


CUSTOMIZING:
------------
You can create your own .par.json file by copying one of these and modifying:

- Set any task's _order parameter to 0 to skip it
- Reduce upperbound/lowerbound to shorten testing
- Reduce perlength to do fewer trials per length
- Change UseAudio: 0=video, 1=audio+video, 2=audio-only
- Change version: 1=low-to-high, 2=high-to-low, 3=staircase


EXAMPLES:
---------

Test just forward digit span with audio+video:
  -v parfile=params/test-forward-digit.par.json

Test ordering tasks with audio only:
  -v parfile=params/test-ordering.par.json -v UseAudio=2

Test all variants quickly:
  -v parfile=params/test-all-quick.par.json

Override subject number:
  -v parfile=params/test-forward-digit.par.json -v subnum=999
