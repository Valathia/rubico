#include "algorithms.h"

// ------------------------ STD ALGS ------------------------

#define SUB_MID_EDGE_RIGHT R_ALG ROT_Y_R L_ALG
#define SUB_MID_EDGE_LEFT  L_ALG ROT_Y_L R_ALG

// ------------------------ MOVE TABLES ------------------------

//move tables are organized as: Origin Piece | Destination Piece | Target Color Orientation
const char *const corner_move_table[N_CORNERS][4][3] = {
   //UFL
   {
      //DFL
      {
         ROT_Y_L R_ALG,
         L_ALG L_ALG L_ALG,
         L_ALG
      },
      //DFR
      {
         "U' " R_ALG,
         "U' " R_ALG R_ALG R_ALG,
         "U' a " L_ALG
      },
      //DBR
      {
         "U2 a " R_ALG,
         "U2 c " L_ALG L_ALG L_ALG,
         "U2 c " L_ALG
      },
      //DBL
      {
         "U c " R_ALG,
         "U c " R_ALG R_ALG R_ALG,
         "U b " L_ALG
      }
   },

   //UFR
   {
      //DFL
      {
         "U " L_ALG,
         "U " L_ALG L_ALG L_ALG,
         "U b " R_ALG
      },
      //DFR
      {
         ROT_Y_R L_ALG,
         R_ALG R_ALG R_ALG,
         R_ALG
      },
      //DBR
      {
         "U' c " L_ALG,
         "U' c " L_ALG L_ALG L_ALG,
         "U' a " R_ALG
      },
      //DBL
      {
         "U2 b " L_ALG,
         "U2 c " R_ALG R_ALG R_ALG,
         "U2 c " R_ALG
      }
   },

   //UBR
   {
      //DFL
      {
         U2 ROT_Y_L R_ALG,
         U2 L_ALG L_ALG L_ALG,
         U2 L_ALG
      },
      //DFR
      {
         U R_ALG,
         U R_ALG R_ALG R_ALG,
         U ROT_Y_R L_ALG
      },
      //DBR
      {
         ROT_Y_R R_ALG,
         ROT_Y_B L_ALG L_ALG L_ALG,
         ROT_Y_B L_ALG
      },
      //DBL
      {
         "U' c " R_ALG,
         "U' c " R_ALG R_ALG R_ALG,
         "U' b " L_ALG
      }
   },

   //UBL
   {
      //DFL
      {
         U_P L_ALG,
         U_P L_ALG L_ALG L_ALG,
         "U' b " R_ALG
      },
      //DFR
      {
         "U2 a " L_ALG,
         U2 R_ALG R_ALG R_ALG,
         U2 R_ALG
      },
      //DBR
      {
         "U c " L_ALG,
         "U c " L_ALG L_ALG L_ALG,
         "U a " R_ALG
      },
      //DBL
      {
         ROT_Y_L L_ALG,
         ROT_Y_B R_ALG R_ALG R_ALG,
         ROT_Y_B R_ALG
      },
   },
   //DFL if in the down corner position and wrong, we need to take them out and re-insert them -- DOWN CORNERS added algorithms to finish table
   {  
      //DFL -- taking them out means they go to the UFL pos (for FL)
      {
         L_ALG L_ALG L_ALG L_ALG,   //se F em D -> U em U
         "",      //trivialmente correcto, se branco para baixo e target é DFL
         L_ALG L_ALG    // se L -> L
      },
      //DFR
      {
         L_ALG U_P R_ALG R_ALG R_ALG,
         L_ALG U_P R_ALG,
         L_ALG U_P ROT_Y_R L_ALG
      },
      //DBR
      {
         L_ALG U2 ROT_Y_B L_ALG L_ALG L_ALG,
         L_ALG U2 ROT_Y_R R_ALG,
         L_ALG U2 ROT_Y_B L_ALG
      },
      //DBL
      {
         L_ALG U ROT_Y_B R_ALG R_ALG R_ALG,
         L_ALG U ROT_Y_B R_ALG,
         L_ALG U ROT_Y_L L_ALG
      }
   },
   //DFR
   {
      //DFL
      {
         R_ALG U L_ALG L_ALG L_ALG,
         R_ALG U L_ALG,
         R_ALG U ROT_Y_L R_ALG
      },
      //DFR
      {
         R_ALG R_ALG R_ALG R_ALG,
         "",                           //trivialmente correcto
         R_ALG R_ALG
      },
      //DBR
      {
         R_ALG U_P ROT_Y_B L_ALG L_ALG L_ALG,
         R_ALG U_P ROT_Y_B L_ALG,
         R_ALG U_P ROT_Y_R R_ALG
      },
      //DBL
      {
         R_ALG U2 ROT_Y_B R_ALG R_ALG R_ALG,
         R_ALG U2 ROT_Y_L L_ALG,
         R_ALG U2 ROT_Y_B R_ALG
      }
   },

   //DBR
   {     //After taking corner out, we have UFL case 
      //DFL Same as UFL -> DBR after rot and L_ALG
      {
         ROT_Y_B U2 ROT_Y_B L_ALG L_ALG L_ALG,
         ROT_Y_B U2 ROT_Y_R R_ALG,
         ROT_Y_B U2 ROT_Y_B L_ALG
      },
      //DFR  (UFL -> DBL)
      {
         ROT_Y_B L_ALG U ROT_Y_B R_ALG R_ALG R_ALG,
         ROT_Y_B L_ALG U ROT_Y_B R_ALG,
         ROT_Y_B L_ALG U ROT_Y_L L_ALG
      },
      //DBR (UFL -> DFL)
      {
         ROT_Y_B L_ALG L_ALG L_ALG L_ALG,
         "",
         ROT_Y_B L_ALG L_ALG
      },
      //DBL (UFL -> DFR)
      {
         ROT_Y_B L_ALG U_P R_ALG R_ALG R_ALG,
         ROT_Y_B L_ALG U_P R_ALG,
         ROT_Y_B L_ALG U_P ROT_Y_R L_ALG
      }
   },
   //DBL
   {     //After taking corner out, we have UFR case 
      //DFL (UFR -> DBR)
      {
         ROT_Y_B R_ALG U_P ROT_Y_B L_ALG L_ALG L_ALG,
         ROT_Y_B R_ALG U_P ROT_Y_B L_ALG,
         ROT_Y_B R_ALG U_P ROT_Y_R R_ALG
      },
      //DFR  (UFR -> DBL)
      {
         ROT_Y_B R_ALG U2 ROT_Y_B R_ALG R_ALG R_ALG,
         ROT_Y_B R_ALG U2 ROT_Y_L L_ALG,
         ROT_Y_B R_ALG U2 ROT_Y_B R_ALG
      },
      //DBR  (UFR -> DFL)
      {
         ROT_Y_B R_ALG U L_ALG L_ALG L_ALG,
         ROT_Y_B R_ALG U L_ALG,
         ROT_Y_B R_ALG U ROT_Y_L R_ALG
      },
      //DBL (UFR -> UFR)
      {
         ROT_Y_B R_ALG R_ALG R_ALG R_ALG,
         "",                                  // trivialmente certo
         ROT_Y_B R_ALG R_ALG
      }
   }
};

