#ifndef CUBE_H
#define CUBE_H

#include "config.h"

extern const uint8_t*  cube_orientation;

/* Index of the next element in a cyclic rotation array of length n */
#define rot_ori(face, n, iscw) ((iscw) ? (((face) + 1) & ((n) - 1)) : (((face) - 1) & ((n) - 1)))

// ------------------------ STATIC INLINES - MOVE HELPERS ------------------------

static inline face_t pack3(face_t f, uint8_t a,uint8_t b,uint8_t c){
    return BYTE(f,a) | (BYTE(f,b)<<8) | (BYTE(f,c)<<16);
}

static inline face_t pack3r(face_t f,uint8_t a,uint8_t b,uint8_t c){
    return BYTE(f,c) | (BYTE(f,b)<<8) | (BYTE(f,a)<<16);
}

static inline void unpack3(face_t * restrict f,uint8_t a,uint8_t b,uint8_t c,face_t v){
    SETB(*f,a,(v&0xFF)<<((7-a)*8));
    SETB(*f,b,((v>>8)&0xFF)<<((7-b)*8));
    SETB(*f,c,((v>>16)&0xFF)<<((7-c)*8));
}

// Rotação de 90 graus no sentido horário da própria face
// Cada sticker ocupa 8 bits, logo uma rotação de 2 posições = 16 bits
// i numero de rotações, 1-90 2-180 3-270 4-360(identidade)
/* Rotate face value right/left by (16 * i) bit positions. i must be in [1, 3]. */
static inline face_t rot_cw(face_t f, uint8_t i) {
    assert((i >= 1) & (i <= 3));
    uint16_t shift = 16u * i;
    return (f >> shift) | (f << (64u - shift));
}

static inline face_t rot_ccw(face_t f, uint8_t i) {
    assert((i >= 1) & (i <= 3));
    uint16_t shift = 16u * i;
    return (f << shift) | (f >> (64u - shift));
}

static inline void copy_self(Cube* restrict cube_arr) {
    cube_arr[0] = cube_arr[1];    
}

static inline void copy_cube(Cube* restrict cube_dest, const Cube* restrict cube_origin,const int8_t size) {
    for(uint8_t i = 0; i<size;i++) {
        cube_dest[i] = cube_origin[i];      
    }
}

static inline void cycle_edges (
    Cube* restrict cube_arr,
    uint8_t f1, uint8_t a1, uint8_t b1, uint8_t c1, face_t val1,
    uint8_t f2, uint8_t a2, uint8_t b2, uint8_t c2, face_t val2,
    uint8_t f3, uint8_t a3, uint8_t b3, uint8_t c3, face_t val3,
    uint8_t f4, uint8_t a4, uint8_t b4, uint8_t c4, face_t val4)  {
    unpack3(&cube_arr[1].f[cube_orientation[f1]], a1, b1, c1, val1);
    unpack3(&cube_arr[1].f[cube_orientation[f2]], a2, b2, c2, val2);
    unpack3(&cube_arr[1].f[cube_orientation[f3]], a3, b3, c3, val3);
    unpack3(&cube_arr[1].f[cube_orientation[f4]], a4, b4, c4, val4);
}

// ------------------------ STATIC INLINES - Getters ------------------------

static inline void get_corner(const Cube* restrict c, uint8_t id, uint8_t* restrict out) {
    // Copia a estrutura pequena do canto para os registos do CPU de uma vez
    const Corner cur_corner = corners[id]; 

    #pragma GCC unroll 3
    for(int k = 0; k < 3; k++) {
        out[k] = BYTE(c->f[cube_orientation[cur_corner.f[k]]], cur_corner.i[k]);
    }
}

static inline void get_edge(const Cube* restrict c,const uint8_t id, uint8_t* restrict out) {
    const Edge cur_edge = edges[id];

    #pragma GCC unroll 2
    for(int k=0;k<2;k++)
        out[k] = BYTE(c->f[cube_orientation[cur_edge.f[k]]], cur_edge.i[k]);
}

static inline uint8_t piece_dest(const uint8_t* restrict col, const uint8_t size, uint8_t (*cond)(const uint8_t*, uint8_t) ) {
    for(uint8_t i=0;i<size;i++) {
        if(cond(col,i)) {
            return i;
        }
    }
    return INT8_MAX;
}

// ------------------------ STATIC INLINES - Checkers ------------------------

static inline uint8_t condition_edge_align (const uint8_t* restrict col,const uint8_t i) {
    const Edge cur_edge = edges[i];
    return (edge_has_color(col[0],col[1],cube_orientation[cur_edge.f[0]]) & edge_has_color(col[0],col[1],cube_orientation[cur_edge.f[1]]));
}

static inline uint8_t condition_corner_align (const uint8_t* restrict col,const uint8_t i) {
    const Corner cur_corner = corners[i];
    return (corner_has_color(col[0],col[1],col[2],cube_orientation[cur_corner.f[0]]) & corner_has_color(col[0],col[1],col[2],cube_orientation[cur_corner.f[1]]) & corner_has_color(col[0],col[1],col[2],cube_orientation[cur_corner.f[2]]));
}

