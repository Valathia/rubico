#include <Servo.h>
#include <stdint.h>

Servo L_GRIP;   // garra esquerda //base:30 | ungrip:90 | grip:15 
Servo L_ROT;    // rodar esquerda // está flat nos 90
Servo R_GRIP;   // garra direita  //base:30 | ungrip:90 |grip:15
Servo R_ROT;    // rodar direita  // fica flat nos 86 ?

uint8_t pinos[] = {5, 6, 10, 11};
Servo* servos[] = {&L_GRIP, &L_ROT, &R_GRIP, &R_ROT};

// constexpr uint8_t L_GRIP = 5;
// constexpr uint8_t L_ROT = 6;
// constexpr uint8_t R_GRIP = 10;
// constexpr uint8_t R_ROT = 11;

constexpr uint8_t NUM_SERVOS = 4;
constexpr uint8_t POS_INICIAL = 90;
constexpr uint8_t MOVE_TIME = 300;
// constexpr uint8_t GRIP      = 15;
// constexpr uint8_t UNGRIP    = 90;
// constexpr uint8_t IDLEGRIP  = 30;

// constexpr uint8_t L         = 180;
// constexpr uint8_t L_P       = 0;
// constexpr uint8_t D         = 180;
// constexpr uint8_t D_P       = 0;

constexpr char COR_RESET[]   = "\033[0m";  // Volta à cor padrão
constexpr char COR_VERMELHO[] = "\033[31m"; // Texto Vermelho
constexpr char COR_VERDE[]    = "\033[32m"; // Texto Verde
constexpr char COR_AMARELO[]  = "\033[33m"; // Texto Amarelo

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
    Serial.println(pinos[i]);

    servos[i]->write(POS_INICIAL + 5);
    delay(300);
    servos[i]->write(POS_INICIAL - 5);
    delay(300);
    servos[i]->write(POS_INICIAL);
    delay(500); 
  }


  servos[0]->write(25);
  servos[2]->write(30);
  Serial.print(COR_VERDE);
  Serial.print("Pronto! ");
  Serial.print(COR_RESET);
  Serial.println("Formato esperado: <pino>,<graus>");
}

void loop() {
  if (Serial.available() > 0) {
    String comando = Serial.readStringUntil('\n');
    comando.trim(); 

    // Filtrar o "lixo" da UART
    // Se a string for muito curta (menor que "5,0") ignora silenciosamente
    if (comando.length() < 3) {
      return; 
    }

    int indiceVirgula = comando.indexOf(',');
    
    #ifdef TEST
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
      if (indiceVirgula > 0) {
        String move = comando.substring(indiceVirgula + 1);

        switch (move[0]) {
          case 'y':
            rot_y_r();
            break;
          default:
            break;
        }
      }
    #endif

  }
}

void grip(Servo &s) {
  if(&s==&L_GRIP | &s==&R_GRIP) {
    s.write(15);
    delay(MOVE_TIME);
  }
}

void ungrip(Servo &s){
  if(&s==&L_GRIP | &s==&R_GRIP) {
    s.write(90);
    delay(MOVE_TIME);
  }
}

void idlegrip(Servo &s) {
  if(&s==&L_GRIP | &s==&R_GRIP) {
    s.write(30);
    delay(MOVE_TIME);
  }
}

void move_l() {
  L_ROT.write(180);
  delay(MOVE_TIME);
}

void move_lp(){
  L_ROT.write(0);
  delay(MOVE_TIME);
}

void move_d() {
  R_ROT.write(180);
  delay(MOVE_TIME);
}

void move_dp() {
  R_ROT.write(0);
  delay(MOVE_TIME);
}

void rot_y_r() {
  ungrip(L_GRIP);
  move_dp();
  grip(L_GRIP);
  ungrip(R_GRIP);
  move_d();
  grip(R_GRIP);
}