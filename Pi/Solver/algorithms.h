#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "config.h"

#define HOOK    "F U R U' R' F' "
#define R_ALG   "R U R' U' "
#define L_ALG   "L' U' L U "
#define NIKLAS  "R U' L' U R' U' L U "
#define SONE    "R U R' U R U2 R' "
// ------------------------ MOVE TABLES ------------------------

extern const char* const corner_move_table[N_CORNERS][4][3];
extern const char* const edge_move_table[N_EDGES][4][2];
extern const char* const midle_edge_move_table[8][4][2];

#endif /* ALGORITHMS_H */

