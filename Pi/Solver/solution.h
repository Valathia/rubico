#ifndef SOLUTION_H
#define SOLUTION_H

#include "config.h"

// ------------------------ SOLUTION FUNCTIONS ------------------------

static inline void push_move(Solution* restrict s, const Move m) {
    if (s->length < MAX_SOLUTION) {
        s->moves[s->length] = m;
        s->length = s->length+1;
    }
}

Solution* optimize_sol(Solution* restrict s);
void print_solution(const Solution s);

#endif /* SOLUTION_H */

