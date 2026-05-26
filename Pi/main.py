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
    cube_string:str = cv.scan_cube()

    # with canvas(device) as draw:
    #     draw.text((5, 0), "Running Solver..." , fill="white")
    
    solution = run_solver(cube_string)


if __name__ == "__main__":
    sys.exit(main())