

## Using the blue cards as the canonical; this makes copies in different colors
##

for i in Blue*.svg
do
    shape=`echo "$i" | sed -e 's/Blue//'`
    echo $shape
    cat $i | sed 's/0900ee/138400/' >  "Green$shape"
    cat $i | sed 's/0900ee/d31b00/' >  "Red$shape"
    cat $i | sed 's/0900ee/fff704/' >  "Yellow$shape"

done
