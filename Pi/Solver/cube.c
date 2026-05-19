#include "cube.h"
#include "solution.h"

static const uint8_t ORIENTATION_LUT[6][6][6] = {
    // ---- TOPO = FRONT (0) ----
   [FRONT] = {
      // Se Topo=FRONT, a Frente nunca pode ser FRONT (0) nem BACK (3)
      [UP]    = {UP, LEFT, DOWN, RIGHT, FRONT, BACK}, // Exemplo de preenchimento (ajuste conforme o seu referencial)
      [RIGHT] = {RIGHT, UP, LEFT, DOWN, FRONT, BACK}, 
      [LEFT]  = {LEFT, DOWN, RIGHT, UP, FRONT, BACK},
      [DOWN]  = {DOWN, RIGHT, UP, LEFT, FRONT, BACK}
   },
   // ---- TOPO = RIGHT (1) ----
   [RIGHT] = {
      [UP]    = {UP, FRONT, DOWN, BACK, RIGHT, LEFT},
      [FRONT] = {FRONT, DOWN, BACK, UP, RIGHT, LEFT},
      [BACK]  = {BACK, UP, FRONT, DOWN,RIGHT, LEFT},
      [DOWN]  = {DOWN, BACK, UP, FRONT, RIGHT, LEFT}
   },
   // ---- TOPO = LEFT (2) ----
   [LEFT] = {
      [UP]    = {UP, BACK, DOWN, FRONT, LEFT, RIGHT},
      [FRONT] = {FRONT, UP, BACK, DOWN, LEFT, RIGHT},
      [BACK]  = {BACK, DOWN, FRONT, UP, LEFT, RIGHT},
      [DOWN]  = {DOWN, FRONT, UP, BACK, LEFT, RIGHT}
   },
   // ---- TOPO = BACK (3) ----
   [BACK] = {
      // Se Topo=FRONT, a Frente nunca pode ser FRONT (0) nem BACK (3)
      [UP]    = {UP, RIGHT, DOWN, LEFT, BACK, FRONT}, // Exemplo de preenchimento (ajuste conforme o seu referencial)
      [RIGHT] = {RIGHT, DOWN, LEFT, UP, BACK, FRONT}, 
      [LEFT]  = {LEFT, UP, RIGHT, DOWN, BACK, FRONT},
      [DOWN]  = {DOWN, LEFT, UP, RIGHT, BACK, FRONT}
   },
      // ---- TOPO = UP (4) ----
   [UP] = {
      [FRONT] = {FRONT, RIGHT, BACK, LEFT, UP, DOWN},   // Esta é a sua orientação padrão inicial!
      [RIGHT] = {RIGHT, BACK, LEFT, FRONT, UP, DOWN},   // Se rodar o cubo para a esquerda (U passa a ser fixo, R vira Frente)
      [LEFT]  = {LEFT, FRONT, RIGHT, BACK, UP, DOWN},
      [BACK]  = {BACK, LEFT, FRONT, RIGHT, UP, DOWN}
   },
   // ---- TOPO = DOWN (5) ----
   [DOWN] = {
      [FRONT] = {FRONT, LEFT, BACK, RIGHT, DOWN, UP},   // Esta é a sua orientação padrão inicial!
      [RIGHT] = {RIGHT, FRONT, LEFT, BACK, DOWN, UP},   // Se rodar o cubo para a esquerda (U passa a ser fixo, R vira Frente)
      [LEFT]  = {LEFT, BACK, RIGHT, FRONT, DOWN, UP},
      [BACK]  = {BACK, RIGHT, FRONT, LEFT, DOWN, UP}
   },
};

const uint8_t*  cube_orientation = ORIENTATION_LUT[UP][FRONT];

// ------------------------ CONSTANTS ------------------------
/* Maps cube stickers from the string array to the BitBoard Representation */
static const uint8_t map[8] = {
    0, // 0 -> 0
    1, // 1 -> 1
    2, // 2 -> 2
    5, // 5 -> 3
    8, // 8 -> 4
    7, // 7 -> 5
    6, // 6 -> 6
    3  // 3 -> 7
};

static const uint8_t sideroll_rotation[] = {UP, LEFT, DOWN, RIGHT};

static const uint8_t XRotConfig[2][4] = {{1,1,0,1}, // FRONT (RIGHT: CW 1,  LEFT: CCW 1)
                                        {0,1,1,1}}; // BACK  (RIGHT: CCW 1, LEFT: CW 1)

static const uint8_t ZRotConfig[3][4] = {{0,1,1,1}, // RIGHT (FRONT: CCW 1, BACK: CW 1)
                                        {1,1,0,1},  // LEFT  (FRONT: CW 1,  BACK: CCW 1)
                                        {0,2,1,2}}; // DOWN  (FRONT: CCW 2, BACK: CW 2)

