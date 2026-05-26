#define UBRRH 1
#include <Arduino.h>
#include <Servo.h>
#include <stdint.h>
//#define TEST 1
Servo L_GRIP;   // garra esquerda //base:30 | ungrip:90 | grip:15 
Servo L_ROT;    // rodar esquerda // está flat nos 90
Servo R_GRIP;   // garra direita  //base:30 | ungrip:90 |grip:15
Servo R_ROT;    // rodar direita  // fica flat nos 86 ?

//posso encolher o código com um método get_grip
//que ao dar o endereço de memória do L_ROT ou R_ROT devolve L_GRIP ou R_GRIP
//posso unificar y e yp tendo em conta a posição de inicio da rotação
//voltar a ver os moves do z,f,u,r,b para não fazer moves desnecessários
uint8_t pinos[] = {5, 6, 10, 11};
Servo* servos[] = {&L_GRIP, &L_ROT, &R_GRIP, &R_ROT};

constexpr uint8_t NUM_SERVOS = 4;
constexpr uint8_t POS_INICIAL = 90;
constexpr uint8_t ROT_MAX = 180;
constexpr uint8_t ROT_MIN = 0;
constexpr uint8_t ROT_MIN_ROLL = 0;

constexpr uint8_t GRIP_MAX = 15;
constexpr uint8_t GRIP_MIN = 90;

constexpr uint8_t GRIP_SOLO = 20;
constexpr uint8_t GRIP_SOLO_LEFT = 20;

constexpr uint8_t GRIP_LEFT_IDLE = 30;
constexpr uint8_t GRIP_RIGHT_IDLE = 35;
constexpr uint8_t GRIP_MOVE_LEFT = 20;
constexpr uint8_t GRIP_MOVE_RIGHT = 25;

constexpr uint16_t MOVE_TIME = 500;
constexpr uint16_t GRIP_TIME = 300;

//não funciona no terminal do arduino, mas funciona no do vscode e outros
constexpr char COR_RESET[]   = "\033[0m";  // Volta à cor padrão
constexpr char COR_VERMELHO[] = "\033[31m"; // Texto Vermelho
constexpr char COR_VERDE[]    = "\033[32m"; // Texto Verde
constexpr char COR_AMARELO[]  = "\033[93m"; // Texto Amarelo
constexpr char COR_LARANJA[]  = "\033[38;5;208m"; // Texto Laranja 

const char* cores[4] = {COR_VERDE,COR_AMARELO,COR_LARANJA,COR_VERMELHO};

void setup() {
  Serial.begin(115200);
  Serial.println("A iniciar servos...");

  for (int i = 0; i < NUM_SERVOS; i++) {
    // Expandir os limites do PWM para os MG996
    // Padrão era attach(pino)
    servos[i]->attach(pinos[i], 500, 2500); 
    servos[i]->write(POS_INICIAL);
  }
  
  delay(1000);

  Serial.println("A iniciar sequencia de identificacao...");
  for (int i = 0; i < NUM_SERVOS; i++) {
    Serial.print("A testar servo: ");
    Serial.print(cores[i]);
    Serial.print(pinos[i]);
    Serial.println(COR_RESET);

    servos[i]->write(POS_INICIAL + 5);
    delay(300);
    servos[i]->write(POS_INICIAL - 5);
    delay(300);
    servos[i]->write(POS_INICIAL);
    delay(500); 
  }

  servos[0]->write(GRIP_LEFT_IDLE);
  servos[2]->write(GRIP_RIGHT_IDLE);
  Serial.print(COR_VERDE);
  Serial.print("Pronto! ");
  Serial.print(COR_RESET);
  #ifdef TEST
    Serial.println("Formato esperado: <pino>,<graus>");
  #else 
    Serial.println("Formato esperado: -<move>  Ex: -L -L' -L2 ");
  #endif
}