const char *const edge_move_table[N_EDGES][4][2] = {
    //UB
   {  
      //UB
      {
         //U F
         "", //solved
         "B L' D' L B'"
      },
      //UR
      {
         //U F
         "B2 D' R2",
         "B' R'"
      },
      //UF
      {
         //U F
         "B2 D2 F2",
         "B2 D R F' R'"
      },
      //UL
      {
         //U F
         "B2 D L2",
         "B L"
      }
   },    
   //UR
   {   
      //UB
      {
         //U F
         "R2 D B2",
         "R B"
      },
      //UR
      {
         //U F
         "",
         "R B' D' B R2"
      },
      //UF
      {
         //U F
         "R2 D' F2",
         "R' F'"
      },
      //UL
      {
         //U F
         "R2 D2 L2",
         "R B' D B L2"
      }
   }, 
   //UF
   {   
      //UB
      {
         //U F
         "F2 D2 B2",
         "U' R U B"
      },
      //UR
      {
         //U F
         "F2 D R2",
         "F R"
      },
      //UF
      {
         //U F
         "",     //trivially solved
         "F R' D' R F2"
      },
      //UL
      {
         //U F
         "F2 D' L2",
         "F' L"
      }
   },
   //UL
   {  
      //UB
      {
         //U F
         "L2 D' B2",
         "L' B'"
      },
      //UR
      {
         //U F
         "L2 D2 R2",
         "L' B D' B' R2"
      },
      //UF
      {
         //U F
         "L2 D F2",
         "L' B D2 B' F2"
      },
      //UL
      {
         //U F
         "",
         "L' B D B' L2"
      }
   },
   
   //DF
   {   
      //UB
      {
         //D F
         "D2 B2",
         "D R' B R",
      },
      //UR
      {
         //D F
         "D R2",
         "F' R F",
         
      },
      //UF
      {
         //D F
         F2,
         "D R F' R'"
      },
      //UL
      {
         //D F
         "D' L2",
         "F L' F'"
      }
   },
   //DR
   {  
      //UB
      {
         //U F
         "D B2",
         "R' B R"
      },
      //UR
      {
         //U F
         R2,
         "R' B' D' R2 B"
      },
      //UF
      {
         //U F
         "D' F2",
         "R F' R'"
      },
      //UL
      {
         //U F
         "D2 L2",
         "R F D' L2 F' R'"
      }
   },
   //DB
   {  
      //UB
      {
         //U F
         B2,
         "D' R' B R"
      },
      //UR
      {
         //U F
         "D' R2",
         "B R' B"
      },
      //UF
      {
         //U F
         "D2 F2",
         "D' R F' R'"
      },
      //UL
      {
         //U F
         "D L2",
         "B' L B"
      }
   },
   //DL
   {  
      //UB
      {
         //U F
         "D' B2",
         "L B' L'"
      },
      //UR
      {
         //U F
         "D2 R2",
         "D F' R F"
      },
      //UF
      {
         //U F
         "D F2",
         "L' F L"
      },
      //UL
      {
         //U F
         L2,
         "D F L' F'"
      }
   },
   
   //FR
   {   
      //UB
      {
         //F R
         "R' D2 B2 R",
         "F D2 B2 F'"
      },
      //UR
      {
         //F R
         R,
         "F D F' R2"
      },
      //UF
      {
         //F R
         "R' D' R F2",
         F_P
      },
      //UL
      {
         //F R
         "R' D2 L2 R",
         "F D' L2 F'"
      }
   },
   //FL
   {   
      //UB
      {
         //F L
         "L D2 B2 L'",
         "F' D2 B2 F"
      },        
      //UR
      {
         //F L
         "L D R2 L",
         "F' D R2 F"
      },
      //UF
      {
         //F L
         "L D L' F2",
         F
      },
      //UL
      {
         //F L
         L_P,
         "F' D' F L2"
      }
   },
   //BR
   {  
      //UB
      {
         //U F
         "R D R' B2",
         "B"            
      },
      //UR
      {
         //U F
         R_P,
         "B' D' B R2"    //adicionado reposição B
      },
      //UF
      {
         //U F
         "R D2 R' F2",
         "B' D2 B F2"    //adicionado reposição B
      },
      //UL
      {
         //U F
         "R D' R' L2",
         "B' D B L2"    //adicionado reposição B
      }
   },
   //BL
   {  
      //UB
      {
         //B L
         "L' D' L B2",  //UP matches Back  -- Branco virado para trás e cor à esquerda. 
         B_P           //UP matches LEFT  -- Branco à esquerda, Cor em Back==Back (bem orientado) (caso ideal)
      },
      //UR
      {
         //B L
         "L' D2 L R2",
         "B D' B' R2"   //adicionado reposição B'
      },
      //UF
      {
         //B L
         "L' D L F2",
         "B D2 B' F2"   //adicionado reposição B'
      },
      //UL
      {
         //B L
         L,         //UP matches Back  -- Branco virado para trás e cor à esquerda.   (bem orientado) (caso ideal)
         "B D B' L2"    //UP matches LEFT  -- Branco à esquerda, Cor em Back==Back --- faltava repor B
      }
   },
};