static const uint8_t YRotConfig[3][4] = {{1,1,0,1}, // RIGHT (UP: CW 1, DOWN: CCW 1)
                                        {0,1,1,1},  // LEFT  (UP: CCW 1, DOWN: CW 1)
                                        {0,2,1,2}}; // BACK  (UP: CCW 2, DOWN: CW 2)


//------------------------ CUBE PARSER ------------------------

static inline uint8_t char_to_color(char c) {
   switch(c) {
      case 'F': return GREEN;
      case 'R': return RED;
      case 'B': return BLUE;
      case 'L': return ORANGE;
      case 'U': return WHITE;
      case 'D': return YELLOW;
      default:  return UINT8_MAX; /* invalid/error sentinel */
   }
}

void parse_cube(Cube* restrict cube_arr, const char* cube_string) {
   char c;
   uint8_t face_size = 9;
   face_t face = 0;

   for(uint8_t j=0; j<N_FACES;j++) {
      face = 0;
      for(uint8_t i=0; i<8;i++) {                  //  o meu ring só tem 8, fazer match da string no meu modelo
                                                   //  index da string face face ao displacement
         c = cube_string[map[i]+(j*face_size)];    //  vai buscar o index na string correspondente ao index do meu ring

         face |= ((uint64_t)char_to_color(c) << ((7-i)*8));
      }
      cube_arr[0].f[face_order[j]] = face;
      cube_arr[1].f[face_order[j]] = face;
   }
}

// ------------------------ PRINT CUBE ------------------------

void print_cube(const Cube* cube) {

   printf("Cube State: \n");
   for(int i=0; i<N_FACES;i++) {
      printf(" %s \t%s \t%016llx \n",sides[i],colors[cube_orientation[i]],(unsigned long long)cube->f[cube_orientation[i]]);
   }
   printf("\n");
}

// ------------------------ CUBE VALIDITY -----------------

void get_pieces(const Cube* c,uint8_t* pieces, uint8_t size){
   uint8_t col[3];

   for(uint8_t i=0; i<size;i++) {
      if(size==N_CORNERS) {
         get_corner(c,i,col);
         pieces[i] = piece_dest(col,size,condition_corner_align);
      }
      else {
         get_edge(c,i,col);
         pieces[i] = piece_dest(col,size,condition_edge_align);
      }
   }
}

uint8_t countSwaps(uint8_t *pieces, uint8_t count) // pieces[5] = 3 means piece 3 is in position 5.
{   
   uint8_t swaps = 0;
   for(uint8_t pos = 0; pos < count; ++pos) {
      if (pieces[pos] != pos) {
         uint8_t dst = pieces[pos];
         pieces[pos] = pieces[dst];
         pieces[dst] = dst; // One piece is swapped into its correct position.
         ++swaps;
         --pos; // Force re-inspection of this position;
      }
   }
   return swaps;
}

uint8_t corner_perms(const Cube* c){
   uint8_t pieces[N_CORNERS];
   get_pieces(c,pieces,N_CORNERS);
   return countSwaps(pieces,N_CORNERS);
}

uint8_t edge_perms(const Cube* c){
   uint8_t pieces[N_EDGES];
   get_pieces(c,pieces,N_EDGES);
   return countSwaps(pieces,N_EDGES);
}

uint8_t permutations(const Cube* c){
   uint8_t edge_swaps = edge_perms(c);
   uint8_t corner_swaps = corner_perms(c);
   #if VALID
      printf(" \tEdge Swaps: %d\n",edge_swaps);
      printf(" \tCorner Swaps: %d\n\n",corner_swaps);
   #endif
   if( (corner_swaps+edge_swaps) & 1) {
      fprintf(stderr, "\nERROR %d: Invalid Cube Configuration \n\t\tOdd Permutation Parity\n",ERR_INVALID_CUBE);
      exit(ERR_INVALID_CUBE);
      return 0;
   }
   return 1;
}

int8_t get_corner_validity(const Cube* c) {
   int8_t corner_ori = 0;
   uint8_t map_corner_ori[] = {0,1,2,3,3,2,1,0};
   int8_t map_ori[4][2] = {{1,-1},{-1,1},{1,-1},{-1,1}};
   for(uint8_t i=0; i<N_CORNERS;i++) {
      uint8_t col[3];
      get_corner(c,i,col);

      //parity check
      if(col[0]!=cube_orientation[UP]&&col[0]!=cube_orientation[DOWN]) {//0 this case would be 0

         if(col[1]==cube_orientation[UP] || col[1]==cube_orientation[DOWN]) //Back|FRONT is yellow or white
                  corner_ori+= map_ori[map_corner_ori[i]][0];
         }
         else{   //Left|Right is yellow or white
               corner_ori+= map_ori[map_corner_ori[i]][1];
         }
      }
   return corner_ori;
}

