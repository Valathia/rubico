from PIL import Image, ImageDraw
import os
import math

# 1. Define coordinates to test for Face 1
top_row_pxl = 130   
mid_row_pxl = 390   
bot_row_pxl = 760   

lft_col_pxl = 120   
mid_col_pxl = 410   
rgt_col_pxl = 720   

rows = [top_row_pxl, mid_row_pxl, bot_row_pxl]
cols = [lft_col_pxl, mid_col_pxl, rgt_col_pxl]

# White Balance (WB) sample point coordinates
wb_col_pxl = 230
wb_row_pxl = 230

# --- Setup hardcoded Base Colors for Euclidean Math ---
# Note: In your final master script, these profiles are generated dynamically 
# from the center pieces of each face file.
base_R_r, base_R_g, base_R_b = (200, 10, 10)    # Red
base_B_r, base_B_g, base_B_b = (10, 10, 180)    # Blue
base_U_r, base_U_g, base_U_b = (230, 230, 230)  # White
base_D_r, base_D_g, base_D_b = (200, 200, 10)   # Yellow
base_F_r, base_F_g, base_F_b = (10, 180, 10)    # Green
base_L_r, base_L_g, base_L_b = (220, 100, 10)   # Orange (Left face default)

def pix_average(im, x, y):
    r, g, b = 0, 0, 0
    # Sampling a 10x10 block centered at the coordinate point
    for i in range(0, 10):
        for j in range(0, 10):
            r1, g1, b1 = im.getpixel((int(x) - 5 + i, int(y) - 5 + j))
            r += r1
            g += g1
            b += b1
    return r / 100, g / 100, b / 100

# File system setup
script_dir = os.path.dirname(os.path.abspath(__file__))
img_path = os.path.join(script_dir, "face1.jpg")

print("Targeting file at:", img_path)

try:
    im = Image.open(img_path).convert('RGB')
    draw = ImageDraw.Draw(im)
    
    # 2. Extract White Balance profile from target coordinate
    wb_r, wb_g, wb_b = pix_average(im, wb_col_pxl, wb_row_pxl)
    # Avoid DivisionByZero if sampling a pitch-black pixel
    wb_r = 1.0 if wb_r == 0 else wb_r
    wb_g = 1.0 if wb_g == 0 else wb_g
    wb_b = 1.0 if wb_b == 0 else wb_b
    
    print(f"\n--- Color Recognition Results ---")
    
    # 3. Iterate through the grid arrays to draw lines and sample colors
    # Replicates your matrix tracking sequence
    for y_iter in range(0, 3):       # Rows
        row_string = []
        for x_iter in range(0, 3):   # Columns
            target_x = cols[x_iter]
            target_y = rows[y_iter]
            
            # Extract raw color and normalize against WB channel values
            r, g, b = pix_average(im, target_x, target_y)
            r = (r / wb_r) * 255
            g = (g / wb_g) * 255
            b = (b / wb_b) * 255

            # Calculate Euclidean distances to find the closest match
            dist_R = math.pow(r - base_R_r, 2) + math.pow(g - base_R_g, 2) + math.pow(b - base_R_b, 2)
            dist_B = math.pow(r - base_B_r, 2) + math.pow(g - base_B_g, 2) + math.pow(b - base_B_b, 2)
            dist_U = math.pow(r - base_U_r, 2) + math.pow(g - base_U_g, 2) + math.pow(b - base_U_b, 2)
            dist_D = math.pow(r - base_D_r, 2) + math.pow(g - base_D_g, 2) + math.pow(b - base_D_b, 2)
            dist_F = math.pow(r - base_F_r, 2) + math.pow(g - base_F_g, 2) + math.pow(b - base_F_b, 2)
            dist_L = math.pow(r - base_L_r, 2) + math.pow(g - base_L_g, 2) + math.pow(b - base_L_b, 2)
            
            dist_min = min(dist_R, dist_B, dist_U, dist_D, dist_F, dist_L)

            if dist_min == dist_R:
                color = 'R'
            elif dist_min == dist_B:
                color = 'B'
            elif dist_min == dist_U:
                color = 'U'
            elif dist_min == dist_D:
                color = 'D'
            elif dist_min == dist_F:
                color = 'F'
            else:
                color = 'L'
                
            row_string.append(color)
            
        print(f"Row {y_iter} Colors: {row_string}")
        
    print(f"---------------------------------\n")

    # 4. Generate the visualization lines over the image file
    for col in cols:
        draw.line(((col, 0), (col, im.height)), fill="red", width=5)
    for row in rows:
        draw.line(((0, row), (im.width, row)), fill="red", width=5)
        
    # Draw blue targeting rectangle for White Balance reference point
    draw.rectangle(((wb_col_pxl - 12, wb_row_pxl - 12), (wb_col_pxl + 12, wb_row_pxl + 12)), outline="blue", width=4)

    # 5. Output image preview compilation
    preview_path = os.path.join(script_dir, "grid_preview.jpg")
    im.save(preview_path)
    print(f"Preview successfully generated at: {preview_path}")

except FileNotFoundError:
    print("Could not load 'face1.jpg'. Check file directory contents:")
    print(os.listdir(script_dir))