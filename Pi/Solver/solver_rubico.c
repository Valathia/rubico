#define RUBICO 1
#include "solver_rubico.h"
#include "cube.h"
#include "algorithms.h"
#include "solution.h"


// ------------------------ CONSTANTS ------------------------

// em principio é igual
static const char *const row_rotate_move[ROT_SIZE] = {"", U_P, U2, U};

static const char *const new_down_rot[] = {ROT_X_B, ROT_Z_L, ROT_X_F, ROT_Z_R, ROT_Z_D};
static const uint8_t side_rotation[ROT_SIZE] = {FRONT, RIGHT, BACK, LEFT};
// start at 0
static const uint64_t down_corners_mask[ROT_SIZE] = {POS0, POS2, POS4};

// ------------------------ STATIC INLINE -----------------

static inline uint8_t check_n(const Cube * restrict cube, const uint64_t mask, const uint8_t rot_arr[], const uint8_t size) {
    for (int i = 0; i < size; i++) {
        if (!check(cube->f[cube_orientation[rot_arr[i]]], cube_orientation[rot_arr[i]], mask))
            return 0;
    }
    return 1;
}

static inline uint8_t orientation_edge(const uint8_t edge_ind, const uint8_t color)
{
    return (!(cube_orientation[edges[edge_ind].f[0]] == color));
}

static inline uint8_t orientation_corner(const uint8_t col[], const Corner c, const uint8_t color)
{
    if (col[0] == color)
    {
        return c.f[0];
    }
    else if (col[1] == color)
    {
        return c.f[1];
    }
    else
    {
        return c.f[2];
    }
}

// get first row bit and check if they are all the same for all faces
// checking 2 faces is enough, since we know the cube isn't solved we will always check at least 2 faces
// We can just return i when we find it as the displacement of the front color
static inline uint8_t is_row_rotated(const Cube * restrict cube, const uint8_t bit, const uint64_t mask)
{

    // #pragma GCC unroll ROT_SIZE
    for (int i = 0; i < ROT_SIZE; i++)
    {
        if (!check(cube->f[cube_orientation[side_rotation[i]]], BYTE(cube->f[cube_orientation[side_rotation[i]]], bit), mask))
            return 0;
        else if (check(cube->f[cube_orientation[side_rotation[i]]], cube_orientation[FRONT], mask))
        {
            return i;
        }
    }
    return 0;
}

// em principio é igual
static inline uint8_t get_wrong_edge(const Cube * restrict cube, const EdgeSearchMode mode, search_res * restrict out)
{
    // Cache global lookups in local CPU registers
    const uint8_t up = cube_orientation[UP];
    const uint8_t down = cube_orientation[DOWN];

    for (uint8_t e = 0; e < N_EDGES; e++)
    {
        uint8_t col[2];
        get_edge(cube, e, col);

        const uint8_t c0 = col[0];
        const uint8_t c1 = col[1];

        // --- ARM OPTIMIZATION: Branchless Filter ---
        // Instead of 'if', evaluate conditions into a boolean integer.
        // The ARM processor can compute these simultaneously using quick bitwise ops.
        uint32_t match = 0;

        if (mode == EDGE_TOP)
        {
            match = (c0 == up) | (c1 == up);
        }
        else
        { // EDGE_MIDDLE
            match = (c0 != up) & (c1 != up) & (c0 != down) & (c1 != down);
        }

        // If the piece does not match the current search criteria, skip early
        if (!match)
            continue;

        const uint8_t ind1 = cube_orientation[edges[e].f[0]];
        const uint8_t ind2 = cube_orientation[edges[e].f[1]];

        if ((c0 != ind1) | (c1 != ind2))
        {
            out->col[0] = c0;
            out->col[1] = c1;
            out->ind = e;
            return 1;
        }
    }
    return 0;
}

static inline uint8_t get_wrong_corner(const Cube * restrict cube, uint8_t color, search_res * restrict out)
{
    // #pragma GCC unroll N_CORNERS
    for (uint8_t i = 0; i < N_CORNERS; i++)
    {
        uint8_t col[3];
        get_corner(cube, i, col);

        const uint8_t c0 = col[0];
        const uint8_t c1 = col[1];
        const uint8_t c2 = col[2];
        uint8_t has_color = corner_has_color(c0, c1, c2, color);

        if (!has_color)
            continue;

        const uint8_t target0 = cube_orientation[corners[i].f[0]];
        const uint8_t target1 = cube_orientation[corners[i].f[1]];
        const uint8_t target2 = cube_orientation[corners[i].f[2]];

        if ((c0 != target0) | (c1 != target1) | (c2 != target2))
        {
            out->col[0] = c0;
            out->col[1] = c1;
            out->col[2] = c2;
            out->ind = i;
            return 1;
        }
    }

    return 0;
}

