Nineteen game was an assignment for artificial intelligence involving a combination of breadth first search and A* search to find the minimum set of moves to solve a puzzle.

The puzzle involves a cross shaped board with the numbers 0 through 19.
An example puzzle might look like this,

           2  1
           6  3
     4  5 12  7  8  9
    10 11 13 17 14 15
           0 16
          18 19
     

The tile with '0' is the "blank" and this tile can be moved up down left or right as long as it remains on the cross.
When the blank is moved, it swaps positions with the number in the position that it moves to.
The goal is to arrange the board in the following configuration in as few moves as possible.


           0  1
           2  3
     4  5  6  7  8  9
    10 11 12 13 14 15
          16 17
          18 19
      
The example puzzle given takes a minimum of 6 moves to solve and the sequence of moves looks like this:

           2  1
           6  3
     4  5 12  7  8  9
    10 11 13 17 14 15
           0 16
          18 19


           2  1
           6  3
     4  5 12  7  8  9
    10 11 13 17 14 15
          16  0
          18 19


           2  1
           6  3
     4  5 12  7  8  9
    10 11 13  0 14 15
          16 17
          18 19


           2  1
           6  3
     4  5 12  7  8  9
    10 11  0 13 14 15
          16 17
          18 19


           2  1
           6  3
     4  5  0  7  8  9
    10 11 12 13 14 15
          16 17
          18 19


           2  1
           0  3
     4  5  6  7  8  9
    10 11 12 13 14 15
          16 17
          18 19


           0  1
           2  3
     4  5  6  7  8  9
    10 11 12 13 14 15
          16 17
          18 19
      
      

To try your own puzzles, download the exe and enter the numbers in order left to right and top to bottom and press enter and the program will attempt to find the optimal
sequence of moves. Keep in mind that nearly half of all configurations have no solution, and some will take lots of time/memory.
