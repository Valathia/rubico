import RPi.GPIO as GPIO
from RPLCD.gpio import CharLCD
import time

GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)

# Backlight (K) on 21
GPIO.setup(21, GPIO.OUT)
GPIO.output(21, GPIO.LOW) # Turn it on

try:
    # Trying the most common data shuffle for this board
    lcd = CharLCD(pin_rs=27, pin_e=23, pins_data=[24, 25, 26, 22],
                  numbering_mode=GPIO.BCM,
                  cols=16, rows=2)
    
    lcd.clear()
    time.sleep(0.1)
    lcd.write_string("ITEAD Fixed!")
    print("Command sent. Look for 'ITEAD Fixed!'")

except Exception as e:
    print(f"Error: {e}")