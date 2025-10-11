## Converts svg pix to .png
##
 pathname="png/"
 ext=".png"
 for i in svg/*.svg
   do 
   base=`echo "$i" | sed -e 's/svg\///' -e 's/\.svg//'`
   echo "Converting : $i to $pathname$base$ext"
   inkscape --without-gui $i  --export-png="./$pathname$base$ext"
  
 done

 