static inline uint8_t condition_corner_flip(const uint8_t* restrict col,const uint8_t i) {
    const Corner cur_corner = corners[i];
    return (col[0]!=cube_orientation[cur_corner.f[0]]);
}

// ------------------------ MOVE FUNCTIONS ------------------------

static inline void Fm(Cube* restrict cube_arr) {
    cube_arr[1].f[cube_orientation[FRONT]] = rot_cw(cube_arr[1].f[cube_orientation[FRONT]], 1);

    cycle_edges(cube_arr,
        UP,    4,5,6, pack3 (cube_arr[0].f[cube_orientation[LEFT]],  2,3,4),
        LEFT,  2,3,4, pack3 (cube_arr[0].f[cube_orientation[DOWN]],  0,1,2),
        DOWN,  0,1,2, pack3r(cube_arr[0].f[cube_orientation[RIGHT]], 0,7,6),
        RIGHT, 0,7,6, pack3r(cube_arr[0].f[cube_orientation[UP]],    4,5,6)
    );
    
    copy_self(cube_arr);
}

static inline void Um(Cube* restrict cube_arr) {
    cube_arr[1].f[cube_orientation[UP]] = rot_cw(cube_arr[1].f[cube_orientation[UP]], 1);
    
    cycle_edges(cube_arr,
        BACK,  0,1,2, pack3(cube_arr[0].f[cube_orientation[LEFT]],  0,1,2),
        LEFT,  0,1,2, pack3(cube_arr[0].f[cube_orientation[FRONT]], 0,1,2),
        FRONT, 0,1,2, pack3(cube_arr[0].f[cube_orientation[RIGHT]], 0,1,2),
        RIGHT, 0,1,2, pack3(cube_arr[0].f[cube_orientation[BACK]],  0,1,2)
    );
    
    copy_self(cube_arr);
}

static inline void Rm(Cube* restrict cube_arr) {
    cube_arr[1].f[cube_orientation[RIGHT]] = rot_cw(cube_arr[1].f[cube_orientation[RIGHT]], 1);

    cycle_edges(cube_arr,
        UP,    2,3,4, pack3 (cube_arr[0].f[cube_orientation[FRONT]], 2,3,4),
        FRONT, 2,3,4, pack3 (cube_arr[0].f[cube_orientation[DOWN]],  2,3,4),
        DOWN,  2,3,4, pack3r(cube_arr[0].f[cube_orientation[BACK]],  0,7,6),
        BACK,  0,7,6, pack3r(cube_arr[0].f[cube_orientation[UP]],    2,3,4)
    );

    copy_self(cube_arr);
}

static inline void Lm(Cube* restrict cube_arr) {
    cube_arr[1].f[cube_orientation[LEFT]] = rot_cw(cube_arr[1].f[cube_orientation[LEFT]], 1);

    cycle_edges(cube_arr,
        UP,    0,7,6, pack3r(cube_arr[0].f[cube_orientation[BACK]],  2,3,4),
        BACK,  2,3,4, pack3r(cube_arr[0].f[cube_orientation[DOWN]],  0,7,6),
        DOWN,  0,7,6, pack3 (cube_arr[0].f[cube_orientation[FRONT]], 0,7,6),
        FRONT, 0,7,6, pack3 (cube_arr[0].f[cube_orientation[UP]],    0,7,6)
    );
    
    copy_self(cube_arr);
}

static inline void Dm(Cube* restrict cube_arr) {
    cube_arr[1].f[cube_orientation[DOWN]] = rot_cw(cube_arr[1].f[cube_orientation[DOWN]], 1);

    cycle_edges(cube_arr,
        FRONT, 4,5,6, pack3(cube_arr[0].f[cube_orientation[LEFT]],  4,5,6),
        LEFT,  4,5,6, pack3(cube_arr[0].f[cube_orientation[BACK]],  4,5,6),
        BACK,  4,5,6, pack3(cube_arr[0].f[cube_orientation[RIGHT]], 4,5,6),
        RIGHT, 4,5,6, pack3(cube_arr[0].f[cube_orientation[FRONT]], 4,5,6)
    );
    
    copy_self(cube_arr);
}

static inline void Bm(Cube* restrict cube_arr) {
    cube_arr[1].f[cube_orientation[BACK]] = rot_cw(cube_arr[1].f[cube_orientation[BACK]], 1);

    cycle_edges(cube_arr,
        UP,    0,1,2, pack3 (cube_arr[0].f[cube_orientation[RIGHT]], 2,3,4),
        RIGHT, 2,3,4, pack3 (cube_arr[0].f[cube_orientation[DOWN]],  4,5,6),
        DOWN,  4,5,6, pack3r(cube_arr[0].f[cube_orientation[LEFT]],  0,7,6),
        LEFT,  0,7,6, pack3r(cube_arr[0].f[cube_orientation[UP]],    0,1,2)
    );
    
    copy_self(cube_arr);
}

void parse_cube(Cube* restrict cube_arr, const char* cube_string);
uint8_t valid_cube_config(const Cube* restrict c);
void print_cube(const Cube* restrict c);
void apply_alg(Cube* restrict c, Solution* sol, const char* alg);

#endif /* CUBE_H */

