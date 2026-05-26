
from PIL import ImageFont
from PIL import Image
import serial
from picamera import PiCamera
import time
import math

# pode-se usar para mandar texto para o ecrã, substituir pelas nossas definições
# from luma.core.interface.serial import i2c
# from luma.core.render import canvas
# from luma.oled.device import ssd1306
#font = ImageFont.truetype(HOME + 'VCR_OSD_MONO_1.001.ttf',20)

#Falta pegar rodar e tirar a foto

#0 = U 
#1 = R
#2 = F 
#3 = D
#4 = L
#5 = B

#ele lê 1->2->4->5->3->0
#       R->F->L->B->D->U
                                #fico com o vermelho para cima
#       R->y'->F->y'->L->y'->B-> x y' ->D-> y2 ->U
#no fim queremos U em cima e verde na F do robot
#   x2 z'

#pi serial config
PORTA_MAC = "/dev/tty.usbmodem101"  #mudar para a porta de usb do raspberry pi
BAUD_RATE = 115200

HOME = ""

IMG_BREITE = 1080 
IMG_HOEHE = 1080

top_row_pxl = 250  #values for cube detection
mid_row_pxl = 500
bot_row_pxl = 750
lft_col_pxl = 200
mid_col_pxl = 450
rgt_col_pxl = 700

wb_row_pxl = 980 #area for white balance
wb_col_pxl = 890

pxl_locs = [[(lft_col_pxl, top_row_pxl),(mid_col_pxl, top_row_pxl),(rgt_col_pxl, top_row_pxl)],
            [(lft_col_pxl, mid_row_pxl),(mid_col_pxl, mid_row_pxl),(rgt_col_pxl, mid_row_pxl)],
            [(lft_col_pxl, bot_row_pxl),(mid_col_pxl, bot_row_pxl),(rgt_col_pxl, bot_row_pxl)]]



#Camera 

#camera = PiCamera()
#camera.resolution = (IMG_BREITE, IMG_HOEHE)
#camera.exposure_mode = 'auto'
#camera.start_preview()


def pix_average(im, x,y):
    r,g,b = 0,0,0
    for i in range (0,10):
        for j in range (0,10):
            r1,g1,b1 = im.getpixel((x-5+i,y-5+j))
            r += r1
            g += g1
            b += b1
    r = r/100
    g = g/100
    b = b/100

    return r, g, b

