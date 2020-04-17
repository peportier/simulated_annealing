# Simulated annealing applied to N-Puzzle

Example : `./np <input`

Generates a plot (`plot.ps`) of the cost against the temperature.

Input must have the following format:

```
T0 = XXX
ALPHA = XXX
BETA = XXX
BETA0 = XXX
MAXTIME = XXX
Title of the first n-puzzle to solve
9 4 8 3 2 0 7 6 5 1
Title of the second n-puzzle to solve
16 7 11 8 3 14 0 6 15 1 4 13 9 5 12 2 10
END
```

* The first five lines are the values of the parameters for the simulated annealing strategy.
* Afterwards, each puzzle is described by two consecutive lines. The first one is a title.
* The second one describes the initial configuration. It is made of numbers separated by spaces.
  * The first number is the total number of tiles.
  * The following numbers are the tiles from left to right and from top to bottom. `0` represents the empty tile.
