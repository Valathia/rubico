import computer_vision as cv
import arduino_interface
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


def compile_rubico():
    subprocess.run(MAKE_RUBICO)

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
    arduino = arduino_interface.arduino_connection(PORTA,BAUD_RATE)
    
    if(arduino.conn!=None):
        cube_string:str = cv.scan_cube(arduino)

        # with canvas(device) as draw:
        #     draw.text((5, 0), "Running Solver..." , fill="white")
        solution = run_solver(cube_string)
    else:
        print("verificar porta de conexão do Arduino\n")


    arduino.close_serial()

if __name__ == "__main__":
    sys.exit(main())