def get_sticker():

    col_sticker = []
    for i in range(54):
        col_sticker.append('') 

    #read center piece and set as color of the side

    im = Image.open(HOME + "Cube/face1.jpg")
    im = im.convert('RGB')
    base_R_r, base_R_g, base_R_b = pix_average(im,pxl_locs[1][1][0],pxl_locs[1][1][1])
    wb_r, wb_g, wb_b = pix_average(im,wb_col_pxl,wb_row_pxl)  # manual white ballance correction
    print("%6.2f %6.2f %6.2f   " % (wb_r, wb_g, wb_b) + " R")
    base_R_r = base_R_r / wb_r * 255
    base_R_g = base_R_g / wb_g * 255
    base_R_b = base_R_b / wb_b * 255
    
    im = Image.open(HOME + "Cube/face5.jpg")
    im = im.convert('RGB')
    base_B_r, base_B_g, base_B_b = pix_average(im,pxl_locs[1][1][0],pxl_locs[1][1][1])
    wb_r, wb_g, wb_b = pix_average(im,wb_col_pxl,wb_row_pxl)
    print("%6.2f %6.2f %6.2f   " % (wb_r, wb_g, wb_b) + " B")
    base_B_r = base_B_r / wb_r * 255
    base_B_g = base_B_g / wb_g * 255
    base_B_b = base_B_b / wb_b * 255
    
    im = Image.open(HOME + "Cube/face0.jpg")
    im = im.convert('RGB')
    base_U_r, base_U_g, base_U_b = pix_average(im,pxl_locs[1][1][0],pxl_locs[1][1][1])
    wb_r, wb_g, wb_b = pix_average(im,wb_col_pxl,wb_row_pxl)
    print("%6.2f %6.2f %6.2f   " % (wb_r, wb_g, wb_b) + " U")
    base_U_r = base_U_r / wb_r * 255
    base_U_g = base_U_g / wb_g * 255
    base_U_b = base_U_b / wb_b * 255
    
    im = Image.open(HOME + "Cube/face3.jpg")
    im = im.convert('RGB')
    base_D_r, base_D_g, base_D_b = pix_average(im,pxl_locs[1][1][0],pxl_locs[1][1][1])
    wb_r, wb_g, wb_b = pix_average(im,wb_col_pxl,wb_row_pxl)
    print("%6.2f %6.2f %6.2f   " % (wb_r, wb_g, wb_b) + " D")
    base_D_r = base_D_r / wb_r * 255
    base_D_g = base_D_g / wb_g * 255
    base_D_b = base_D_b / wb_b * 255
    
    im = Image.open(HOME + "Cube/face2.jpg")
    im = im.convert('RGB')
    base_F_r, base_F_g, base_F_b = pix_average(im,pxl_locs[1][1][0],pxl_locs[1][1][1])
    wb_r, wb_g, wb_b = pix_average(im,wb_col_pxl,wb_row_pxl)
    print("%6.2f %6.2f %6.2f   " % (wb_r, wb_g, wb_b) + " F")
    base_F_r = base_F_r / wb_r * 255
    base_F_g = base_F_g / wb_g * 255
    base_F_b = base_F_b / wb_b * 255
    
    im = Image.open(HOME + "Cube/face4.jpg")
    im = im.convert('RGB')
    base_L_r, base_L_g, base_L_b = pix_average(im,pxl_locs[1][1][0],pxl_locs[1][1][1])
    wb_r, wb_g, wb_b = pix_average(im,wb_col_pxl,wb_row_pxl)
    print("%6.2f %6.2f %6.2f   " % (wb_r, wb_g, wb_b) + " L")
    base_L_r = base_L_r / wb_r * 255
    base_L_g = base_L_g / wb_g * 255
    base_L_b = base_L_b / wb_b * 255    
    
    
    masterstring = ""
    for img_iter in range(0, 6):
        img_path = HOME + "Cube/face" + str(img_iter) + ".jpg"
        im = Image.open(img_path)
        im = im.convert('RGB')
        wb_r, wb_g, wb_b = pix_average(im,wb_col_pxl,wb_row_pxl)
        for x_iter in range(0,3): #iterate over all nine color locations on each face
            for y_iter in range(0,3):
                r, g, b = pix_average(im, pxl_locs[y_iter][x_iter][0],pxl_locs[y_iter][x_iter][1]) #get pixel value
                #find euclidian distances
                r = r / wb_r * 255
                g = g / wb_g * 255
                b = b / wb_b * 255

                dist_R = math.pow(r - base_R_r, 2) + math.pow(g - base_R_g, 2) + math.pow(b - base_R_b, 2)
                dist_B = math.pow(r - base_B_r, 2) + math.pow(g - base_B_g, 2) + math.pow(b - base_B_b, 2)
                dist_U = math.pow(r - base_U_r, 2) + math.pow(g - base_U_g, 2) + math.pow(b - base_U_b, 2)
                dist_D = math.pow(r - base_D_r, 2) + math.pow(g - base_D_g, 2) + math.pow(b - base_D_b, 2)
                dist_F = math.pow(r - base_F_r, 2) + math.pow(g - base_F_g, 2) + math.pow(b - base_F_b, 2)
                dist_L = math.pow(r - base_L_r, 2) + math.pow(g - base_L_g, 2) + math.pow(b - base_L_b, 2)
                dist_min = min(dist_R, dist_B, dist_U, dist_D, dist_F, dist_L) #find minimum distance value

                #figure out which color has that minimum value

                if(dist_min == dist_R):
                    color = 'R'
                elif(dist_min == dist_B):
                    color = 'B'
                elif(dist_min == dist_U):
                    color = 'U'
                elif(dist_min == dist_D):
                    color = 'D'
                elif(dist_min == dist_F):
                    color = 'F'
                else:
                    color = 'L'
                print("%6.2f %6.2f %6.2f %6.2f %6.2f %6.2f  " % (dist_R,dist_B,dist_U,dist_D,dist_F,dist_L) + color)
                #set cubie face as that color

                col_sticker[img_iter * 9 + 3*y_iter + x_iter] = color
        
#Korrektur oben      //sticker are not in correct order due to movements at reading the cube
    dummy_1 = col_sticker[0]   
    dummy_2 = col_sticker[1]
    col_sticker[0] = col_sticker[6]
    col_sticker[1] = col_sticker[3]
    col_sticker[6] = col_sticker[8]
    col_sticker[3] = col_sticker[7]
    col_sticker[8] = col_sticker[2]
    col_sticker[7] = col_sticker[5]
    col_sticker[2] = dummy_1
    col_sticker[5] = dummy_2  
 