//There should be a mod3 number of "wrong edges"
uint8_t corner_parity(const Cube* c) {
   int8_t par = get_corner_validity(c);
   #if VALID
      printf(" \tCorner Parity: %d Parity: %d\n",par,par%3);
   #endif
   if(par % 3){ //if par%3!=0
      fprintf(stderr, "\nERROR %d: Invalid Cube Configuration \n\t\tOdd Corner Parity\n",ERR_INVALID_CUBE);
      exit(ERR_INVALID_CUBE);
      return 0;
   }
   return 1;
}

/*parity: 
   key positions 
      UP and DOWN facing edges : 8 U/D facing colors
      FL FR BL BR edges : 4 : F/B facing colors      
   Any Pieces that have yellow/white (up/down color) in those positions
   Any pieces that have Red or Orange (R/L color) in those positions that are NOT attached to yellow/white
*/
uint8_t get_edge_validity(const Cube* c) {
   uint8_t edge_ori = 0;
   for(uint8_t i=0; i<N_EDGES;i++) {
      uint8_t col[2];
      get_edge(c,i,col);

      //parity check
      if(col[0]==cube_orientation[UP] || col[0]==cube_orientation[DOWN]) {
               edge_ori++;
      }
      else if(col[0]==cube_orientation[LEFT] || col[0]==cube_orientation[RIGHT]){
         if(col[1]!=cube_orientation[UP]&&col[1]!=cube_orientation[DOWN]) {
               edge_ori++;
         }
      }
   }

   return edge_ori;
}
//There should be an even number of "wrong edges"
uint8_t edge_parity(const Cube* c) {
   uint8_t par = get_edge_validity(c);
      
   #if VALID
      printf(" \tEdge Parity: %d Parity: %d\n",par,par&1);
   #endif
   if(par & 1){
      fprintf(stderr, "\nERROR %d: Invalid Cube Configuration \n\t\tOdd Edge Parity\n",ERR_INVALID_CUBE);
      exit(ERR_INVALID_CUBE);
      return 0;
   }
   return 1;
}

uint8_t validate_colors(const Cube* c) {
   uint8_t count[N_FACES] = {0};
   for(uint8_t face=0; face<6; face++) {   
      for(uint8_t i=0; i<8; i++) {
         uint8_t color = BYTE(c->f[face],i);
         count[color]++;
      }
   }

   for(uint8_t i=0; i<N_FACES;i++) {
      if(count[i]!=8) {
         fprintf(stderr, "\nERROR %d: Invalid Cube Configuration \n\t\t%s has %d stickers \n",ERR_INVALID_CUBE, colors[cube_orientation[i]],count[i]);
         exit(ERR_INVALID_CUBE);
         return 0;
      }
   }

   return 1;
}

uint8_t valid_cube_config(const Cube* c) {

   #if VALID 
      printf("------------------------------------ Checking Cube Configuration ------------------------------------\n");
   #endif
    // Fail fast: every invariant must pass for config to be trusted.
   return validate_colors(c) && edge_parity(c) && corner_parity(c) && permutations(c);
}

// ------------------------ ROTATE CUBE ------------------------

void rotate_x(Cube* restrict cube_arr, uint8_t new_top) {

   uint8_t new_front[] = {DOWN,UP};

   int8_t i = new_top == FRONT ? 0 : new_top == BACK? 1 : -1;
   if(i==-1 || cube_orientation[new_top] == cube_orientation[UP] ) return; //Safe
   uint8_t old_back = cube_orientation[BACK];

   cube_orientation = ORIENTATION_LUT[cube_orientation[new_top]][cube_orientation[new_front[i]]];

   cube_arr[1].f[cube_orientation[RIGHT]]  = (XRotConfig[i][0]) ? rot_cw(cube_arr[1].f[cube_orientation[RIGHT]],XRotConfig[i][1]) : rot_ccw(cube_arr[1].f[cube_orientation[RIGHT]],XRotConfig[i][1]);
   cube_arr[1].f[cube_orientation[LEFT]]   = (XRotConfig[i][2]) ? rot_cw(cube_arr[1].f[cube_orientation[LEFT]],XRotConfig[i][3]) : rot_ccw(cube_arr[1].f[cube_orientation[LEFT]],XRotConfig[i][3]);
   cube_arr[1].f[old_back]                 = rot_cw(cube_arr[1].f[old_back],2);
   cube_arr[1].f[cube_orientation[BACK]]   = rot_cw(cube_arr[1].f[cube_orientation[BACK]],2);

   copy_self(cube_arr);
}

