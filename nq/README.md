# Simulated annealing applied to N-Queens

Example : `./nq <input`

Generates a plot (`plot.ps`) of the cost against the temperature

Input must have the following format:

```
T0 = XXX
ALPHA = XXX
BETA = XXX
BETA0 = XXX
MAXTIME = XXX
Title of the first n-queens problem to solve
10 0 1 2 3 4 5 6 7 8 9
Title of the second n-queens problem to solve
20 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19
```

* The first five lines are the values of the parameters for the 
  simulated annealing strategy.
* Afterwards, each puzzle is described by two consecutive lines. 
  The first one is a title.
* The second one describes the initial configuration. 
  It is made of numbers separated by spaces.
  * The first number is the total number of queens.
  * The following numbers are the initial rows of queens 0 to N-1.
    It works since a configuration is represented such that queen i 
    stays on column i.
  * Thus for the two examples above, the queens are positioned on 
    the principal diagonal.
    * With such an initial configuration, if a move is the swap of 
      the rows of two queens, then queens can only be attacked on 
      diagonals. This property is used in the code.
