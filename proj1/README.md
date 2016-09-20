#Programming Assignment 1

Programming Assignment 1 consists of three parts. As of this README's last modified date, only the first part is completed.
These assignments are to practise the behaviours of programming with pthreads in C.

##Sudoku Validator
Runs 3 threads (as opposed to the suggested 11), one for rows, one for columns, and one for a 3x3 grid. Accepts a ".su" file, which contains width, height, and then sudoku puzzle data separated out with spaces or new lines. The algorithm uses the first 9 bits of shorts (16-bit integers) and sets bits to "1" for each number that appears in each row, column, or grid respectively. If each function returns 0x1FF (511) every time, the sudoku puzzle is guaranteed to be correct.

This library utilises the CN_Grids Data Structure Library (written by Clara) to aid in storing information about the sudoku puzzle.
