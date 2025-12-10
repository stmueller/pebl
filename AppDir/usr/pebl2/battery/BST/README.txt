The Bivalent Shape Task Version 1.0 (Esposito et al., in press)

ABOUT:

This archive enables the Bivalent Shape Task (BST) to be run
on your computer.  To do so, you must first have installed 
PEBL version 0.13 or later (available at http://pebl.sf.net).

Authors:
Shane T. Mueller (shanem@mtu.edu) & Alena Esposito (alenaesposito@gmail.com)


REFERENCES:

Please reference the following:

Esposito et al.  (in press). Interference Suppression
 vs. Response Inhibition: An Explanation for the Absence of a
 Bilingual Advantage in Preschoolers' Stroop Task
 Performance. Cognitive Development. DOI: 


AVAILABILITY: 
This file is part of the PEBL project and the PEBL Test Battery
http://pebl.sourceforge.net



INSTALLATION:
To run this test, you must first install PEBL on your computer.  Then,
place this file and related media files in a subdirectory of the 
PEBL experiment directory (e.g., Documents\pebl-exp-0.14\)  Finally,
run the PEBL launcher and navigate to this file in order to run the 
test.  Data will be saved in the data\ sub-directory of the folder this 
file was saved in, in a file named BST-XXX.csv, where XXX 
is the participant code entered in the PEBL Launcher.

HELP:
For help installing, running, or interpreting data from this test,
please email the pebl-list: pebl-list@lists.sourceforge.net



MODIFYING:
A number of options exist for modifying the program.  To do so, open the BST.pbl 
file in a text editor.  Editable parameters are near the top of the program,
and include:


 gUseRandom <- 0     ##set to 1 to make each test different
 gResponseOffset <- 100  ##Pixels from center line that the responses are placed
 gResponseY     <- gVideoHeight-150 ##Height of responses. 
 gResponseTimeLimit <- 3000 ##Timeout in ms.
 gUseVisualFeedback <- 0 ##Give visual feedback of 'correct' and 'incorrect'
 gUseAudioFeedbackAlways <- 0
 gUseAudioFeedbackPractice <- 1
 numTrials <- 20  ##How many trials per block?
 gUseMouse <- 1   ##Use mouse/touch input.

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