uint8_t check_corner_alignment(const Cube * restrict cube, const uint8_t start, const uint8_t end)
{

    // 0-3 UP | 4-7 DOWN
    for (uint8_t i = start; i < end; i++)
    {
        uint8_t col[3]; // color
        get_corner(cube, i, col);
        if (!condition_corner_align(col, i))
        {
            return 0;
        }
    }

    return 1;
}
// ------------------------ SOLVER AUX ------------------------

uint8_t how_many(const Cube * restrict cube, const uint64_t mask)
{ // just send the cube not the array
    uint8_t count = 0;

    // #pragma GCC unroll ROT_SIZE
    for (uint8_t i = 0; i < ROT_SIZE; i++)
    {
        const uint8_t index = side_rotation[i];
        count += check(cube->f[cube_orientation[index]], cube_orientation[index], mask);
    }
    return count;
}

// solver align_edges
void align_edges_rotate_cube(Cube *restrict cube_arr, Solution *sol)
{
    // who's aligned ?
    uint8_t whoaligned[4];
    uint8_t count = 0;

    // #pragma GCC unroll ROT_SIZE
    for (int i = 0; i < ROT_SIZE; i++)
    {
        int index = side_rotation[i];
        whoaligned[i] = check(cube_arr[0].f[cube_orientation[index]], cube_orientation[index], POS5);
        count += whoaligned[i];
    }
    // I can only get 2 done or none
    // special case: if adjacent I need to orient the cube
    if (count == 2) {
        if (whoaligned[BACK] & whoaligned[RIGHT]) {
            apply_alg(cube_arr, sol, ROT_Y_L); // Right to Front
            return;
        }
        else if (whoaligned[FRONT] & whoaligned[LEFT]) {
            apply_alg(cube_arr, sol, ROT_Y_R); // Back to Front
            return;
        }
        else if (whoaligned[FRONT] & whoaligned[RIGHT])
        {
            apply_alg(cube_arr, sol, ROT_Y_B); // Left to Front
            return;
        }
        else
        {
            return; // nothing to align
        }
    }
    return; // nothing to align
}

// solver align_edges
uint8_t align_edges_max_edges(const Cube *restrict cube_arr)
{
    Cube work[2];
    uint8_t cur_max = 0;
    uint8_t aux;
    uint8_t index = 0;
    copy_cube(work, cube_arr, 2);

    // #pragma GCC unroll ROT_SIZE
    for (int i = 0; i < ROT_SIZE; i++)
    {
        aux = how_many(&work[0], POS5);
        if ((aux % 2 == 0) & (aux > cur_max)) {
            if (aux == 4)
                return i;

            cur_max = aux;
            index = i;
        }

        Dm(work);
    }

    return index;
}


uint8_t count_corners(Cube *restrict cube_arr, uint8_t (*cond)(const uint8_t *, uint8_t))
{

    uint8_t corner_search[] = {DFR, DBR, DBL, DFL};

    uint8_t count = 0;
    for (uint8_t i = 0; i < 4; i++) {
        uint8_t col[3]; // color
        get_corner(&cube_arr[0], corner_search[i], col);
        if (cond(col, corner_search[i])) {
            count++;
        }
    }
    return count;

}
// solver flip & align corners

void corners_FLU_corner(Cube* restrict cube_arr,Solution* sol, uint8_t (*cond)(const uint8_t*, uint8_t)) {
   uint8_t ind=0;
   for(uint8_t i=0;i<4;i++){
      uint8_t col[3]; //color
      get_corner(&cube_arr[0], i, col);
      if(cond(col,i)) {
         ind = i;
         break;
      }
   }

   switch (ind)
   {
   case 1:
      apply_alg(cube_arr,sol,ROT_Y_R);  // right to front
      break;
   case 2:
      apply_alg(cube_arr,sol,ROT_Y_B); // back to front
      break;
   case 3:
      apply_alg(cube_arr,sol,ROT_Y_L); // left to front
      break;
   default:    // if 0 it's either correctly placed or there are no correct corners. do nothing
      break;
   }
}

