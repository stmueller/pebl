#!/bin/sh
## Converts svg pix to .png
## requires inkscape to be installed.
 pathname="png/"
 ext=".png"
 for i in svg/*
   do 
   base=`echo "$i" | sed -e 's/svg\///' -e 's/\.svg//'`
   echo "Converting : $i to $pathname$base$ext"
   inkscape --without-gui $i  --export-png="./$pathname$base$ext"
  
 done

 