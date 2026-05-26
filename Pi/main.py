import computer_vision as cv
from arduino_interface import arduino_connection as ard_con
import sys
import subprocess

# pode-se usar para mandar texto para o ecrã, substituir pelas nossas definições
# from luma.core.interface.serial import i2c
# from luma.core.render import canvas
# from luma.oled.device import ssd1306

SOLVER = "./Solver/rubik_solver"
MAKE_RUBICO = "./Solver/make_rubico.sh"

#pi serial config
PORTA = "/dev/tty.usbmodem101"  #mudar para a porta de usb do raspberry pi
BAUD_RATE = 115200

back_moves = {
    "L":    "L'",
    "L'":   "L" ,
    "L2":   "L2",
    "D":    "D'",
    "D'":   "D" ,
    "D2":   "D2",
    "U":    "U'",    
    "U'":   "U" ,
    "U2":   "U2",
    "R":    "R'",  
    "R'":   "R" ,
    "R2":   "R2",
    "B":    "B'",   
    "B'":   "B" ,
    "B2":   "B2",
    "F":    "F'",   
    "F'":   "F" ,
    "F2":   "F2",
    "y":    "y'",
    "y'":   "y" ,
    "y2":   "y2",
    "x":    "x'",
    "x'":   "x" ,
    "x2":   "x2",
    "z":    "z'",      
    "z'":   "z" ,     
    "z2":   "z2",     
}

class solve_state:
    def __init__(self,arduino_con:ard_con,cube_string:str,solution:str) -> None:
        self.conn = arduino_con
        self.cs = cube_string
        self.sol = solution.split(" ")
        self.cur_pos = 0
        self.sol_len = len(self.sol)
    
    def forward(self):
        if self.cur_pos<self.sol_len:
            self.conn.send_comand(self.sol[self.cur_pos])
            self.cur_pos += 1
        else:
            print("Todos os passos já foram executados\n")
    
    def backwards(self):
        if(self.cur_pos>0): 
            #undo last move
            self.cur_pos-=1
            self.conn.send_comand(back_moves[self.sol[self.cur_pos]])
        else:
            print("Já se encontra na posição inicial\n")
    
    def fast_solve(self):
        while(self.cur_pos<self.sol_len):
            self.forward()
    
    def fast_unsolve(self):
        while(self.cur_pos>0):
            self.backwards()

    def forward_nsteps(self,steps):
        end_pos = self.cur_pos+steps
        while(self.cur_pos<self.sol_len and self.cur_pos<end_pos):
            self.forward()

    def backwards_nsteps(self,steps):
        end_pos = self.cur_pos-steps 
        while(self.cur_pos>0 and self.cur_pos>end_pos):
            self.backwards

def compile_rubico():
    subprocess.run([MAKE_RUBICO],capture_output=True,
        text=True)

def run_solver(cube_string):

    result = subprocess.run(
        [SOLVER, cube_string],
        capture_output=True,
        text=True
    )
    
    if result.returncode != 0:
        
        print("\nFail: ")
        print(result.stderr.strip())
        sys.exit(result.returncode)

    else:
        solution = result.stdout.strip()
        print("Solution: \n")
        print(solution)
        print("\n")
        return solution 

def main():
    compile_rubico()
    arduino = ard_con(PORTA,BAUD_RATE)
    
    if(arduino.conn!=None):
        cube_string:str = cv.scan_cube(arduino)

        # with canvas(device) as draw:
        #     draw.text((5, 0), "Running Solver..." , fill="white")
        solution = run_solver(cube_string)
        
        #cria estado da aplicação
        state = solve_state(arduino,cube_string,solution)

        #algures por aqui isto tem que ter um loopzito para receber instruções da app

        arduino.close_serial()
    else:
        print("verificar porta de conexão do Arduino\n")


if __name__ == "__main__":
    sys.exit(main())