void corners_DFR_corner(Cube *restrict cube_arr, Solution *sol, uint8_t (*cond)(const uint8_t *, uint8_t))
{

    uint8_t corner_search[] = {DFR, DBR, DBL, DFL};

    uint8_t ind = 0;
    for (uint8_t i = 0; i < 4; i++)
    {
        uint8_t col[3]; // color
        get_corner(&cube_arr[0], corner_search[i], col);
        if (cond(col, corner_search[i]))
        {
            ind = i;
            break;
        }
    }

    switch (ind)
    {
    case 1:
        apply_alg(cube_arr, sol, ROT_Y_R); // right to front
        break;
    case 2:
        apply_alg(cube_arr, sol, ROT_Y_B); // back to front
        break;
    case 3:
        apply_alg(cube_arr, sol, ROT_Y_L); // left to front
        break;
    default: // if 0 it's either correctly placed or there are no correct corners. do nothing
        break;
    }
}

uint8_t go_to_next_corner(Cube *restrict cube_arr, Solution *sol) {

    const char *cmove[3] = {U_P, U2, U};

    for (uint8_t i = 0; i < 3; i++) {
        if (!check(cube_arr[0].f[UP], cube_orientation[UP], down_corners_mask[i])) {
            apply_alg(cube_arr, sol, cmove[i]);
            return 1;
        }
    }

    return 0;
}

// ------------------------ 3RD ROW STAGES ------------------------

// limited by max recursion
uint8_t flip_edges(Cube *restrict cube_arr, Solution *sol)
{
    const uint8_t color = cube_orientation[DOWN];

    uint8_t i = 0;
    while ((i < MAX_RECURSION) & (!is3rdRowEdgeFlipped(cube_arr[0]))) {
        if (check(cube_arr[0].f[color], color, POS1) & check(cube_arr[0].f[color], color, POS5)) {
            apply_alg(cube_arr, sol, D);
        } // vertical line -> change to horizontal line

        if (check(cube_arr[0].f[color], color, POS7) & check(cube_arr[0].f[color], color, POS3)) {
            apply_alg(cube_arr, sol, ROT_Y_R L ROT_Y_L L_ALG_P ROT_Y_R L_P ROT_Y_L);  //  F R_ALG F_P -> ROT_Y_R L ROT_Y_L L_ALG_P ROT_Y_R L_P ROT_Y_L
            return is3rdRowEdgeFlipped(cube_arr[0]);
        } // horizontal line -> flip edges

        // HOOK Case -> quero no 3 e 5
        if ((check(cube_arr[0].f[color], color, POS7) & check(cube_arr[0].f[color], color, POS5)) |
            (check(cube_arr[0].f[color], color, POS7) & check(cube_arr[0].f[color], color, POS1)) |
            (check(cube_arr[0].f[color], color, POS3) & check(cube_arr[0].f[color], color, POS1)) ) {
            if (check(cube_arr[0].f[color], color, POS7) & check(cube_arr[0].f[color], color, POS5)) {
                apply_alg(cube_arr, sol, D_P);
            }
            else if (check(cube_arr[0].f[color], color, POS3) & check(cube_arr[0].f[color], color, POS1)) {
                apply_alg(cube_arr, sol, D);
            }
            else if (check(cube_arr[0].f[color], color, POS7) & check(cube_arr[0].f[color], color, POS1)) {
                apply_alg(cube_arr, sol, D2);
            }
        }
        // oriented hook or dot case
        apply_alg(cube_arr, sol, HOOK_RUBICO);
        i++;
    }

    return is3rdRowEdgeFlipped(cube_arr[0]);
}

// limit set by max recursion
uint8_t align_edges(Cube *restrict cube_arr, Solution *sol)
{

    uint8_t i = 0;

    while (i < MAX_RECURSION) {
        uint8_t move = align_edges_max_edges(cube_arr); // check how many edges can be aligned by moving the Up layer
        switch (move)
        {
        case 1:
            apply_alg(cube_arr, sol, D);
            break;
        case 2:
            apply_alg(cube_arr, sol, D2);
            break;
        case 3:
            apply_alg(cube_arr, sol, D_P);
            break;
        default:
            break;
        }

        align_edges_rotate_cube(cube_arr, sol); // if there are two adjacent edges, rotate the cube to solve them.

        if (is3rdRowEdgeAligned(cube_arr[0]))
        { // moving the up layer may solve the edges
            return 1;
        }

        apply_alg(cube_arr, sol, SONE_RUBICO); // apply sone algorithm to swap adjacent edges on the front and rigt side
                                        // The sone algorithm usually needs some kind of Um after to align everything
        i++;
    }

    if ((i == MAX_RECURSION) & (!is3rdRowEdgeAligned(cube_arr[0])))
    {
        print_solution(*sol);
        fprintf(stderr, "\nERROR %d: 3rd Layer Alligning Edges Solving Attempts exceeded, something went wrong \n ", ERR_MAX_REC_EXCEEDED);
        exit(ERR_MAX_REC_EXCEEDED);
    }

    return is3rdRowEdgeAligned(cube_arr[0]);
}

