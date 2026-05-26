import serial
import time


COR_RESET    = "\033[0m"           #Volta à cor padrão
COR_VERMELHO = "\033[31m"          #Texto Vermelho
COR_VERDE    = "\033[32m"          #Texto Verde
COR_AMARELO  = "\033[93m"          #Texto Amarelo
COR_LARANJA  = "\033[38;5;208m"    #Texto Laranja 
COR_CIANO    = "\033[36m"

#wait times por excesso: um L  costuma demorar no máximo 2-3s
#                        um L2 costuma demorar no máximo 7-8s
wait_times = { 
    "L": 5,
    "L'": 5,
    "L2": 10,
    "D": 5,
    "D'": 5,
    "D2": 10,
    "U": 30,     #rot_xp + rot_y +  <move L>  + rot_yp + rot_x
    "U'": 30,
    "U2": 35,
    "R": 25,     #rot_y2 + <move L> + rot_y2
    "R'": 25,
    "R2": 30,
    "B": 18,    #rot_yp + <move L> + rot_y
    "B'": 18,
    "B2": 21,
    "F": 18,    #rot_y + <move L> + rot_yp
    "F'": 18,
    "F2": 21,
    "y": 7,
    "y'": 7,
    "y2": 10,
    "x": 7,
    "x'": 7,
    "x2": 10,
    "z": 25,      #rot_xp + rot_yp + rot_x
    "z'": 25,     #rot_xp + rot_y + rot_x
    "z2": 30,     #rot_x2 + rot_y2
    "g": 3,
    "g'": 3 
}

class arduino_connection:
    def __init__(self,porta,baud_rate) -> None:
        self.p = porta
        self.rate = baud_rate
        self.conn = self._init_serial()

    def _init_serial(self):
        try:
            print(f"A abrir ligação com o Arduino em {COR_CIANO} {self.p} {COR_RESET}...")
            arduino = serial.Serial(self.p, self.rate, timeout=1)

            # O Arduino reinicia sempre que a porta serial é aberta.
            # É preciso esperar pelo boot.
            time.sleep(7)
            print(f"{COR_VERDE}Ligação estabelecida!{COR_RESET} Arduino pronto.")
            return arduino
        
        except serial.SerialException as e:
            print(f"{COR_VERMELHO}Erro{COR_RESET} ao aceder à porta serial: {COR_CIANO}{e}{COR_RESET}")
            return None

    def close_serial(self):
        if self.conn != None:
            self.conn.close()
            print("Ligação encerrada.")
            self.conn = None
        else:
            print("Ligação previamente encerrada. ")

    def open_serial(self):
        self.conn = self._init_serial()

    def send_comand(self,cmd:str):
        #Envia o comando convertido em bytes (.encode())
        if self.conn != None:
            cmd_prefix = '-'
            comando = f"{cmd_prefix}{cmd}\n"
            self.conn.write(comando.encode('utf-8'))
            print(f"Comando enviado: {comando.strip()} \n")

            # Opcional: Lê a resposta do Arduino
            time.sleep(wait_times[cmd]) 
            # Aguarda o processamento do Arduino
            while self.conn.in_waiting > 0:
                resposta = self.conn.readline().decode('utf-8').strip()
                print(f"Resposta do Arduino: {resposta}")
        else:
            print(f"Conexão previamente encerrada. Estabeleça uma nova conexão com o método {COR_CIANO} open_serial() {COR_RESET} \n")





