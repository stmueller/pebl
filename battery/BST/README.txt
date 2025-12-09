The Bivalent Shape Task Version 1.1 (Esposito et al., 2013)

ABOUT:

This archive enables the Bivalent Shape Task (BST) to be run
on your computer.  To do so, you must first have installed
PEBL version 2.2 or later (available at http://pebl.sf.net).

Authors:
Shane T. Mueller (shanem@mtu.edu) & Alena Esposito (alenaesposito@gmail.com)


REFERENCES:

Please reference the following:

Esposito, A. G., Baker-Ward, L., & Mueller, S. T. (2013). Interference
suppression vs. response inhibition: An explanation for the absence of a
bilingual advantage in preschoolers' Stroop task performance.
Cognitive Development, 28(4), 354-363.
http://www.sciencedirect.com/science/article/pii/S0885201413000518


AVAILABILITY:
This file is part of the PEBL project and the PEBL Test Battery
http://pebl.sourceforge.net



INSTALLATION:
To run this test, you must first install PEBL on your computer. Then,
place this file and related media files in a subdirectory of the
PEBL experiment directory (e.g., Documents\pebl-exp-0.14\) Finally,
run the PEBL launcher and navigate to this file in order to run the
test. Data will be saved in the data\ sub-directory of the folder this
file was saved in, in a file named BST-XXX.csv, where XXX
is the participant code entered in the PEBL Launcher.

HELP:
For help installing, running, or interpreting data from this test,
please email the pebl-list: pebl-list@lists.sourceforge.net



MODIFYING:
A number of options can be modified via parameter files. Create a file named
BST.pbl.par (CSV format) or params/BST.pbl.par.json (JSON format) with the
following parameters:

  userandom - Set to 1 to randomize trials (default: 1)
  timelimit - Response timeout in milliseconds (default: 3000)
  usevideofeedback - Show visual feedback for correct/incorrect (default: 1)
  useaudiofeedbackalways - Play audio feedback on all trials (default: 0)
  useaudiofeedbackpractice - Play audio feedback during practice (default: 1)
  numtrials - Number of trials per block (default: 20)
  responsemode - Input method (default: "keyboardShift")

Response modes:
  - keyboardShift: Left/Right Shift keys
  - keyboardSafe: Z and / keys
  - mousebutton: Left/Right mouse buttons
  - mousetarget: Click on footer images
  - touchtarget: Touch footer images (for touchscreens)

Example JSON parameter file (params/BST.pbl.par.json):
{
  "userandom": 1,
  "timelimit": 3000,
  "usevideofeedback": 1,
  "useaudiofeedbackalways": 0,
  "useaudiofeedbackpractice": 1,
  "numtrials": 20,
  "responsemode": "keyboardShift"
}


VERSION HISTORY:

Version 1.1 (2025):
- Migrated to Layout & Response System for unified response handling
- Added support for multiple response modes (keyboard, mouse, touch)
- Added JSON parameter file support with schema validation
- Added translations for 7 languages (EN, DE, ES, IT, LT, NL, PT)
- Improved visual layout with better spacing and positioning
- Fixed Linux/Wayland mouse position issues in touchtarget mode
- Modernized code for PEBL 2.2 compatibility

Version 1.0 (2013):
- Initial release


LICENSE:

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.


