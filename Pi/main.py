import sys
import subprocess
import time
import threading 
import RPi.GPIO as GPIO
from RPLCD.gpio import CharLCD
import serial.tools.list_ports
from flask import Flask, render_template_string, jsonify, redirect, url_for

import computer_vision as cv
from arduino_interface import arduino_connection as ard_con

# --- Configurações do Flask ---
app = Flask(__name__)

# Variáveis globais partilhadas entre as threads
state = None  
arduino = None
status_sistema = "A inicializar hardware..." # "scanning", "ready", "erro"

# --- Configurações GPIO / LCD ---
GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)
GPIO.setup(21, GPIO.OUT)
GPIO.output(21, GPIO.HIGH)

lcd = CharLCD(pin_rs=22, pin_e=23, pins_data=[24, 25, 26, 27],
              numbering_mode=GPIO.BCM, cols=16, rows=2)

SOLVER = "./Solver/rubik_solver"
BAUD_RATE = 115200

back_moves = {"L": "L'", "L'": "L", "L2": "L2", "D": "D'", "D'": "D", "D2": "D2", "U": "U'", "U'": "U", "U2": "U2", "R": "R'", "R'": "R", "R2": "R2", "B": "B'", "B'": "B", "B2": "B2", "F": "F'", "F'": "F", "F2": "F2", "y": "y'", "y'": "y", "y2": "y2", "x": "x'", "x'": "x", "x2": "x2", "z": "z'", "z'": "z", "z2": "z2"}

# Lock para garantir que a rotação manual via Web e a thread não usem a serial ao mesmo tempo
serial_lock = threading.Lock()

class solve_state:
    def __init__(self, arduino_con: ard_con, cube_string: str, solution: str) -> None:
        self.conn = arduino_con
        self.cs = cube_string
        self.sol = solution.split(" ") if solution else []
        self.cur_pos = 0
        self.sol_len = len(self.sol)
    
    def forward(self):
        with serial_lock:
            if self.cur_pos < self.sol_len:
                self.conn.send_comand(self.sol[self.cur_pos])
                self.cur_pos += 1
                return f"Executado: {self.sol[self.cur_pos-1]}"
            return "Todos os passos já foram executados"
    
    def backwards(self):
        with serial_lock:
            if self.cur_pos > 0: 
                self.cur_pos -= 1
                self.conn.send_comand(back_moves[self.sol[self.cur_pos]])
                return f"Desfeito: {self.sol[self.cur_pos]}"
            return "Já se encontra na posição inicial"
    
    def fast_solve(self):
        while self.cur_pos < self.sol_len:
            self.forward()
        return "Cubo resolvido!"
    
    def fast_unsolve(self):
        while self.cur_pos > 0:
            self.backwards()
        return "Retornado ao estado inicial!"

def find_arduino_port():
    ports = serial.tools.list_ports.comports()
    for port in ports:
        if "Arduino" in port.description or "CH340" in port.description or "ACM" in port.device or "USB" in port.device:
            return port.device
    return None

def run_solver(cube_string):
    result = subprocess.run([SOLVER, cube_string], capture_output=True, text=True)
    if result.returncode != 0:
        return None
    return result.stdout.strip()

# --- THREAD DE BACKGROUND (Rotina do Robô) ---
def robo_background_worker():
    global state, arduino, status_sistema
    
    lcd.cursor_mode = 'hide'
    lcd.clear()
    lcd.cursor_pos = (0, 0)
    lcd.write_string("Hello Rubico!")
    lcd.cursor_pos = (1, 0)
    lcd.write_string("Arduino...")

    PORTA = find_arduino_port()
    if not PORTA:
        print("Erro: Arduino não encontrado!")
        lcd.clear()
        lcd.cursor_pos = (0, 0)
        lcd.write_string("Erro Rubico!")
        lcd.cursor_pos = (1, 0)
        lcd.write_string("Arduino...")
        status_sistema = "Erro: Arduino não encontrado"
        return

    arduino = ard_con(PORTA, BAUD_RATE)
    
    if arduino.conn is not None:
        status_sistema = "A posicionar cubo..."
        lcd.clear()
        lcd.cursor_pos = (0, 0)
        lcd.write_string("Hello Rubico!")
        lcd.cursor_pos = (1, 0)
        lcd.write_string("Place cube!")
        status_sistema = "Erro: Arduino não encontrado"
        
        with serial_lock:
            arduino.send_comand("g'")
            time.sleep(5)
            arduino.send_comand("g")

        status_sistema = "Scan cubo (CV)..."
        lcd.clear()
        lcd.cursor_pos = (0, 0)
        lcd.write_string("Hello Rubico!")
        lcd.cursor_pos = (1, 0)
        lcd.write_string("Scan cube...")
        
        cube_string = cv.scan_cube(arduino)
        
        status_sistema = "A calcular solucao..."
        lcd.clear()
        lcd.cursor_pos = (0, 0)
        lcd.write_string("Hello Rubico!")
        lcd.cursor_pos = (1, 0)
        lcd.write_string("Solve...")
        solution = run_solver(cube_string)
        
        if not solution:
            status_sistema = "Erro ao resolver cubo"
            lcd.clear()
            lcd.write_string("Erro Rubico!")
            lcd.cursor_pos = (1, 0)
            lcd.write_string("Solve fail!")
            return

        state = solve_state(arduino, cube_string, solution)
        status_sistema = "Pronto"
        
        lcd.clear()
        lcd.write_string("Rubico Online!\nWeb Server Ready")
    else:
        status_sistema = "Erro na conexao Serial"

