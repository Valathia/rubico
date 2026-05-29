#include "solution.h"

// ------------------------ SOLUTION CONSTS ------------------------
/* Move Mappings*/
// MOVE MOVE = MOVE2
// MOVE MOVE2 = MOVE'
// MOVE MOVE' = NO MOVE
// MOVE2 MOVE2 = NO MOVE
// MOVE2 MOVE' = MOVE
// MOVE' MOVE' = MOVE2
/* Maps optimizations of same movement moves to the correct move %3
   Taking advantage of their order in the Move enum  */
static const uint8_t move_opt_table[3][3] = {
   {1,2,UINT8_MAX},
   {2,UINT8_MAX,0},
   {UINT8_MAX,0,1}    
};

// ------------------------ SOLUTION FUNCTIONS ------------------------

const char* move_to_str(const Move m)  {
   switch(m) {
      case MOVE_F    :     return F;
      case MOVE_F2   :     return F2;
      case MOVE_Fp   :     return F_P;
      case MOVE_R    :     return R;
      case MOVE_R2   :     return R2;
      case MOVE_Rp   :     return R_P;
      case MOVE_B    :     return B;
      case MOVE_B2   :     return B2;
      case MOVE_Bp   :     return B_P;
      case MOVE_L    :     return L;
      case MOVE_L2   :     return L2;
      case MOVE_Lp   :     return L_P;
      case MOVE_U    :     return U;
      case MOVE_U2   :     return U2;
      case MOVE_Up   :     return U_P;
      case MOVE_D    :     return D;
      case MOVE_D2   :     return D2;
      case MOVE_Dp   :     return D_P;
      case ROT_RIGHT :     return "y ";  // "RRF" ; 
      case ROT_BACK  :     return "y2 "; //"RBF" ;
      case ROT_LEFT  :     return "y' "; //"RLF" ;  
      case ROL_RIGHT :     return "z' "; //"RRU" ;
      case ROL_DOWN  :     return "z2 "; //"RDU" ;
      case ROL_LEFT  :     return "z ";  //"RLU" ;
      case ROL_UP    :     return "x' "; //"RUF" ;
      case ROL_FRONT :     return "x ";  //"RFU" ;
      case ROL_DOWN_X:     return "x2";
      default: return "?";
   }   
}

Solution* optimize_sol(Solution* restrict s) {
   uint16_t len = s->length;

   if (len < 2) {
      return s;      //nothing to optimize, return the same empty sol
   }

   Solution* new_sol = (Solution*)malloc(sizeof(Solution));
   if (new_sol == NULL) {
      fprintf(stderr, "\nERROR: Failed to allocate optimized solution\n");
      return NULL;
   }

   new_sol->length = 0;
   Move cur_move = s->moves[0];
   push_move(new_sol,cur_move);

   for(uint16_t i=1; i<len;i++){
      const Move top = new_sol->moves[new_sol->length-1];
      cur_move = s->moves[i];
      const uint8_t top_i = top/3;
      const uint8_t cur_move_i = cur_move/3;


      if((top_i!=cur_move_i)) {
         push_move(new_sol,cur_move);
      }
      else{
         uint8_t move_dif = move_opt_table[cur_move%3][top%3];
         new_sol->length--;
         if(move_dif!=UINT8_MAX) {
            push_move(new_sol,cur_move_i*3+move_dif);

         }
      }
   }

   return new_sol;
}

void print_solution(const Solution s) {
   for(int i = 0; i < s.length; i++) {
      printf("%s", move_to_str(s.moves[i]));
   }
   printf("\n");
}