void loop() {
  if (Serial.available() > 0) {
    String comando = Serial.readStringUntil('\n');
    comando.trim(); 

    #ifdef TEST
      // Filtrar o "lixo" da UART
      // Se a string for muito curta (menor que "5,0") ignora silenciosamente

      if (comando.length() < 3) {
        return; 
      }
      int indiceVirgula = comando.indexOf(',');

      // Só processa se existir uma vírgula
      if (indiceVirgula > 0) {
        int pinoAlvo = comando.substring(0, indiceVirgula).toInt();
        int anguloAlvo = comando.substring(indiceVirgula + 1).toInt();

        bool pinoEncontrado = false;
        for (int i = 0; i < NUM_SERVOS; i++) {
          if (pinos[i] == pinoAlvo) {
            anguloAlvo = constrain(anguloAlvo, 0, 180); 
            if((i==0 || i==2) && (anguloAlvo>90)){
              anguloAlvo=90;
              Serial.print("Aviso: Angulo de trabalho foi reduzido para 90.");
            }
            else if((i==0 || i==2) && (anguloAlvo<15)){
              anguloAlvo=15;
              Serial.print("Aviso: Angulo de trabalho foi reduzido para 15.");
            }
            servos[i]->write(anguloAlvo);
            
            Serial.print("OK: Pino ");
            Serial.print(pinoAlvo);
            Serial.print(" -> ");
            Serial.print(anguloAlvo);
            Serial.println(" graus.");
            
            pinoEncontrado = true;
            break; 
          }
        }
        
        // Se não encontrou o pino mas o formato estava correto
        if (!pinoEncontrado) {
          Serial.print("Aviso: Pino ignorado (");
          Serial.print(pinoAlvo);
          Serial.println(")");
        }
      } 
    #else
      if (comando.length() < 2) {
        return; 
      }
      int indx = comando.indexOf('-');

      if (indx == 0) {
        String move = comando.substring(indx + 1);

        switch (move[0]) {
          case 'D':
            switch(move[1]){
              case '\'':
                move_dp();
                break;
              case '2':
                move_d2();
                break;
              default:
                move_d();
                break;            
            }
            break;
          case 'L':
            switch(move[1]){
              case '\'':
                move_lp();
                break;
              case '2':
                move_l2();
                break;
              default:
                move_l();
                break;            
            }
            break;
          case 'F':
            rot_y();
            switch(move[1]){
              case '\'':
                move_lp();
                break;
              case '2':
                move_l2();
                break;
              default:
                move_l();
                break;            
            }
            rot_yp();
            break;
          case 'B':
            rot_yp();
            switch(move[1]){
              case '\'':
                move_lp();
                break;
              case '2':
                move_l2();
                break;
              default:
                move_l();
                break;            
            }
            rot_y();
            break;
          case 'R':
            rot_y2();
            switch(move[1]){
              case '\'':
                move_lp();
                break;
              case '2':
                move_l2();
                break;
              default:
                move_l();
                break;            
            }
            rot_y2();
            break;
          case 'U':
            rot_xp();
            rot_y();
            switch(move[1]){
              case '\'':
                move_lp();
                break;
              case '2':
                move_l2();
                break;
              default:
                move_l();
                break;            
            }
            rot_yp();
            rot_x();
            break;
          case 'y':
            switch(move[1]) {
              case '\'':
                rot_yp();
                break;
              case '2':
                rot_y2();
                break;
              default:
                rot_y();
                break;
            }
            break;
          case 'x':
            switch(move[1]) {
              case '\'':
                rot_xp();
                break;
              case '2':
                rot_x2();
                break;
              default:
                rot_x();
                break;
            }
            break;
          case 'z':
            switch(move[1]) {
              case '\'':
                rot_zp();
                break;
              case '2':
                rot_z2();
                break;
              default:
                rot_z();
                break;
            }
            break;
          case 'g':
            switch(move[1]) {
              case '\'':
                iddle_grip_cube();
                break;
              default:
                grip_cube();
                break;
            }
            break;
          default:
            break;
        }
        
      }
    #endif

  }
}