const char *const midle_edge_move_table[8][4][2] = {
   //UB
   {  
      //FR
      {
         //U B
         
         ROT_Y_R SUB_MID_EDGE_LEFT,  //Front color to up
         "U' " SUB_MID_EDGE_RIGHT //Front color to back
      },
      //FL
      {
         //U B
         ROT_Y_L SUB_MID_EDGE_RIGHT, //Front color to up
         "U " SUB_MID_EDGE_LEFT   //Front color to back
      },
      //BR
      {
         //U B
         "U2 a " SUB_MID_EDGE_RIGHT, //Back color to up --added
         "U' c " SUB_MID_EDGE_LEFT    //back color to back
      },
      //BL
      {
         //U B
         "U2 b " SUB_MID_EDGE_LEFT, //Back color to up
         "U c " SUB_MID_EDGE_RIGHT  //Back color to back ... to go to Left "U L' U' L U a R U R' U'"
      }
   },    
   //UR
   {   
      //FR
      {
         //U R
         "U' a " SUB_MID_EDGE_LEFT,  //front color to up //added
         "U2 " SUB_MID_EDGE_RIGHT      //front color toright
      },
      //FL
      {
         //U R
         "U' b " SUB_MID_EDGE_RIGHT,   //front color to up //Addeed
         SUB_MID_EDGE_LEFT         //front color to right
      },
      //BR
      {
         //U R
         "U a " SUB_MID_EDGE_RIGHT,      //back color to up     --added
         "U2 c " SUB_MID_EDGE_LEFT      //back color to right   --added
      },
      //BL
      {
         //U R
         "U b " SUB_MID_EDGE_LEFT,    //back color to up      --added
         ROT_Y_B SUB_MID_EDGE_RIGHT         //back color to right   --added
      }
   }, 
   //UF
   {   
      //FR
      {
         //U F
         "U2 a " SUB_MID_EDGE_LEFT,   //Front color to up  -- da match ao centro da direita added
         "U " SUB_MID_EDGE_RIGHT       //Front color to front
      },
      //FL
      {
         //U F
         "U2 b " SUB_MID_EDGE_RIGHT,   //Front color to up  -- da match ao centro da esquerda added
         "U' " SUB_MID_EDGE_LEFT      //Front color to front
      },
      //BR
      {
         //U F
         ROT_Y_R SUB_MID_EDGE_RIGHT,        //Back color to up -- match centro da direita --added
         "U c " SUB_MID_EDGE_LEFT       //Back color to front -- match cenntro de tras --added
      },
      //BL
      {
         //U F
         ROT_Y_L SUB_MID_EDGE_LEFT,         //Back color to up -- match centro da esquerda
         "U' c " SUB_MID_EDGE_RIGHT       //Back color to front -- match cenntro de tras --added
      }
   },
   //UL
   {  
      //FR
      {
         //U L 
         "U a " SUB_MID_EDGE_LEFT,    //Front color to up -- match centro da direita  --added
         SUB_MID_EDGE_RIGHT         //Front color to left -- match centro da frente
      },
      //FL
      {
         //U L
         "U b " SUB_MID_EDGE_RIGHT, //--added
         "U2 " SUB_MID_EDGE_LEFT
      },
      //BR
      {
         //U L
         "U' a " SUB_MID_EDGE_RIGHT, //--added
         ROT_Y_B SUB_MID_EDGE_LEFT   //--added
      },
      //BL
      {
         //U L
         "U' b " SUB_MID_EDGE_LEFT,
         "U2 c " SUB_MID_EDGE_RIGHT
      }
   },
   
   //FR
   {   
      //FR
      {
         //F R
         "",             //trivialmente resolvido
         SUB_MID_EDGE_RIGHT //cor trocada tirar fora
      },
      
      //FL
      {
         //F R
         SUB_MID_EDGE_RIGHT,
         SUB_MID_EDGE_RIGHT
      },
      //BR
      {
         //F R
         SUB_MID_EDGE_RIGHT,
         SUB_MID_EDGE_RIGHT
      },
      //BL
      {
         //F R
         SUB_MID_EDGE_RIGHT,
         SUB_MID_EDGE_RIGHT
      }
   },
   //FL
   {   
      //FR
      {
         //F L
         SUB_MID_EDGE_LEFT,
         SUB_MID_EDGE_LEFT
      },        
      //FL
      {
         //F L
         "",         //solved
         SUB_MID_EDGE_LEFT
      },
      //BR
      {
         //F L
         SUB_MID_EDGE_LEFT,
         SUB_MID_EDGE_LEFT
      },
      //BL
      {
         //F L
         SUB_MID_EDGE_LEFT,
         SUB_MID_EDGE_LEFT
      }
   },
   //BR
   {  
      //FR
      {
         //B R
         ROT_Y_B SUB_MID_EDGE_LEFT,
         ROT_Y_B SUB_MID_EDGE_LEFT
      },
      //FL
      {
         //B R
         ROT_Y_B SUB_MID_EDGE_LEFT,
         ROT_Y_B SUB_MID_EDGE_LEFT
      },
      //BR
      {
         //B R
         "",
         ROT_Y_B SUB_MID_EDGE_LEFT
      },
      //BL
      {
         //U F
         ROT_Y_B SUB_MID_EDGE_LEFT,
         ROT_Y_B SUB_MID_EDGE_LEFT
      }
   },
   //BL
   {  
      //FR
      {
         //B L
         ROT_Y_B SUB_MID_EDGE_RIGHT,
         ROT_Y_B SUB_MID_EDGE_RIGHT
      },
      //FL
      {
         //B L
         ROT_Y_B SUB_MID_EDGE_RIGHT,
         ROT_Y_B SUB_MID_EDGE_RIGHT
      },
      //BR
      {
         //B L
         ROT_Y_B SUB_MID_EDGE_RIGHT,
         ROT_Y_B SUB_MID_EDGE_RIGHT
      },
      //BL
      {
         //B L
         "",
         ROT_Y_B SUB_MID_EDGE_RIGHT
      }
   }
};
