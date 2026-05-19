#include "config.h"

// ------------------------ GLOBAL CONSTS ------------------------

const Edge edges[N_EDGES]  = {
    {{UP,BACK}, {1,1}}, // UB
    {{UP,RIGHT},{3,1}}, // UR
    {{UP,FRONT},{5,1}}, // UF
    {{UP,LEFT}, {7,1}}, // UL

    {{DOWN,FRONT},{1,5}}, // DF
    {{DOWN,RIGHT},{3,5}}, // DR
    {{DOWN,BACK}, {5,5}}, // DB
    {{DOWN,LEFT}, {7,5}}, // DL

    {{FRONT,RIGHT},{3,7}}, // FR
    {{FRONT,LEFT}, {7,3}}, // FL
    {{BACK,RIGHT}, {7,3}}, // BR
    {{BACK,LEFT},  {3,7}}, // BL
};

const Corner corners[N_CORNERS] = {
    // U layer
    {{UP, FRONT, LEFT}, {6, 0, 2}},     // UFL
    {{UP, FRONT, RIGHT}, {4, 2, 0}},    // UFR
    {{UP, BACK, RIGHT}, {2, 0, 2}},     // UBR      //isto está da perspectiva de quem vê o cubo de frente
    {{UP, BACK, LEFT}, {0, 2, 0}},      // UBL       
    // D layer
    {{DOWN, FRONT, LEFT}, {0, 6, 4}},   // DFL
    {{DOWN, FRONT, RIGHT}, {2, 4, 6}},  // DFR
    {{DOWN, BACK, RIGHT}, {4, 6, 4}},   // DBR
    {{DOWN, BACK, LEFT}, {6, 4, 6}}     // DBL
};

//Mask arrays and Positional arrays

const uint8_t face_order[N_FACES] = {UP, RIGHT, FRONT, DOWN, LEFT, BACK};

//print friendly

const char *const colors[N_FACES] = {"Green","Red","Blue","Orange","White","Yellow"};
const char *const sides[N_FACES] = {"Front","Right","Back","Left","Up","Down"};
const char *const corner_names[N_CORNERS] = {"UFL","UFR","UBR","UBL","DFL","DFR","DBR","DBL"};
const char *const edges_names[N_EDGES] = {"UB","UR","UF","UL","DF","DR","DB","DL","FR","FL","BR","BL"};
