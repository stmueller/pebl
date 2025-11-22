#!/bin/bash
export PATH="/c/msys64/mingw64/bin:$PATH"
export TMPDIR=/tmp
export TMP=/tmp
export TEMP=/tmp
cd /c/Users/shanem/Documents/dev/pebl
/c/msys64/usr/bin/make.exe -j 1 -f Makefile-win.mak main
