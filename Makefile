#
# Makefile
#
CC = gcc
CFLAGS = -Wall -Wshadow -Wundef -Wmaybe-uninitialized -fbounds-check

CFLAGS += -O3 -g3 -I./
LDFLAGS += -lSDL2 -lm
BIN = demo
VPATH = 

EXAMPLE_DIR = ${shell pwd}

MAINSRC = main.c \
	  monitor.c


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

