# Solving a 15-Puzzle using IDA* Search Algorithm

The 15-Puzzle popularly consists of decomposing an image into a number of sliding tiles on a board, shuffling their positions, then sliding the tiles to recreate the image.

Each tile can be identified by a number in $[1, n^2-1]$ where n is the side length of the square board. This leaves 1 space left over, **B**, which is a blank space. Each configuration of the puzzle is called a *state*. We can represent an unsolved initial state using the diagrams below:


A puzzle solution is a sequence of moves which achieves the following state:

subject to the constraint that we can only swap positions of the blank tile with some adjacent one.


The 15-puzzle Graph $G = ⟨V, E⟩$ is implicitly
defined. The vertex set $V$ is defined as all the possible puzzle configurations (states), and the edges
$E$ connecting two vertexes are defined by the legal movements, also called actions or operators.

The IDA* algorithm utilises a depth-first search strategy and is implemented as a recursive function.


The heuristic that is used to prune the search space is the Sum of Manhattan Distances, which is defined as follows:

$\sum_{i} d(t_i,t_i^*)$


where $t_i$ is the position of the ith tile and $t_i^*$ is the position where this tile should be. Positions are given by 2–component vectors $t_i$ = (x, y). Manhattan distance is easily computed  


$d(t_i,t_i^*) = |x − x^∗| + |y − y^∗|$  


For the initial state depicted above, the heuristic computation would be, starting from the upper left
corner  

$h(s) = 1 + 1 + 2 + 2 + 4 + 1 + 1 + 4 + 1 + 1 + 1 + 4 + 1 + 1 + 2 = 27$  

note that we don’t take into account the (mis)placement of the blank space.
This heuristic is proved to be admissible, i.e. $h(s)$ is always lower or equal to the optimal distance
$h^∗(s)$ to the objective configuration from any state $s$, for the 15–puzzle, being a useful heuristic to search for optimal solutions.