void print_aviso(){
    Serial.print(COR_AMARELO);
    Serial.print("[Aviso]: ");
    Serial.print(COR_RESET);
}

void grip(Servo &g,uint8_t grip_strength) {
  if(grip_strength<GRIP_MAX) {  //se menor que o maximo assumir que queria agarrar
    print_aviso();
    Serial.print("Angulo de trabalho foi aumentado para ");
    Serial.print(COR_AMARELO);
    Serial.print(GRIP_SOLO);
    Serial.println(COR_RESET);

    g.write(GRIP_SOLO);
  }
  else if(grip_strength>GRIP_MIN){  //se maior que o minimo assumir que queria abrir
    print_aviso();
    Serial.print("Angulo de trabalho foi reduzido para ");
    Serial.print(COR_AMARELO);
    Serial.print(GRIP_MIN);
    Serial.println(COR_RESET);
    g.write(GRIP_MIN);
  }
  else {
    g.write(grip_strength);
  }
  delay(MOVE_TIME);

}

void ungrip(Servo &g){
  /*  Antes de fazer ungrip a uma garra, certificar que a outra
      está a segurar o cubo. Este ungrip é para usar com os movimentos
      E não deve ser usado para largar o cubo.
  */
  if(&g==&L_GRIP) {
    if(R_GRIP.read()!=GRIP_SOLO) {
      //usar a função grip para passar nas guardas
      grip(R_GRIP,GRIP_SOLO);
    }
    Serial.print("Ungrip Garra Esquerda: ");
    Serial.print(COR_VERDE);
    Serial.print("Ok");
    Serial.println(COR_RESET);
  }
  else if(&g==&R_GRIP) {
    if(L_GRIP.read()!=GRIP_SOLO_LEFT) {
      grip(L_GRIP,GRIP_SOLO_LEFT);
    }

    Serial.print("Ungrip Garra Direita: ");
    Serial.print(COR_VERDE);
    Serial.print("Ok");
    Serial.println(COR_RESET);
  }
  else {
    print_aviso();
    Serial.println("Ungrip Ignorado - Servo escolhido não corresponde a um servo de 'grip' ");
    return;
  }

  g.write(POS_INICIAL);
  delay(MOVE_TIME);
}

void grip_cube(){
  grip(R_GRIP,GRIP_MOVE_LEFT);
  grip(L_GRIP,GRIP_MOVE_RIGHT);
}

void iddle_grip_cube(){
  grip(L_GRIP,GRIP_LEFT_IDLE);
  grip(R_GRIP,GRIP_RIGHT_IDLE);
}

void regrip(){
  if((R_ROT.read()==POS_INICIAL) & (L_ROT.read()==POS_INICIAL)) {
    iddle_grip_cube();
    grip_cube();
  }
}

/*  Wrapper around write for rotating servos
    checks rot_angle to see if it's inside range before turning
    We don't have to check the minimums due to uint8_t restricting
    to a minimum of 0
*/
void write_rotation(Servo &rot, uint8_t rot_angle) {
  if(&rot==&L_ROT | &rot==&R_ROT) {  
    if(rot_angle>ROT_MAX) {
      print_aviso();
      Serial.print("Angulo de trabalho foi reduzido para ");
      Serial.print(COR_AMARELO);
      Serial.print(ROT_MAX);
      Serial.println(COR_RESET);
      
      rot.write(ROT_MAX);
    }
    else{ 
      rot.write(rot_angle);
    }
    delay(MOVE_TIME);
    return;
  }
  print_aviso();
  Serial.println("Rotação Ignorada - Servo escolhido não corresponde a um servo de rotação ");
}

/* reposition sem grip final*/
void reposition(Servo &g, Servo &rot,uint8_t grip_strength,uint8_t rot_angle,boolean ismove){
  ungrip(g);
  write_rotation(rot,rot_angle);
  if(ismove) {
    grip(g,grip_strength);
  }
}

