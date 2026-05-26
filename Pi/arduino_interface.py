import serial
import time

#pi serial config
PORTA_MAC = "/dev/tty.usbmodem101"  #mudar para a porta de usb do raspberry pi
BAUD_RATE = 115200
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

def send_comand(arduino:serial.Serial,cmd:str):
    #Envia o comando convertido em bytes (.encode())
    cmd_prefix = '-'
    comando = f"{cmd_prefix}{cmd}\n"
    arduino.write(comando.encode('utf-8'))
    print(f"Comando enviado: {comando.strip()}")

    # Opcional: Lê a resposta do Arduino
    time.sleep(wait_times[cmd]) 
    # Aguarda o processamento do Arduino
    while arduino.in_waiting > 0:
        resposta = arduino.readline().decode('utf-8').strip()
        print(f"Resposta do Arduino: {resposta}")

