# Makefile for Judger's lua binding

CC           = gcc
LDFLAGS     ?= -pthread -lseccomp -fPIC -shared
LUAVER      ?= 5.3
LUAVERPURE  ?= $(subst .,,${LUAVER})
CFLAGS      ?= -Wall -Werror -O3 -std=c99 -fPIC -I/usr/include/lua${LUAVER} -DLUA${LUAVERPURE}

JUDGER_OBJECTS = child.o killer.o logger.o runner.o rules/c_cpp.o rules/general.o
OBJECTS        = $(addprefix ../../src/,${JUDGER_OBJECTS}) judger.o

.PHONY: all test install clean

all: judger.so

judger.so: ${OBJECTS}
	${CC} ${OBJECTS} ${LDFLAGS} -o judger.so

install: all
	@mkdir -p /usr/local/lib/lua/${LUAVER}/
	@cp -f judger.so /usr/local/lib/lua/${LUAVER}/

clean:
	@rm -f ${OBJECTS} judger.so

%.o: %c
	${CC} ${CFLAGS} -o $@ -c $<