# --- ROTAS FLASK ---

HTML_TEMPLATE = """
<!DOCTYPE html>
<html>
<head>
    <title>Rubico Controller</title>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta http-equiv="refresh" content="3"> <style>
        body { font-family: Arial, sans-serif; text-align: center; background: #222; color: white; padding-top: 50px; }
        .btn { display: inline-block; padding: 15px 25px; margin: 10px; font-size: 18px; cursor: pointer; border: none; border-radius: 5px; color: white; text-decoration: none;}
        .blue { background-color: #008CBA; } .orange { background-color: #f4511e; } .green { background-color: #4CAF50; } .gray { background-color: #555555; }
        .status { margin-top: 20px; font-size: 20px; color: #ffeb3b; }
        .badge { background: #555; padding: 5px 10px; border-radius: 4px; font-size: 14px; }
    </style>
</head>
<body>
    <h1>Rubico - Painel de Controlo</h1>
    <p>Status do Sistema: <span class="badge">{{ status }}</span></p>
    
    {% if status == "Pronto" and state_obj %}
        <p>Passo Atual: <strong>{{ current }} / {{ total }}</strong></p>
        <div>
            <a href="/action/back" class="btn orange">◀ Voltar Passo</a>
            <a href="/action/next" class="btn blue">Avançar Passo ▶</a>
        </div>
        <div>
            <a href="/action/all" class="btn green">Resolver Tudo</a>
            <a href="/action/reset" class="btn gray">Reset Tudo</a>
        </div>
        <div class="status">
            <p>Próximo movimento: {{ next_move }}</p>
        </div>
    {% else %}
        <div class="status" style="color: #ffa500;">
            <p>Por favor, aguarde o fim do scan do cubo...</p>
        </div>
    {% endif %}
    <br><br>
    <a href="/moves" style="color: #4CAF50;">Ver JSON de movimentos (/moves)</a>
</body>
</html>
"""

@app.route('/')
def index():
    next_move = "Nenhum"
    current = 0
    total = 0
    if state:
        next_move = state.sol[state.cur_pos] if state.cur_pos < state.sol_len else "Fim!"
        current = state.cur_pos
        total = state.sol_len

    return render_template_string(
        HTML_TEMPLATE, 
        status=status_sistema,
        state_obj=state,
        current=current, 
        total=total, 
        next_move=next_move
    )

@app.route('/moves')
def moves():
    """Retorna os movimentos. Se ainda estiver a escanear, avisa o cliente no JSON"""
    if status_sistema != "Pronto" or not state:
        return jsonify({
            "status": status_sistema,
            "solution": [],
            "total_moves": 0,
            "current_position": 0,
            "remaining_moves": []
        })
        
    return jsonify({
        "status": status_sistema,
        "solution": state.sol,
        "total_moves": state.sol_len,
        "current_position": state.cur_pos,
        "remaining_moves": state.sol[state.cur_pos:]
    })

@app.route('/action/<type>')
def action(type):
    if not state:
        return redirect(url_for('index'))
    
    if type == "next":
        state.forward()
    elif type == "back":
        state.backwards()
    elif type == "all":
        state.fast_solve()
        lcd.clear()
        lcd.write_string("Done via Web!")
    elif type == "reset":
        state.fast_unsolve()
        
    return redirect(url_for('index'))

if __name__ == "__main__":
    # 1. Cria e inicia a Thread do robot
    robo_thread = threading.Thread(target=robo_background_worker)
    robo_thread.daemon = True # Morre se o Flask fechar
    robo_thread.start()

    # 2. Inicia o Flask imediatamente na thread principal
    app.run(host='0.0.0.0', port=5000, debug=False, threaded=True)