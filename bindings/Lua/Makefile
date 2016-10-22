# Makefile for Judger's lua binding

CC       = gcc
CFLAGS   = -Wall -Werror -O3 -std=c99 -fPIC
LDFLAGS ?= -pthread -lseccomp -fPIC -shared

JUDGER_OBJECTS = child.o killer.o logger.o runner.o rules/c_cpp.o rules/general.o
OBJECTS        = $(addprefix ../../src/,${JUDGER_OBJECTS}) judger.o

.PHONY: all test install clean

all: judger.so

judger.so: ${OBJECTS}
	${CC} ${OBJECTS} ${LDFLAGS} -o judger.so

install: all
	@mkdir -p /usr/local/lib/lua/5.3/
	@cp -f judger.so /usr/local/lib/lua/5.3/

clean:
	@rm -f ${OBJECTS} judger.so

%.o: %c
	${CC} ${CFLAGS} -o $@ -c $<

