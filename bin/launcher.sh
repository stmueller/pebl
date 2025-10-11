#!/bin/sh
## This is a demo launcher script written using 
## zenity, the Gnome Dialog Utility.  
## Edit to run the experiment of your choice, or even
## a string of experiments

sub=`zenity --entry --title="PEBL Launcher" --text="Enter Subject Number"  --entry-text="00"`

/usr/local/bin/pebl ../demo/test.pbl -v "$sub"