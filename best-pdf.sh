#!/usr/local/bin/bash
# only bash has $RANDOM!
rm -r temp 2>/dev/null
mkdir -p temp
FNAME=""
Y=1
cat <<EOF >temp/input
4 2 4
4 2 8
6 3 12
6 3 18
6 3 35
6 3 36
8 4 8
8 4 16
8 4 24
9 3 18
9 3 27
9 3 36
36 6 6
81 9 3
100 1 1
EOF
 
while read X; do
    ./sumaker $X $RANDOM > temp/$Y.ps
    FNAME="$FNAME temp/$Y.ps"
    Y=$((Y+1))
done < temp/input
echo gs -sDEVICE=pdfwrite -dNOPAUSE -dBATCH -dSAFER -sOutputFile=best.pdf $FNAME
gs -sDEVICE=pdfwrite -dNOPAUSE -dBATCH -dSAFER -sOutputFile=best.pdf $FNAME
rm -r temp
