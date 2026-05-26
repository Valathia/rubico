#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "config.h"
               //F R U R' U' F'
               //F D L D_P L_P F'.    u'
#define L_ALG_P         L D L_P D_P 
#define L_ALG_P_I       D L D_P L_P
#define FAKE_R_ALG      L_P D_P L D 

#define HOOK            "F U R U' R' F' "
#define HOOK_RUBICO     F L_ALG_P_I F_P
#define R_ALG           "R U R' U' "
#define L_ALG           "L' U' L U "
#define NIKLAS          "R U' L' U R' U' L U "
#define SONE            "R U R' U R U2 R' "
#define SONE_RUBICO      L D L_P D L D2 L_P
#define NIKLAS_RUBICO    L D_P R_P D L_P D_P R D

//L D' R' D L' D' R D

// ------------------------ MOVE TABLES ------------------------

#ifdef RUBICO
    extern const char * const rubico_corners[N_CORNERS][4][3];
    extern const char * const rubico_middle_edges[8][4][2];

    #define corner_move_table rubico_corners
    #define midle_edge_move_table rubico_middle_edges
#else 
    extern const char* const midle_edge_move_table[8][4][2];
    extern const char* const corner_move_table[N_CORNERS][4][3];
#endif

extern const char* const edge_move_table[N_EDGES][4][2];


#endif /* ALGORITHMS_H */

