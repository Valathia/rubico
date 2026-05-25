#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdalign.h>

//solved cube: UUUUUUUUURRRRRRRRRFFFFFFFFFDDDDDDDDDLLLLLLLLLBBBBBBBBB 

/* SOLVED CUBE init config
    U:  0x00 00 00 00 00 00 00 00   --  green   front
    F:  0x01 01 01 01 01 01 01 01   --  red     right
    R:  0x02 02 02 02 02 02 02 02   --  blue    back
    B:  0x03 03 03 03 03 03 03 03   --  orange  left
    L:  0x04 04 04 04 04 04 04 04   --  white   top
    D:  0x05 05 05 05 05 05 05 05   --  yellow  bottom
*/


// ------------------------ ENUMS ------------------------

enum faces{
    FRONT,
    RIGHT,
    BACK,
    LEFT,
    UP,
    DOWN    
};

enum colors{
    GREEN,
    RED,
    BLUE,
    ORANGE,
    WHITE,
    YELLOW
};

typedef enum {
    MOVE_F, MOVE_F2, MOVE_Fp,
    MOVE_R, MOVE_R2, MOVE_Rp,
    MOVE_B, MOVE_B2, MOVE_Bp,
    MOVE_L, MOVE_L2, MOVE_Lp,
    MOVE_U, MOVE_U2, MOVE_Up,
    MOVE_D, MOVE_D2, MOVE_Dp,
    ROT_RIGHT,ROT_BACK,ROT_LEFT,    //18-20 0-2
    ROL_RIGHT,ROL_DOWN,ROL_LEFT,    //21-23 3-5 FRONT   18+color
    ROL_UP,ROL_FRONT                //24-25 special cases where up goes to front, and front goes to up
} Move;

enum corner_ind{
    UFL,
    UFR,
    UBR,
    UBL,
    DFL,
    DFR,
    DBR,
    DBL
};

enum edge_ind {
    UB,
    UR,
    UF,
    UL,
    DF,
    DR,
    DB,
    DL,
    FR,
    FL,
    BR,
    BL
};

typedef enum {
    MOVE_RIGHT_FRONT,
    MOVE_LEFT_FRONT,
    MOVE_BACK_FRONT,
    MOVE_UP_FRONT,
    MOVE_DOWN_FRONT,

    MOVE_RIGHT_UP,
    MOVE_LEFT_UP,
    MOVE_DOWN_UP,
    MOVE_BACK_UP,
    MOVE_FRONT_UP
} MoveRotation;

// ------------------------ MACROS ------------------------

#define N_FACES 6
#define N_EDGES 12
#define N_CORNERS 8
#define N_MOVES (MOVE_Dp+1)
#define BYTE_SIZE 8
#define ROT_SIZE 4
#define MAX_SOLUTION 512
#define MAX_RECURSION 50
#define CUBE_STR_LEN 54

#ifndef CACHE_LINE_SIZE
    #define CACHE_LINE_SIZE 64 // Fallback caso não venha do Makefile
#endif

// ------------------------ DEBUG FLAGS ------------------------

#if DEBUGALL 
    #define VALID 1
    #define SOLVE 1
    #define DEBUGSOLVE1STROW 1
    #define DEBUGSOLVECORNERS1STROW 1
    #define DEBUGSOLVECROSS 1
    #define DEBUGSOLVE2NDROW 1
    #define DEBUGSOLVE3RDROW 1
    #define DEBUGFLIPEDGES 1
    #define DEBUGFLIPCORNERS 1
    #define DEBUGALIGNCORNERS 1
#elif DEBUGSOLVE1STROW
    #define DEBUGSOLVECORNERS1STROW 1
    #define DEBUGSOLVECROSS 1
#elif DEBUGSOLVE3RDROW
    #define DEBUGFLIPEDGES 1
    #define DEBUGFLIPCORNERS 1
    #define DEBUGALIGNCORNERS 1
#endif

// ------------------------ ERRORS ------------------------


#define ERR_1ST_ROW             10
#define ERR_2ND_ROW             11 
#define ERR_3RD_ROW             12
#define ERR_INVALID_CUBE        13
#define ERR_MAX_SOLVE_CALL      14
#define ERR_MAX_REC_EXCEEDED    15

// ------------------------ MASKS ------------------------


#define ALL         0xFFFFFFFFFFFFFFFF

