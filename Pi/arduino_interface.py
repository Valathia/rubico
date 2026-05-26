import serial
import time

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
        self.conn = self.init_serial()

    def init_serial(self):
        try:
            print(f"A abrir ligação com o Arduino em {self.p}...")
            arduino = serial.Serial(self.p, self.rate, timeout=1)

            # O Arduino reinicia sempre que a porta serial é aberta.
            # É preciso esperar pelo boot.
            time.sleep(7)
            print("Ligação estabelecida! Arduino pronto.")
            return arduino
        
        except serial.SerialException as e:
            print(f"Erro ao aceder à porta serial: {e}")
            return None

    def close_serial(self):
        if self.conn != None:
            self.conn.close()
            print("Ligação fechada.")
            self.conn = None

    def send_comand(self,cmd:str):
        #Envia o comando convertido em bytes (.encode())
        cmd_prefix = '-'
        comando = f"{cmd_prefix}{cmd}\n"
        if self.conn != None:
            self.conn.write(comando.encode('utf-8'))
            print(f"Comando enviado: {comando.strip()}")

            # Opcional: Lê a resposta do Arduino
            time.sleep(wait_times[cmd]) 
            # Aguarda o processamento do Arduino
            while self.conn.in_waiting > 0:
                resposta = self.conn.readline().decode('utf-8').strip()
                print(f"Resposta do Arduino: {resposta}")





