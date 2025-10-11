#!/bin/sh

for i in *
do
cd $i/translations
#pebl2 converttranslations.pbl
svn add *.json
#rm *.csv
cd ~/Dropbox/Research/pebl/branches/sdl2/battery
done