void rotate_z(Cube* restrict cube_arr, uint8_t new_top) {

   int8_t i = new_top==RIGHT ? 0 : new_top == LEFT? 1 : new_top == DOWN ? 2 : -1;
   if((i==-1) || cube_orientation[new_top] == cube_orientation[UP]) return; //SAFE

   cube_orientation = ORIENTATION_LUT[cube_orientation[new_top]][cube_orientation[FRONT]];
      
   for(uint8_t j=0; j<ROT_SIZE; j++) {
      uint8_t index = cube_orientation[sideroll_rotation[j]];
      cube_arr[1].f[index] = (ZRotConfig[i][0]) ? rot_cw(cube_arr[1].f[index],ZRotConfig[i][1]) : rot_ccw(cube_arr[1].f[index],ZRotConfig[i][1]);
   }
   cube_arr[1].f[cube_orientation[FRONT]] = (ZRotConfig[i][0]) ? rot_cw(cube_arr[1].f[cube_orientation[FRONT]],ZRotConfig[i][1]) : rot_ccw(cube_arr[1].f[cube_orientation[FRONT]],ZRotConfig[i][1]);
   cube_arr[1].f[cube_orientation[BACK]]  = (ZRotConfig[i][2]) ? rot_cw(cube_arr[1].f[cube_orientation[BACK]],ZRotConfig[i][3]) : rot_ccw(cube_arr[1].f[cube_orientation[BACK]],ZRotConfig[i][3]);
   copy_self(cube_arr);
}

void rotate_y(Cube* restrict cube_arr, uint8_t new_front) {

   int8_t i = new_front==RIGHT ? 0 : new_front == LEFT? 1 : new_front == BACK ? 2 : -1;

   if((i==-1) || cube_orientation[new_front] == cube_orientation[FRONT]) return; //SAFE
   
   cube_orientation = ORIENTATION_LUT[cube_orientation[UP]][cube_orientation[new_front]];
   
   cube_arr[1].f[cube_orientation[UP]] = (YRotConfig[i][0]) ? rot_cw(cube_arr[1].f[cube_orientation[UP]],YRotConfig[i][1]) : rot_ccw(cube_arr[1].f[cube_orientation[UP]],YRotConfig[i][1]);
   cube_arr[1].f[cube_orientation[DOWN]]  = (YRotConfig[i][2]) ? rot_cw(cube_arr[1].f[cube_orientation[DOWN]],YRotConfig[i][3]) : rot_ccw(cube_arr[1].f[cube_orientation[DOWN]],YRotConfig[i][3]);
   copy_self(cube_arr);
}

// ------------------------ APPLY ALG ------------------------

uint8_t apply_move(Cube* restrict c, char m){
   switch(m){
      case 'F': 
         Fm(c); 
         return FRONT;
         break;
      case 'U': 
         Um(c); 
         return UP;
         break;
      case 'R':
         Rm(c); 
         return RIGHT;
         break;
      case 'L': 
         Lm(c);
         return LEFT;
         break;
      case 'B': 
         Bm(c);
         return BACK;
         break;
      case 'D': 
         Dm(c);
         return DOWN;
         break;
      case 'a':
         rotate_y(c,RIGHT);
         return ROT_RIGHT;
         break;
      case 'b':
         rotate_y(c,LEFT);
         return ROT_LEFT;
         break;
      case 'c':
         rotate_y(c,BACK);
         return ROT_BACK;
         break;        
      case 'd':
         rotate_x(c,BACK); //up to front == Back to up
         return ROL_UP;
         break;   
      case 'e':
         rotate_x(c,FRONT);  //front to up == down to front
         return ROL_FRONT;
         break;
      case 'f':
         rotate_z(c,RIGHT);
         return ROL_RIGHT;
         break;   
      case 'g':
         rotate_z(c,LEFT);
         return ROL_LEFT;
         break;   
      case 'h':
         rotate_z(c,DOWN);   //down to up
         return ROL_DOWN;
         break;
      
      default:
         return UINT8_MAX;
         break;
   }
}

void apply_alg(Cube* restrict c, Solution* sol, const char* alg){
   int len = strlen(alg);

   for(uint8_t i=0; i<len; i++){
      char m = alg[i];
      if(m==' ') continue;

      uint8_t move=0;
      uint8_t times=1;
      if(alg[i+1]=='2'){ times=2; i++; }
      else if(alg[i+1]=='\''){ times=3; i++; }

      for(uint8_t k=0;k<times;k++)
         move = apply_move(c,m);
      
      if(sol!=NULL && move!=UINT8_MAX){ //if it's just the work cube we don't want to record any of its moves
         if(move<ROT_RIGHT) {
               move = move*3+(times-1);
         }
         push_move(sol, move);
      }   

   }
}
