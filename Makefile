#
# Makefile
#
CC ?= gcc

PC_SIMULATOR ?= y
CFLAGS := -Wall -Wshadow -Wundef

MONITOR_SRC := 

LDFLAGS :=
ifeq ($(PC_SIMULATOR),y)
CFLAGS += -Wmaybe-uninitialized -fbounds-check
LDFLAGS += -lSDL2 -lm
MONITOR_SRC += monitor.c
CFLAGS += -DCONFIG_PC_SIMULATOR
else
CFLAGS += -I/usr/local/_install_arm/libjpeg/include
LDFLAGS += -L/usr/local/_install_arm/libjpeg/lib
MONITOR_SRC += fb_monitor.c
endif
LDFLAGS += -ljpeg

CFLAGS += -O3 -g3 -I./
BIN = jpeg-example
VPATH = 

EXAMPLE_DIR = ${shell pwd}

MAINSRC = main.c \
	  $(MONITOR_SRC)
	  
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

