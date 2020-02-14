CC=gcc
CFLAGS = -Wall
DEBUG = y

ifeq ($(DEBUG), y)
	CFLAGS += -g
else
	CFLAGS += -O2
endif

PWD=$(shell pwd)
SUBDIRS := src/module src/common  src/common/ini 


LDFLAGS = -L$(PWD)/lib -lkeros -lpthread -lm



INCLUDES  =  -I$(PWD)/include -I$(PWD)/include/module -I$(PWD)/include/common -I$(PWD)/extern/ini -I$(PWD)/lib/include
CFLAGS += ${INCLUDES}

SRC = $(wildcard src/common/*.c) \
	  $(wildcard src/*.c) \
	  $(wildcard src/module/*.c) \
	  $(wildcard extern/ini/*.c) 
   
OBJ = $(patsubst %.c,%.o,${SRC})

export CC  CFLAGS LDFLAGS INCLUDES

TARGET = demo.bin

MAKE = make

all: $(TARGET)


$(TARGET): $(OBJ) 
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) 

%.o:%.c
	$(CC) $(CFLAG) $(INCLUDES) -o $@ -c $<

clean:
	@rm -rf $(OBJ)
	rm  *.o $(TARGET) -f


.PHONY: all clean 
