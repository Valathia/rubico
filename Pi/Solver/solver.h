#ifndef SOLVER_H
#define SOLVER_H

#include "config.h"

// ------------------------ MACRO FUNCTIONS ------------------------

typedef enum {
    EDGE_TOP,
    EDGE_MIDDLE
}EdgeSearchMode;

#define cmt_side_i(i_s) (((i_s)==FRONT | (i_s)==BACK) ? 0 : (((i_s)==UP | (i_s)==DOWN) ? 1 : 2))
#define cmt_down_i(i_c) (i_c-4)

//checking the down/up face and checking the rows ensures that the we didn't solve the up pieces in the down face and vice-versa
#define isSolved(cube)          (check_n(&cube,ALL,face_order,6))
#define is1stRowSolved(cube)    ((check(cube.f[cube_orientation[DOWN]],cube_orientation[DOWN],ALL)) & check_n(&cube,ROW1DONE,side_rotation,ROT_SIZE))
#define isUpCrossSolved(cube)   ((check(cube.f[cube_orientation[UP]],cube_orientation[UP],EDGES)) & check_n(&cube,COLOREDGES,side_rotation,ROT_SIZE))
#define is2ndRowSolved(cube)    ((check(cube.f[cube_orientation[DOWN]],cube_orientation[DOWN],ALL)) & check_n(&cube,ROW2DONE,side_rotation,ROT_SIZE))

#define is3rdRowEdgeFlipped(cube)((check(cube.f[cube_orientation[UP]],cube_orientation[UP],EDGES)))
#define is3rdRowEdgeAligned(cube)(check_n(&cube,COLOREDGES,side_rotation,ROT_SIZE))


#define is1stRowSolved_rubico(cube)    ((check(cube.f[cube_orientation[UP]],cube_orientation[UP],ALL)) & check_n(&cube,ROW3DONE,side_rotation,ROT_SIZE))
#define is2ndRowSolved_rubico(cube)    ((check(cube.f[cube_orientation[UP]],cube_orientation[UP],ALL)) & check_n(&cube,ROW2DONE_R,side_rotation,ROT_SIZE))

#define is3rdRowEdgeFlipped_rubico(cube)((check(cube.f[cube_orientation[DOWN]],cube_orientation[UP],EDGES)))
#define is3rdRowEdgeAligned_rubico(cube)(check_n(&cube,POS5,side_rotation,ROT_SIZE))

void solve(Cube* restrict cube_arr, Solution* sol);

#endif /* SOLVER_H */

