MAKNAM = Makefile
CC ?= gcc
TARGET := rubik_solver
ARCH := $(shell uname -m)

CFLAGS_BASE= -std=c11 -Wall -Wextra -Wpedantic

ifeq ($(ARCH), arm64)
    # Configuração para o Mac M1 (Apple Silicon) - 64-bit nativo
	ARCH_CFLAGS = -O3 -mcpu=apple-m1
	CACHE_LINE_SIZE := 64
else ifeq ($(ARCH), aarch64) 
    # Configuração para a Raspberry Pi 3 B+ para OS 64-bits -> se não correr direito ou compilar, deixar só -O2
    ARCH_CFLAGS = -O3 -march=armv8-a+crc -mcpu=cortex-a53 -mtune=cortex-a53 -ftree-vectorize -flto
	CACHE_LINE_SIZE := 64
else 
    # Configuração genérica de fallback (ex: se compilar num PC Intel/AMD normal)
    ARCH_CFLAGS = -O3
	CACHE_LINE_SIZE := $(shell cat /sys/devices/system/cpu/cpu0/cache/index0/coherency_line_size 2>/dev/null || echo 64)
endif

CFLAGS_BASE += -DCACHE_LINE_SIZE=$(CACHE_LINE_SIZE)

ifdef TEST
    CFLAGS_TEST = -DTEST
endif 

ifdef RUBICO
	CFLAGS_SOLVER = -DRUBICO
	SOLVER = solver_rubico.c
	SOLVER_H = solver_rubico.h
else 
	SOLVER = solver.c
	SOLVER_H = solver.h
endif


ifdef DEBUGALL
	CFLAGS_DEBUG = -fsanitize=address -g -DDEBUGALL
endif

CFLAGS_CUSTOM := $(CFLAGS_TEST) $(CFLAGS_SOLVER) $(CFLAGS_DEBUG) 

CFLAGS := $(CFLAGS_BASE) $(ARCH_CFLAGS) $(CFLAGS_CUSTOM)

SRC := 	main.c\
		$(SOLVER)\
		algorithms.c\
		cube.c\
		solution.c\
		config.c\

OBJFLS := $(SRC:.c=.o)

# *  Define the rules.                                                       *

$(TARGET): $(OBJFLS)
	$(CC) $(CFLAGS) $(OBJFLS) -o $@

#o comando $< captura o .c como input e o $@ define o .o como output
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

all:
	make -f $(MAKNAM) clean
	make -f $(MAKNAM) $(TARGET)

clean:
	-rm $(TARGET)
	-rm $(OBJFLS)

# Sem esta configuração isto não vai compilar
main.o: cube.h $(SOLVER_H) solution.h config.h
solver.o: solver.h cube.h algorithms.h solution.h config.h
solver_rubico.o: $(SOLVER_H) cube.h algorithms.h solution.h config.h
cube.o: cube.h solution.h
solution.o: solution.h config.h 
algorithms.o: algorithms.h
config.o: config.h