# Korrektur unten
    dummy_1 = col_sticker[27]   
    dummy_2 = col_sticker[28]
    col_sticker[27] = col_sticker[33]
    col_sticker[28] = col_sticker[30]
    col_sticker[33] = col_sticker[35]
    col_sticker[30] = col_sticker[34]
    col_sticker[35] = col_sticker[29]
    col_sticker[34] = col_sticker[32]
    col_sticker[29] = dummy_1
    col_sticker[32] = dummy_2    

    print (masterstring)    
    for i in range (54):
        masterstring = masterstring + col_sticker[i]

    print(masterstring)
    return masterstring


def send_comand(arduino,cmd):
    # 4. Envia o comando convertido em bytes (.encode())
    cmd_prefix = '-'
    comando = f"{cmd_prefix}{cmd}\n"
    arduino.write(comando.encode('utf-8'))
    print(f"Comando enviado: {comando.strip()}")

    # 5. Opcional: Lê a resposta do Arduino (com os seus logs ANSI do VS Code)
    time.sleep(5) # Aguarda o processamento do Arduino
    while arduino.in_waiting > 0:
        resposta = arduino.readline().decode('utf-8').strip()
        print(f"Resposta do Arduino: {resposta}")


def get_cube():

    try:
        # 1. Abre a porta serial
        print(f"A abrir ligação com o Arduino em {PORTA_MAC}...")
        arduino = serial.Serial(PORTA_MAC, BAUD_RATE, timeout=1)

        # 2. CRUCIAL: O Arduino Uno reinicia sempre que a porta serial é aberta.
        # Precisamos de esperar 2 segundos para o bootloader terminar antes de enviar dados.
        time.sleep(7)
        print("Ligação estabelecida! Arduino pronto.")

        send_comand(arduino,"g")
        camera.capture(HOME + 'Cube/face1.jpg')

        send_comand(arduino,"y'")
        camera.capture(HOME + 'Cube/face2.jpg')    

        send_comand(arduino,"y'")
        camera.capture(HOME + 'Cube/face4.jpg')

        send_comand(arduino,"y'")
        camera.capture(HOME + 'Cube/face5.jpg')

        send_comand(arduino,"x")
        send_comand(arduino,"y'")
        camera.capture(HOME + 'Cube/face3.jpg')

        send_comand(arduino,"y2")
        camera.capture(HOME + 'Cube/face0.jpg')


        #deixa o cubo com a frente no verde e o up no branco
        send_comand(arduino,"x2")
        send_comand(arduino,"z'")
        # 6. Fecha a porta de forma limpa
        arduino.close()
        print("Ligação fechada.")

    except serial.SerialException as e:
        print(f"Erro ao aceder à porta serial: {e}")


def scan_cube(): 
    #global message
    # with canvas(device) as draw:
    #     draw.text((5, 0), "Scanning..." , fill="white")
    
    get_cube() 
    
    # with canvas(device) as draw:
    #     draw.text((5, 0), "Analysing..." , fill="white")
    
    cube_string = get_sticker()

    print(cube_string + "\n")
    return cube_string


#para estar a ligação serial
def test_serial():

    try:
        # 1. Abre a porta serial
        print(f"A abrir ligação com o Arduino em {PORTA_MAC}...")
        arduino = serial.Serial(PORTA_MAC, BAUD_RATE, timeout=1)
        
        # 2. CRUCIAL: O Arduino Uno reinicia sempre que a porta serial é aberta.
        # Precisamos de esperar 2 segundos para o bootloader terminar antes de enviar dados.
        time.sleep(7)
        print("Ligação estabelecida! Arduino pronto.")

        # 3. Formato do comando: "-cmd"
        cmd_prefix = '-'
        cmd = "y"
        comando = f"{cmd_prefix}{cmd}\n"

        # 4. Envia o comando convertido em bytes (.encode())
        arduino.write(comando.encode('utf-8'))
        print(f"Comando enviado: {comando.strip()}")

        # 5. Opcional: Lê a resposta do Arduino (com os seus logs ANSI do VS Code)
        time.sleep(5) # Aguarda o processamento do Arduino
        while arduino.in_waiting > 0:
            resposta = arduino.readline().decode('utf-8').strip()
            print(f"Resposta do Arduino: {resposta}")

        # 6. Fecha a porta de forma limpa
        arduino.close()
        print("Ligação fechada.")

    except serial.SerialException as e:
        print(f"Erro ao aceder à porta serial: {e}")

# if __name__ == "__main__":
#     sys.exit(test_serial())