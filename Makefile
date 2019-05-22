#
# Makefile
#
CC = gcc
CFLAGS = -Wall -Wshadow -Wundef -Wmaybe-uninitialized -fbounds-check

CFLAGS += -O3 -g3 -I./
# CFLAGS += -I../libjpeg/include/
LDFLAGS :=
LDFLAGS += -lSDL2 -lm
# LDFLAGS += -L../libjpeg/lib/ -ljpeg
LDFLAGS += -ljpeg
BIN = demo
VPATH = 

EXAMPLE_DIR = ${shell pwd}

MAINSRC = main.c \
	  monitor.c
	  
MAINSRC += if_libjpeg.c
MAINSRC += scaler.c
MAINSRC += file.c

OBJEXT ?= .o

AOBJS = $(ASRCS:.S=$(OBJEXT))
COBJS = $(CSRCS:.c=$(OBJEXT))

MAINOBJ = $(MAINSRC:.c=$(OBJEXT))

SRCS = $(ASRCS) $(CSRCS) $(MAINSRC)
OBJS = $(AOBJS) $(COBJS)

## MAINOBJ -> OBJFILES

all: clean default

%.o: %.c
	@$(CC)  $(CFLAGS) -c $< -o $@
	@echo "CC $<"
    
default: $(AOBJS) $(COBJS) $(MAINOBJ)
	$(CC) -o $(BIN) $(MAINOBJ) $(AOBJS) $(COBJS) $(LDFLAGS)

clean: 
	rm -f $(BIN) $(AOBJS) $(COBJS) $(MAINOBJ)

