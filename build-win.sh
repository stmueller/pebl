#!/bin/bash
export PATH="/c/msys64/mingw64/bin:$PATH"
export TMPDIR="C:/Users/shanem/AppData/Local/Temp"
export TMP="C:/Users/shanem/AppData/Local/Temp"
export TEMP="C:/Users/shanem/AppData/Local/Temp"
cd /c/Users/shanem/Documents/pebl
/c/msys64/usr/bin/make.exe -j 10 -f Makefile-win.mak main
