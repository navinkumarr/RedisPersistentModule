# set environment variable HANDLER_INCLUDE_DIR to the location of redismodule.h
ifndef HANDLER_INCLUDE_DIR
	HANDLER_INCLUDE_DIR=../../
endif

CFLAGS = -g -fPIC -lc -lm -O3 -std=gnu99 -I$(HANDLER_INCLUDE_DIR) -Wall -Wno-unused-function
CC=gcc

OBJS=handler.o FileHandler.o

all: libhandler.a

clean:
	rm -rf *.o *.a

libhandler.a: $(OBJS)
	ar rcs $@ $^