// limit set by max recursion
uint8_t align_corners(Cube *restrict cube_arr, Solution *sol)
{
    uint8_t i = 0;
    while (!check_corner_alignment(&cube_arr[0], 4, 8) & (i < MAX_RECURSION)) {
        corners_DFR_corner(cube_arr, sol, condition_corner_align); // rotate cube so that FLU corner is a correctly placed corner
        apply_alg(cube_arr, sol, NIKLAS_RUBICO); // apply niklas algorithm to swap corners
        i++;
    }

    if ((i == MAX_RECURSION) & (!check_corner_alignment(&cube_arr[0], 4, 8)))
    {
        print_solution(*sol);
        fprintf(stderr, "\nERROR %d: 3rd Layer Alligning Corners Solving Attempts exceeded, something went wrong \n ", ERR_MAX_REC_EXCEEDED);
        exit(ERR_MAX_REC_EXCEEDED);
    }

    return check_corner_alignment(&cube_arr[0], 4, 8);
}


// limit set by max recursion
uint8_t flip_corners(Cube *restrict cube_arr, Solution *sol)
{
    apply_alg(cube_arr,sol,ROT_Z_D);    //flip cube
    corners_FLU_corner(cube_arr, sol, condition_corner_flip); // move an unsolved corner to the FLU corner
    //apply_alg(cube_arr, sol, ROT_Z_D);                       
    uint8_t j = 0;

    while (j < MAX_RECURSION)
    {
        // is UP face solved?
        if (check(cube_arr[0].f[cube_orientation[UP]], cube_orientation[UP], ALL))
        {
            if (isSolved(cube_arr[0]))
            { // is the cube solved?
                return 1;
            }
            else
            {
                uint8_t rot = is_row_rotated(&cube_arr[0], 4, ROW3DONE);

                if (rot) {
                    apply_alg(cube_arr, sol, row_rotate_move[rot]);
                    return isSolved(cube_arr[0]);
                }
            }
        }
        // down face is not solved, cycle through corners
        uint8_t out[3];
        // worst case there's 4 corners to flip
        // #pragma GCC unroll ROT_SIZE
        for (int i = 0; i < ROT_SIZE; i++) {
            get_corner(&cube_arr[0], UFL, out);
            // minimum is 2 times
            while (condition_corner_flip(out, UFL)&&(j<MAX_RECURSION)) { // if flipped
                apply_alg(cube_arr, sol, L_ALG_P L_ALG_P);
                // apply_alg(cube_arr,sol,r_alg);
                get_corner(&cube_arr[0], UFL, out);
                j++;

            }

            if (!go_to_next_corner(cube_arr, sol)) { // no more flipped corners
                break; // break is to check the isSolved condition at the top, usually the cube needs to be reoriented.
            }
        }

        j++;
    }

    if ((j == MAX_RECURSION) & (!isSolved(cube_arr[0]))) {
        print_cube(&cube_arr[0]);
        print_solution(*sol);
        fprintf(stderr, "\nERROR %d: 3rd Row Corner Flipping Solving Attempts exceeded, last step failled, something went wrong \n ", ERR_MAX_REC_EXCEEDED);
        exit(ERR_MAX_REC_EXCEEDED);
    }

    return isSolved(cube_arr[0]);
}

// Checks edges while there are wrongly placed top edges
// or until it reaches a maximum recursion threshold
uint8_t solve_cross(Cube *restrict cube_arr, Solution *sol) {

    search_res out;
    uint8_t n = get_wrong_edge(&cube_arr[0], EDGE_TOP, &out);
    uint8_t i = 0;
    while (n & (i < MAX_RECURSION)) {
        const uint8_t dest = piece_dest(out.col, N_EDGES, condition_edge_align);
        const uint8_t ori = orientation_edge(dest, out.col[0]);
        const char *c = edge_move_table[out.ind][dest][ori];

        apply_alg(cube_arr, sol, c);

        n = get_wrong_edge(&cube_arr[0], EDGE_TOP, &out);
        i++;
    }

    if ((i == MAX_RECURSION) & (!isUpCrossSolved(cube_arr[0]))) {
        print_solution(*sol);
        fprintf(stderr, "\nERROR %d: Up Layer Cross Solving Attempts exceeded, something went wrong \n ", ERR_MAX_REC_EXCEEDED);
        exit(ERR_MAX_REC_EXCEEDED);
    }
    return isUpCrossSolved(cube_arr[0]);
}