boolean pre_move(Servo &s,boolean ismove){
  if(&s==&R_ROT) {
    if(ismove) {
        if(R_GRIP.read()!=GRIP_MOVE_RIGHT){
          grip(R_GRIP,GRIP_MOVE_RIGHT);
        }
    }
    else {
        if(R_GRIP.read()!=GRIP_SOLO){
          grip(R_GRIP,GRIP_SOLO);
        }
    }

    if(L_ROT.read()!=POS_INICIAL){
      Serial.println("Pre-move: Repositioning Left Grip...");
      reposition(L_GRIP,L_ROT,GRIP_SOLO_LEFT,POS_INICIAL,ismove);
      return true;
    }
    return false;
  }
  else if(&s==&L_ROT){
    if(ismove) {
      if(L_GRIP.read()!=GRIP_MOVE_LEFT){
        grip(L_GRIP,GRIP_MOVE_LEFT);
      }
    }
    else {
      if(L_GRIP.read()!=GRIP_SOLO_LEFT){
        grip(L_GRIP,GRIP_SOLO_LEFT);
      }
    }

    if(R_ROT.read()!=POS_INICIAL){
      Serial.println("Pre-move: Repositioning Right Grip...");
      reposition(R_GRIP,R_ROT,GRIP_SOLO,POS_INICIAL,ismove);
      return true;
    }
    return false;
  }

  return false;
}

void move_l() {
  pre_move(L_ROT,true);
  uint16_t pos = (uint16_t)L_ROT.read();
  Serial.print("Current Pos L_ROT: ");
  Serial.print(pos);
  Serial.print(" to ");
  pos += POS_INICIAL;
  Serial.println(pos);

  if(pos <= ROT_MAX) {
    write_rotation(L_ROT,pos);
  }
  else {  //reposition
    Serial.println("Repositioning...");
    /*  ao reposicionar para 0, ao mover, move para 90 
        e já fica direito para o próximo move
    */ 
    reposition(L_GRIP,L_ROT,GRIP_MOVE_LEFT,ROT_MIN,true);
    move_l();
    return;
  }
  regrip();
}

void move_lp(){
  pre_move(L_ROT,true);
  int16_t pos = L_ROT.read();
  Serial.print("Current Pos L_ROT: ");
  Serial.print(pos);
  Serial.print(" to ");
  pos -= POS_INICIAL;
  Serial.println(pos);
  
  if(pos >= ROT_MIN) {
    write_rotation(L_ROT,pos);
  }
  else {  //reposition
    Serial.println("Repositioning...");
    reposition(L_GRIP,L_ROT,GRIP_MOVE_LEFT,ROT_MAX,true);
    move_lp();
    return;
  }
  regrip();
}

void move_l2(){
  pre_move(L_ROT,true);
  uint8_t cur_pos = L_ROT.read();
  if(cur_pos==ROT_MIN) {
    write_rotation(L_ROT,ROT_MAX);
  }
  else if(cur_pos==ROT_MAX){
    write_rotation(L_ROT,ROT_MIN);
  }
  else{
    ungrip(L_GRIP);
    
    write_rotation(L_ROT,ROT_MIN);
    
    grip(L_GRIP,GRIP_MOVE_LEFT);
    
    write_rotation(L_ROT,ROT_MAX);
  }
  regrip();
}

void move_d() {
  pre_move(R_ROT,true);

  uint16_t pos = R_ROT.read();
  Serial.print("Current Pos R_ROT: ");
  Serial.print(pos);
  Serial.print(" to ");
  pos += POS_INICIAL;
  Serial.println(pos);
  
  if(pos <= ROT_MAX) {
    write_rotation(R_ROT,pos);
  }
  else {  //reposition
    Serial.println("Repositioning...");
    reposition(R_GRIP,R_ROT,GRIP_MOVE_RIGHT,ROT_MIN,true);
    move_d();
  }
  regrip();
}

