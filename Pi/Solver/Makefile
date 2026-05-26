MAKNAM = Makefile
CC ?= gcc
TARGET := rubik_solver
ARCH := $(shell uname -m)
CACHE_LINE_SIZE := $(shell cat /sys/devices/system/cpu/cpu0/cache/index0/coherency_line_size 2>/dev/null || echo 64)

CFLAGS_BASE= -std=c11 -Wall -Wextra -Wpedantic

ifeq ($(ARCH), armv6l)
    # Raspberry Pi B+ (ARM1176JZF-S, ARMv6 hard-float)
    ARCH_CFLAGS = -O3 -march=armv6zk -mcpu=arm1176jzf-s -mtune=arm1176jzf-s -mfpu=vfp -mfloat-abi=hard
    CACHE_LINE_SIZE := $(shell getconf LEVEL1_DCACHE_LINESIZE 2>/dev/null || echo 32)
else ifeq ($(ARCH), arm64)
    # Configuração para o Mac M1 (Apple Silicon) - 64-bit nativo
    # O Clang/GCC no M1 já otimiza para a arquitetura nativa com -O3
	ARCH_CFLAGS = -O2 -mcpu=apple-m1
	CACHE_LINE_SIZE := $(shell sysctl -n hw.cachelinesize 2>/dev/null || echo 128)
else ifeq ($(findstring armv7,$(ARCH)), armv7)
    # Configuração para a Raspberry Pi 3 B+ (em modo 32-bit padrão do Raspberry Pi OS)
    # Ativa instruções vetorizadas NEON e otimiza para o chip Cortex-A53     ARCH_CFLAGS = -O3 -march=armv7-a -mfpu=neon-vfpv4 -mfloat-abi=hard
    ARCH_CFLAGS = -O3 -march=armv8-a+crc -mcpu=cortex-a53 -mtune=cortex-a53 -mfpu=neon-fp-armv8 -mfloat-abi=hard
    CACHE_LINE_SIZE := $(shell getconf LEVEL1_DCACHE_LINESIZE 2>/dev/null || echo 64)
else ifeq ($(ARCH), aarch64) 
    # Configuração para a Raspberry Pi 3 B+ se estiver a usar um OS de 64-bit nativo (Ubuntu/Debian 64) ARCH_CFLAGS = -O3 -march=armv8-a
    ARCH_CFLAGS = -O3 -march=armv8-a+crc -mcpu=cortex-a53 -mtune=cortex-a53
	CACHE_LINE_SIZE := $(shell getconf LEVEL1_DCACHE_LINESIZE 2>/dev/null || echo 64)
else 
    # Configuração genérica de fallback (ex: se compilar num PC Intel/AMD normal)
    ARCH_CFLAGS = -O2
endif

CFLAGS_BASE += -DCACHE_LINE_SIZE=$(CACHE_LINE_SIZE)

#make TEST=1
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
#solver.h 
main.o: cube.h $(SOLVER_H) solution.h config.h
solver.o: solver.h cube.h algorithms.h solution.h config.h
solver_rubico.o: $(SOLVER_H) cube.h algorithms.h solution.h config.h
cube.o: cube.h solution.h
solution.o: solution.h config.h 
algorithms.o: algorithms.h
config.o: config.h