// Checks corners while there are wrongly placed bottom corners
// or until it reaches a maximum recursion threshold
uint8_t solve_corners_1strow(Cube *restrict cube_arr, Solution *sol) {
    search_res out;
    uint8_t color = cube_orientation[UP];
    uint8_t n = get_wrong_corner(&cube_arr[0], color, &out);
    uint8_t i = 0;
    while (n && i < MAX_RECURSION) {
        // check for corners on 2 front positions first -->
        /*
            - orient corner to correct face
            - rotate
            - check ori
            - pick lefty or right alg
        */
        const uint8_t dest = piece_dest(out.col, N_CORNERS, condition_corner_align);
        const uint8_t ori = orientation_corner(out.col, corners[out.ind], color);

        if (dest >= N_CORNERS) {
            print_cube(&cube_arr[0]);
            fprintf(stderr, "ERROR %d: Couldn't Find the Destination Corner for a wrongly placed Corner\n", ERR_1ST_ROW);
            printf("    \t\ttrying to move %s to Down while solving %s \n", corner_names[out.ind], colors[cube_orientation[DOWN]]);
            exit(ERR_1ST_ROW);
        }

        const char *cm = corner_move_table[out.ind][dest][cmt_side_i(ori)];

        apply_alg(cube_arr, sol, cm);

        n = get_wrong_corner(&cube_arr[0], color, &out);
        i++;
    }

    if (i == MAX_RECURSION && n)
    {
        print_solution(*sol);
        fprintf(stderr, "\nERROR %d: 1st Row Corners Solving Attempts exceeded, something went wrong \n ", ERR_MAX_REC_EXCEEDED);
        exit(ERR_MAX_REC_EXCEEDED);
    }

    return is1stRowSolved(cube_arr[0]);
}

// ------------------------ ROW SOLVERS ------------------------

// Each row solver should only run once.

uint8_t solve_1st_row(Cube *restrict cube_arr, Solution *sol) {
    // check if top cross is solved  if(!up_cross_solved(cube_arr))
    if (!isUpCrossSolved(cube_arr[0])) {

        if (!solve_cross(cube_arr, sol)) {
            fprintf(stderr, "ERROR %d: 1st Cross Solve FAILED - Recheck Algorithm\n", ERR_1ST_ROW);
            exit(ERR_1ST_ROW);
        }
        #if TEST
                valid_cube_config(&cube_arr[0]);
        #endif
    }

    // if corners not solved - same cost as checking the top row solve
    if (!is1stRowSolved(cube_arr[0])) {

        if (!solve_corners_1strow(cube_arr, sol))
        {
            fprintf(stderr, "ERROR %d: 1st Row - Corner Solve FAILED - Recheck Algorithm\n", ERR_1ST_ROW);
            exit(ERR_1ST_ROW);
        }

        #if TEST
                valid_cube_config(&cube_arr[0]);
        #endif
    }

    return is1stRowSolved(cube_arr[0]);
}

// limited by max recursion
uint8_t solve_2nd_row(Cube *restrict cube_arr, Solution *sol) {

    search_res out;
    uint8_t n = get_wrong_edge(&cube_arr[0], EDGE_MIDDLE, &out);
    uint16_t i = 0;
    while (n && i < MAX_RECURSION) {

        uint8_t dest = piece_dest(out.col, N_EDGES, condition_edge_align);

        if (dest == UINT8_MAX || dest < FR) {
            break;
        }

        uint8_t ori = orientation_edge(dest, out.col[0]);

        dest = dest - N_CORNERS;
        out.ind-=4;
        const char *c = midle_edge_move_table[out.ind][dest][ori];

        apply_alg(cube_arr, sol, c);

        n = get_wrong_edge(&cube_arr[0], EDGE_MIDDLE, &out);
        i++;
    }

    if (i == MAX_RECURSION && n) {
        print_solution(*sol);
        fprintf(stderr, "\nERROR %d: 2nd Row Solving Attempts exceeded, something went wrong \n ", ERR_MAX_REC_EXCEEDED);
        exit(ERR_MAX_REC_EXCEEDED);
    }

    #if TEST
        valid_cube_config(&cube_arr[0]);
    #endif
    return is2ndRowSolved(cube_arr[0]);
}

