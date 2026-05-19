#ifndef SOLUTION_H
#define SOLUTION_H

#include "config.h"

// ------------------------ SOLUTION FUNCTIONS ------------------------

void push_move(Solution* restrict s, Move m);
Solution* optimize_sol(Solution* restrict s);
void print_solution(Solution s);

#endif /* SOLUTION_H */

