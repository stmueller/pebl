#!/bin/bash
export PATH="/c/msys64/mingw64/bin:$PATH"
export TMPDIR=/tmp
export TMP=/tmp
export TEMP=/tmp
cd /c/Users/shanem/Documents/pebl
/c/msys64/usr/bin/make.exe -j 1 -f Makefile-win.mak main > build.log 2>&1
echo "Exit code: $?"
tail -100 build.log
