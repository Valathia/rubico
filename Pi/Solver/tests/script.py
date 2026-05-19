#!/usr/bin/env python3

import re
import sys
import time
import subprocess

from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.common.keys import Keys

from selenium.webdriver.firefox.service import Service
from webdriver_manager.firefox import GeckoDriverManager


#SOLVER = sys.argv[1] if len(sys.argv) > 1 else "./rubik_solver"
SOLVER = "../rubik_solver"
#ITERATIONS = int(sys.argv[2]) if len(sys.argv) > 2 else 1

ITERATIONS = int(sys.argv[1]) if len(sys.argv)> 1 else 1

fails = 0
successes = 0

def get_scramble(driver):
    
    driver.get("https://scramble.cubing.net/")

    time.sleep(3)

    scramble = driver.execute_script(""" return [...document.querySelectorAll('twisty-alg-leaf-elem')].map(e =>
        e.shadowRoot.lastChild.lastChild.lastChild.data.toString()).join(' '); """)
    
    #print(f"\nScramble: {scramble}")

    with open("test_results.txt", "a", encoding="utf-8") as f:
            f.write(f"\nScramble: {scramble}")

    
    return scramble

def run_solver(scramble):

    result = subprocess.run(
        [SOLVER, scramble],
        capture_output=True,
        text=True
    )
    
    if result.returncode != 0:
        global fails
        fails = fails + 1

        print("\nFail: ")
        print(result.stderr.strip())

        with open("test_results.txt", "a", encoding="utf-8") as f:
            f.write(f"\nFail:")
            f.write(result.stderr.strip())
    else:
        global successes
        successes = successes + 1

        print("\nSuccess!")
        #print(result.stdout.strip())

        with open("test_results.txt", "a", encoding="utf-8") as f:
            f.write(f"\nSolution: ")
            f.write(result.stdout.strip())
            f.write(f"\nSuccess!")

def main():

    options = webdriver.FirefoxOptions()

    # Uncomment for headless mode:
    # options.add_argument("-headless")

    driver = webdriver.Firefox(
        service=Service(
            GeckoDriverManager().install()
        ),
        options=options
    )

    time.sleep(3)
    with open("test_results.txt", "w", encoding="utf-8") as f:
        f.write(f"\n------------------- Tests {ITERATIONS} --------------------")

    for i in range(ITERATIONS):

        with open("test_results.txt", "a", encoding="utf-8") as f:
            f.write(f"\n=== Test {i+1}/{ITERATIONS} ===")

        print(f"\n=== Test {i+1}/{ITERATIONS} ===")
        scramble = get_scramble(driver)

        run_solver(scramble)


    print(f"\nPassed:{successes}/{ITERATIONS} Failed:{fails}/{ITERATIONS} ")
    with open("test_results.txt", "a", encoding="utf-8") as f:
        f.write(f"\n----------------------------------------------------------")
        f.write(f"\nRESULTS")
        f.write(f"\n\tPassed:{successes}/{ITERATIONS} | Failed:{fails}/{ITERATIONS}")    
    
    driver.quit()


if __name__ == "__main__":
    main()