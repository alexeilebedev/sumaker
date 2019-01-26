#!/usr/local/bin/bash
make sumaker
./sumaker 4 2 4  $RANDOM | lp
./sumaker 4 2 8  $RANDOM | lp
./sumaker 6 3 12  $RANDOM | lp
./sumaker 6 3 18  $RANDOM | lp
./sumaker 6 3 35  $RANDOM | lp
./sumaker 6 3 36  $RANDOM | lp
./sumaker 8 4 8  $RANDOM | lp
./sumaker 8 4 16  $RANDOM | lp
./sumaker 8 4 24  $RANDOM | lp
./sumaker 9 3 18 $RANDOM | lp
./sumaker 9 3 27 $RANDOM | lp
./sumaker 9 3 36 $RANDOM | lp
./sumaker 36 6 6  $RANDOM | lp
./sumaker 81 9 3  $RANDOM | lp
./sumaker 100 1 1  $RANDOM | lp