void move_dp() {
  pre_move(R_ROT,true);
  int16_t pos = R_ROT.read();

  Serial.print("Current Pos R_ROT: ");
  Serial.print(pos);
  Serial.print(" to ");
  pos -= POS_INICIAL;
  Serial.println(pos);
  
  if(pos>= ROT_MIN) {
    write_rotation(R_ROT,pos);    
  }
  else {  //reposition
    Serial.println("Repositioning...");
    reposition(R_GRIP,R_ROT,GRIP_MOVE_RIGHT,ROT_MAX,true);
    move_dp();
  }
  regrip();
}

void move_d2(){
  pre_move(R_ROT,true);
  uint8_t cur_pos = R_ROT.read();

  if(cur_pos==ROT_MIN) {
    write_rotation(R_ROT,ROT_MAX);
  }
  else if(cur_pos==ROT_MAX){
    write_rotation(R_ROT,ROT_MIN);
  }
  else{
    ungrip(R_GRIP);
    
    write_rotation(R_ROT,ROT_MIN);

    grip(R_GRIP,GRIP_MOVE_RIGHT);
    
    write_rotation(R_ROT,ROT_MAX);
  }
  regrip();
}


//right to front
void rot_y() {
  /*
    se for 180, é só largar o esquerdo e mover
    se for se for 0 é meter a 180, largar o esquerdo e mover
    se for 90 pre_move no L 1º porque não sabe onde ele está
    no caso de ser 180 ou 0 sabemos que a outra garra está a 90º
  */
  uint8_t r_pos =  R_ROT.read();
  if(r_pos==POS_INICIAL) {
    pre_move(R_ROT,true);
  }

  if(r_pos!=ROT_MAX) {
    Serial.println("Repositioning Right...");
    reposition(R_GRIP,R_ROT,GRIP_SOLO,ROT_MAX,true);  //right is gripped at the end
  }

  Serial.println("Ungrip Left");
  ungrip(L_GRIP);

  Serial.println("Rotating Right: Right face to Front");
  write_rotation(R_ROT,POS_INICIAL);
  
  Serial.println("Grip Left");
  grip(L_GRIP,GRIP_MOVE_LEFT);
  regrip();
}

//left to front
void rot_yp() {
  /*
    se for 0, é só largar o esquerdo e mover
    se for se for 180 é meter a 0, largar o esquerdo e mover
    se for 90 pre_move no L 1º porque não sabe onde ele está
    no caso de ser 180 ou 0 sabemos que a outra garra está a 90º
  */
  uint8_t r_pos =  R_ROT.read();
  if(r_pos==POS_INICIAL) {
    pre_move(R_ROT,true);
  }

  if(r_pos!=ROT_MIN) {
    Serial.println("Repositioning Right...");
    reposition(R_GRIP,R_ROT,GRIP_SOLO,ROT_MIN,true);  //right is gripped at the end
  }

  Serial.println("Ungrip Left");
  ungrip(L_GRIP);

  Serial.println("Rotating Right: Right face to Front");
  write_rotation(R_ROT,POS_INICIAL);
  
  Serial.println("Grip Left");
  grip(L_GRIP,GRIP_MOVE_LEFT);
  regrip();
}

void rot_y2(){
  /*
    if 0 ou 180 é só rodar para o outro oposto
    se 90, pre_move, e escolher um dos extremos
  */
  uint8_t r_pos =  R_ROT.read();
  if(r_pos==POS_INICIAL) {
    pre_move(R_ROT,true);
    Serial.println("Repositioning Right...");
    reposition(R_GRIP,R_ROT,GRIP_SOLO,ROT_MAX,true);  //right is gripped at the end
    r_pos = ROT_MAX;
  }

  Serial.println("Ungrip Left");
  ungrip(L_GRIP);

  if(r_pos==ROT_MAX) {
    Serial.println("Rotating Right: Back face to Front");
    write_rotation(R_ROT,ROT_MIN);
  }

  if(r_pos==ROT_MIN) {
    Serial.println("Rotating Right: Back face to Front");
    write_rotation(R_ROT,ROT_MAX);
  }

  Serial.println("Grip Left");
  grip(L_GRIP,GRIP_SOLO_LEFT);

  Serial.println("Reposition Right...");
  reposition(R_GRIP,R_ROT,GRIP_MOVE_RIGHT,POS_INICIAL,true);
  regrip();
}

