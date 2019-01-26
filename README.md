Sudoku generator

To generate a medium-complexity 9x9 sudoku, use

~~~
make sumaker
./sumaker 9 3 18
~~~

This outputs a postscript file which you can convert to pdf
or print directly

The generator supports any size board and "chunk" size, so
for instance you can create 6x6 sudokus with 3x2 bricks with
./sumaker 6 3 1

At high board sizes (e.g. 81), even solving for 1 missing square becomes a challenge,
because once your short-term memory is blown, searching for a missing square becomes
hard.... So suggests a new game:

The missing number.
Use

~~~
make missingnum
./missingnum 10
~~~

This again generate a postscript page which you can print.
The game is simply to find one missing number. A simple strategy might be to linearly
scan the board for each number, starting at 1 (hint: better one exist).