uint8_t solve_3rd_row(Cube *restrict cube_arr, Solution *sol) {

    /*
    if edges are NOT flipped : flip_edges

    if edges are NOT alligned: align_edges

    if corners are NOT alligned: align_corners

    if cube NOT solved: flip cube vertically, flip corners
    */

    // check if edges of top face are flipped

    if (!is3rdRowEdgeFlipped(cube_arr[0])) {
        if (!flip_edges(cube_arr, sol)) {
            return 0;
        }

        #if TEST
            valid_cube_config(&cube_arr[0]);
        #endif
    }

    if (!is3rdRowEdgeAligned(cube_arr[0])) {
        if (!align_edges(cube_arr, sol)) {
            return 0;
        }
        #if TEST
            valid_cube_config(&cube_arr[0]);
        #endif
    }

    // verifying that the corners aren't solved is equivalent to verifying the whole cube
    if (!isSolved(cube_arr[0])) {

        // check top row corner alignment
        if (!check_corner_alignment(&cube_arr[0], 4, 8)) {
            if (!align_corners(cube_arr, sol)) {
                return 0;
            }
            #if TEST
                valid_cube_config(&cube_arr[0]);
            #endif
        }

        // recheck condition in case cube was solved in previous stage
        if (!isSolved(cube_arr[0])) {
            flip_corners(cube_arr, sol);
            #if TEST
                valid_cube_config(&cube_arr[0]);
            #endif
        }
    }

    return isSolved(cube_arr[0]);
}
// ------------------------ SOLVER ------------------------

uint8_t solve_(Cube *restrict cube_arr, Solution *sol) {
    // cube solve is already being tested at loop entrance

    if (!is1stRowSolved(cube_arr[0])) {
        if (!solve_1st_row(cube_arr, sol)) {
            fprintf(stderr, "ERROR %d: 1st Row Call FAILED - Recheck Algorithm\n", ERR_1ST_ROW);
            exit(ERR_1ST_ROW);
        }
    }

    if (!is2ndRowSolved(cube_arr[0])) {
        if (!solve_2nd_row(cube_arr, sol)) {
            fprintf(stderr, "ERROR %d: 2nd Row Call FAILED - Recheck Algorithm\n", ERR_2ND_ROW);
            exit(ERR_2ND_ROW);
        }
    }

    if (!isSolved(cube_arr[0])) {
        if (!solve_3rd_row(cube_arr, sol)) {
            fprintf(stderr, "ERROR %d: 3rd Row Call FAILED - Recheck Algorithm\n", ERR_3RD_ROW);
            exit(ERR_3RD_ROW);
        }
    }

    return isSolved(cube_arr[0]);
}

void solve(Cube *restrict cube_arr, Solution *sol) {
    if (isSolved(cube_arr[0])){
        print_solution(*sol);
        return;
    }

    // check if there is already a solved face we can use
    uint8_t new_down = DOWN; // default is the already down face
    for (int i = 0; i < N_FACES; i++) {
        if (check(cube_arr[0].f[cube_orientation[i]], cube_orientation[i], ALL)) {
            new_down = i; // before it was cube_orientation[i]
        }
    }

    if (new_down < 5) {
        apply_alg(cube_arr, sol, new_down_rot[new_down]);
    }

    if (!valid_cube_config(&cube_arr[0])) {
        print_solution(*sol);
        fprintf(stderr, "ERROR %d: Invalid Cube Configuration after Rotation Move %s\n", ERR_INVALID_CUBE, new_down_rot[new_down]);
        exit(ERR_INVALID_CUBE);
    }

    const uint8_t solved = solve_(cube_arr, sol);

    Solution *optimized = optimize_sol(sol);
    if (optimized == NULL) {
        return;
    }

    sol = optimized;
    print_solution(*sol);

    if (isSolved(cube_arr[0])) {
        return;
    }
    else if (!solved) {
        fprintf(stderr, "\nERROR %d: Something went wrong, only partial solution after calling solver \n", ERR_MAX_SOLVE_CALL);
        exit(ERR_MAX_SOLVE_CALL);
    }
}