//front to up
void rot_x(){
  /*
    se for 180, é só largar o esquerdo e mover
    se for se for 0 é meter a 180, largar o esquerdo e mover
    se for 90 pre_move no L 1º porque não sabe onde ele está
    no caso de ser 180 ou 0 sabemos que a outra garra está a 90º
  */
  uint8_t l_pos = L_ROT.read();
  if(l_pos==POS_INICIAL) {
    pre_move(L_ROT,true);
  }

  if(l_pos!=ROT_MAX) {
    Serial.println("Repositioning Left...");
    reposition(L_GRIP,L_ROT,GRIP_SOLO_LEFT,ROT_MAX,true);  //right is gripped at the end
  }

  Serial.println("Ungrip Right");
  ungrip(R_GRIP);

  Serial.println("Rotating Left: Front face to Up");
  write_rotation(L_ROT,POS_INICIAL);
  
  Serial.println("Grip Right");
  grip(R_GRIP,GRIP_MOVE_RIGHT);
  regrip();
}

//back to up
void rot_xp(){
    /*
    se for 180, é só largar o esquerdo e mover
    se for se for 0 é meter a 180, largar o esquerdo e mover
    se for 90 pre_move no L 1º porque não sabe onde ele está
    no caso de ser 180 ou 0 sabemos que a outra garra está a 90º
  */
  uint8_t l_pos = L_ROT.read();
  if(l_pos==POS_INICIAL) {
    pre_move(L_ROT,true);
  }

  if(l_pos!=ROT_MIN_ROLL) {
    Serial.println("Repositioning Left...");
    reposition(L_GRIP,L_ROT,GRIP_SOLO_LEFT,ROT_MIN_ROLL,true);  //right is gripped at the end
  }

  Serial.println("Ungrip Right");
  ungrip(R_GRIP);

  Serial.println("Rotating Left: Front face to Up");
  write_rotation(L_ROT,POS_INICIAL);
  
  Serial.println("Grip Right");
  grip(R_GRIP,GRIP_MOVE_RIGHT);
  regrip();
}

//down to up
void rot_x2(){
  /*
    if 0 ou 180 é só rodar para o outro oposto
    se 90, pre_move, e escolher um dos extremos
  */
  uint8_t l_pos =  L_ROT.read();
  if(l_pos==POS_INICIAL) {
    pre_move(L_ROT,true);
    Serial.println("Repositioning Left...");
    reposition(L_GRIP,L_ROT,GRIP_SOLO_LEFT,ROT_MAX,true);  //right is gripped at the end
    l_pos = ROT_MAX;
  }

  Serial.println("Ungrip Right");
  ungrip(R_GRIP);

  if(l_pos==ROT_MAX) {
    Serial.println("Rotating Left: Down face to Up");
    write_rotation(L_ROT,ROT_MIN_ROLL);
  }

  if(l_pos==ROT_MIN | l_pos==ROT_MIN_ROLL) {
    Serial.println("Rotating Left: Down face to Up");
    write_rotation(L_ROT,ROT_MAX);
  }

  Serial.println("Grip Right");
  grip(R_GRIP,GRIP_SOLO);

  Serial.println("Reposition Left...");
  reposition(L_GRIP,L_ROT,GRIP_MOVE_LEFT,POS_INICIAL,true);
  regrip();
}

//left to up
void rot_z(){
  rot_xp();
  rot_yp();
  rot_x();
}

//right to up
void rot_zp(){
  rot_xp();
  rot_y();
  rot_x();
}

//down to up 
void rot_z2(){
  rot_x2();
  regrip();
  rot_y2();
}