#define POS0        0xFF00000000000000
#define POS1        0x00FF000000000000
#define POS2        0x0000FF0000000000
#define POS3        0x000000FF00000000
#define POS4        0x00000000FF000000
#define POS5        0x0000000000FF0000
#define POS6        0x000000000000FF00
#define POS7        0x00000000000000FF

#define CORNERS     0xFF00FF00FF00FF00          //verifica os cantos de uma face            - usado com UP & DOWN face
#define EDGES       0x00FF00FF00FF00FF          //verifica as edges de uma face             - usado com UP & DOWN face
#define COLOREDGES  0x00FF000000000000          //verificar edge na top row                 - verificação nas faces laterais

//solver
#define COLCORNERS  0xFF00FF0000000000          //verificar cantos na top row               - verificação nas faces laterais
#define COLCORDOWN  0x00000000FF00FF00          //verificar cantos na bottom row            - verificação nas faces laterais

#define ROW1DONE    0x00000000FFFFFF00          //same as checking ROW3 (countint top-down) - verificação nas faces laterais
#define ROW2DONE    0x000000FFFFFFFFFF          //checking rows 1&2                         - verificação nas faces laterais
#define ROW3DONE    0xFFFFFF0000000000          //verifica só a row 3                       - verificação nas faces laterais

#define ROW2DONE_R  0xFFFFFFFF000000FF          //verifica 3&2 row

#define F       "F "  
#define F_P     "F' " 
#define F2      "F2 "
#define U       "U "  
#define U_P     "U' " 
#define U2      "U2 "
#define R       "R "  
#define R_P     "R' " 
#define R2      "R2 "
#define L       "L "  
#define L_P     "L' " 
#define L2      "L2 "
#define B       "B "  
#define B_P     "B' " 
#define B2      "B2 "
#define D       "D "  
#define D_P     "D' " 
#define D2      "D2 "
#define ROT_Y_R "a "    // right to front 
#define ROT_Y_L "b "    // left to front
#define ROT_Y_B "c "    // back to front
#define ROT_X_B "d "    // back to up   -- special case  -> up to front
#define ROT_X_F "e "    // front to up  -- special case -> down to up
#define ROT_Z_R "f "    // right to up
#define ROT_Z_L "g "    // left to up
#define ROT_Z_D "h "    // down to up

// ------------------------ STRUCTS ------------------------
typedef uint64_t face_t;

typedef struct __attribute__ ((aligned(CACHE_LINE_SIZE))) cube {
    face_t f[N_FACES];
}Cube;

typedef struct edge {
    uint8_t f[2];
    uint8_t i[2];
} Edge;

typedef struct corner {
    uint8_t f[3]; // faces
    uint8_t i[3]; // indices
} Corner;

typedef struct search_res {
    uint8_t ind;
    uint8_t col[3];
}search_res;

typedef struct solution {
    Move moves[MAX_SOLUTION];
    uint16_t length;
} Solution;

// ------------------------ CONSTANTS ------------------------

extern const Edge edges[N_EDGES];
extern const Corner corners[N_CORNERS]; //so usado em prints de debug fora das helper..

static const face_t solved_cube[N_FACES] = {0x0000000000000000,     //FRONT
                                            0x0101010101010101,     //RIGHT
                                            0x0202020202020202,     //BACK
                                            0x0303030303030303,     //LEFT
                                            0x0404040404040404,     //UP
                                            0x0505050505050505};    //DOWN

// ------------------------ POS ARRAYS ------------------------

extern const uint8_t face_order[N_FACES];


// ------------------------ PRINT FRIENDLY ------------------------

extern const char *const colors[N_FACES];
extern const char *const sides[N_FACES];
extern const char *const corner_names[N_CORNERS];
extern const char *const edges_names[N_EDGES];

// ------------------------ UTIL MACRO FUNCTIONS ------------------------
#define BYTE(x,i)   (((x) >> (8*(7-i))) & 0xFFULL)
#define SETB(x,i,v) ((x) =(((x) & ~(0xFFULL << (8*(7-i)))) | (uint64_t)(v)))

#define check(face,color,mask) (((face & mask) ^( solved_cube[color] & mask)) == 0)

#define corner_has_color(a,b,c,color)   (((a)==(color)) | ((b)==(color)) | ((c)==(color)))
#define edge_has_color(a,b,color)       (((a)==(color)) | ((b)==(color)) )

#endif /* CONFIG_H */