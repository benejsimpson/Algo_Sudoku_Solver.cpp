Algorithmic Sudoku Solver by Benjamin Simpson

This is my first ever C++ program.
I was inspired by a yt video making a chess engine using bitboards and thought it would be fun to try it with Sudoku, modelling each cells candidate digits as a 9-bit integer.
I have heard of recursive backtracking processes being used to solve sudokus but this is purely logic based, mimicing the method I use when solving sudokus myself.

By implementing logic to:
- Fill 'naked singles' - fill cells that have only one candidate digit.
- Fill 'hidden singles' - fill a cell with a candidate digit that appears only once in a constraint.
- Find 'naked pairs' - if a pair of cells in the same constraint both contain the same set of 2 digits, eliminate those candidates from other cells in constraint.

In its current state (20/04/26), the algorithm can solve most (if not all) sudoku.com puzzles with difficulty rating from 'easy' to 'expert' - occasionally the 'master' difficulty too!

Next steps:
- Extend 'naked pairs' to naked triples - quadruples are rarely used when solving sudokus but may end up using this if necessary
- Add logic for X-wings - specific digit appears only twice in two different rows (or columns) and these pairs align in the same two columns (or rows)
- Extent X-wing to Y-wing - using 3 corners instead of 4

I got tired of typing out so many zeros when entering the puzzles to test so I created a notation system following this method:
- Read from left to right - top to bottom.
- We consider each row within a box as a 'subrow' i.e. the first bit of the notation we need to consider is the first 3 cells in the first row (R1C123), then the second three cells in the first row (R1C456), then the last 3 cells in the first row (R1C789), before moving to the first 3 cells of the second row (R2C123).
- If a 'subrow' is completely empty - a row within a box containing no digits - we use a '.'
- If a 'subrow' contains a digit X followed by 2 empty cells then we would use 'X+'
- If a 'subrow' contains 2 empty cells followed by a digit Y then we would use '+Y'

ALL OF THIS WAS WRITTEN AND DEBUGGED BY ME WITHOUT THE USE OF AI!
