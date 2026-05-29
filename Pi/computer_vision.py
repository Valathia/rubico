from arduino_interface import arduino_connection as con
from PIL import ImageFont
from PIL import Image
from picamera2 import Picamera2
import time
import math
import colorsys

# pode-se usar para mandar texto para o ecrã, substituir pelas nossas definições
# from luma.core.interface.serial import i2c
# from luma.core.render import canvas
# from luma.oled.device import ssd1306
#font = ImageFont.truetype(HOME + 'VCR_OSD_MONO_1.001.ttf',20)

#definição usada pelo computer vision. a numeração no nosso bit-board não é esta
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

HOME = ""

IMG_BREITE = 1080 
IMG_HOEHE = 1080


# Use these exact coordinates for Face 1 in your dictionary
top_row_pxl = 130   # Pushed down from 100 to clear the plastic borders
mid_row_pxl = 390   # Kept the same (already centered well)
bot_row_pxl = 760   # Lifted slightly from 680 to hit the true sticker centers

lft_col_pxl = 120   # Squeezed right from 130 to get deeper into the stickers
mid_col_pxl = 410   # Kept the same (perfect center)
rgt_col_pxl = 720   # Squeezed left from 690 to stay clear of the right edge


wb_col_pxl = 230
wb_row_pxl = 230

pxl_locs = [[(lft_col_pxl, top_row_pxl),(mid_col_pxl, top_row_pxl),(rgt_col_pxl, top_row_pxl)],
            [(lft_col_pxl, mid_row_pxl),(mid_col_pxl, mid_row_pxl),(rgt_col_pxl, mid_row_pxl)],
            [(lft_col_pxl, bot_row_pxl),(mid_col_pxl, bot_row_pxl),(rgt_col_pxl, bot_row_pxl)]]



#Camera 

camera = Picamera2()
config = camera.create_still_configuration()
config["main"]["size"] = (IMG_BREITE, IMG_HOEHE)
camera.configure(config)
# In your camera initialization block:
camera.set_controls({"AeEnable": True, "AwbEnable": True})
time.sleep(2) # Give the camera time to adjust to the room lighting
camera.set_controls({"AwbEnable": False}) # Lock it so it stays consistent across faces

camera.start_preview()
camera.start()

time.sleep(2)

# camera.resolution = (IMG_BREITE, IMG_HOEHE)
# camera.exposure_mode = 'auto'
# camera.start_preview()


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
    col_sticker = [''] * 54 

    # Helper function to get normalized HSV from an image location
    def get_hsv_at_loc(image, x, y, wb_factors):
        r, g, b = pix_average(image, x, y)
        
        # Normalize to 0.0 - 1.0
        r, g, b = r / 255.0, g / 255.0, b / 255.0
        return colorsys.rgb_to_hsv(r, g, b)

    # 1. Gather White Balance factors and Base Hues for centers
    base_hues = {}
    faces = {1: 'R', 5: 'B', 0: 'U', 3: 'D', 2: 'F', 4: 'L'}
    
    for face_num, face_letter in faces.items():
        im = Image.open(HOME + f"Cube/face{face_num}.jpg").convert('RGB') # NO CHANNEL SWAP
        wb_rgb = pix_average(im, wb_col_pxl, wb_row_pxl)
        
        # Get HSV of the center sticker
        h, s, v = get_hsv_at_loc(im, pxl_locs[1][1][0], pxl_locs[1][1][1], wb_rgb)
        base_hues[face_letter] = h

    # 2. Analyze all stickers using Hue distance
    # 2. Analyze all stickers using Hue distance
    masterstring = ""
    faces_names = {0: 'U (Top)', 1: 'R (Right)', 2: 'F (Front)', 3: 'D (Bottom)', 4: 'L (Left)', 5: 'B (Back)'}

    for img_iter in range(0, 6):
        im = Image.open(HOME + f"Cube/face{img_iter}.jpg").convert('RGB')
        wb_rgb = pix_average(im, wb_col_pxl, wb_row_pxl)
        
        # Create a temporary 3x3 grid just for debugging this face
        face_grid = [[' ' for _ in range(3)] for _ in range(3)]
        
        for x_iter in range(0, 3): 
            for y_iter in range(0, 3):
                h, s, v = get_hsv_at_loc(im, pxl_locs[y_iter][x_iter][0], pxl_locs[y_iter][x_iter][1], wb_rgb)
                
                # Convert Hue to 0-360 Degrees
                hue_degrees = h * 360.0

                # --- THE WHITE VS YELLOW FINE-TUNING GATE ---
                
                # 1. Strict White Check: Low saturation always wins
                if s < 0.12:  
                    detected_color = 'U'  # White
                    
                # 2. Strict Yellow Check: High saturation and right Hue range
                elif 40 <= hue_degrees < 85 and s >= 0.50:
                    detected_color = 'D'  # Yellow
                    
                # 3. Ambient Glare / Edge Cases
                elif s < 0.25 and v > 0.85:
                    # If it has slightly higher saturation but is extremely bright, 
                    # it's usually a white sticker reflecting ambient warm light.
                    detected_color = 'U'  # White
                    
                # 4. Process all other standard colors
                else:
                    if hue_degrees < 15 or hue_degrees > 345:
                        if v > 0.82:
                            detected_color = 'L'  # Orange
                        else:
                            detected_color = 'R'  # Red
                            
                    elif 15 <= hue_degrees < 40:
                        detected_color = 'L'  # Orange
                        
                    elif 40 <= hue_degrees < 85:
                        # Catch-all for lower saturation yellow
                        detected_color = 'D'  # Yellow
                        
                    elif 85 <= hue_degrees < 165:
                        detected_color = 'F'  # Green
                        
                    elif 165 <= hue_degrees < 260:
                        detected_color = 'B'  # Blue
                        
                    else:
                        detected_color = 'R'  # Fallback
                        
                # Save to the main array
                col_sticker[img_iter * 9 + 3*y_iter + x_iter] = detected_color
                # Save to our debug grid (row = y_iter, col = x_iter)
                face_grid[y_iter][x_iter] = detected_color

        # --- DEBUG PRINT FOR THE CURRENT FACE ---
        print(f"\n--- DEBUG: Face {img_iter} ({faces_names[img_iter]}) ---")
        for row in face_grid:
            print(f"  [ {row[0]}   {row[1]}   {row[2]} ]")
        print("-" * 34)
        
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

def get_cube(arduino:con):

    arduino.send_comand("g")
    camera.capture_file(HOME + 'Cube/face1.jpg')

    arduino.send_comand("y'")
    camera.capture_file(HOME + 'Cube/face2.jpg')    

    arduino.send_comand("y'")
    camera.capture_file(HOME + 'Cube/face4.jpg')

    arduino.send_comand("y'")
    camera.capture_file(HOME + 'Cube/face5.jpg')

    arduino.send_comand("x")
    arduino.send_comand("y'")
    camera.capture_file(HOME + 'Cube/face3.jpg')

    arduino.send_comand("y2")
    camera.capture_file(HOME + 'Cube/face0.jpg')


    #deixa o cubo com a frente no verde e o up no branco
    arduino.send_comand("x2")
    arduino.send_comand("z'")

def scan_cube(arduino:con): 
    #global message
    # with canvas(device) as draw:
    #     draw.text((5, 0), "Scanning..." , fill="white")
    
    get_cube(arduino) 
    
    # with canvas(device) as draw:
    #     draw.text((5, 0), "Analysing..." , fill="white")
    
    cube_string = get_sticker()

    print(cube_string + "\n")
    return cube_string