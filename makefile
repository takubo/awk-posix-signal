# makefile for AWK-Signal

AWK_H_DIR = ${HOME}/gawk-4.0.1
CC = gcc
CFLAGS = -Wall -fPIC -shared -g -c -O2 -DHAVE_STRING_H -DHAVE_SNPRINTF -DHAVE_STDARG_H -DHAVE_VPRINTF -DDYNAMIC -I/usr/include/opencv -I${AWK_H_DIR}
LDFLAGS = -shared

all: awk-signal.so
	gawk 'BEGIN{ extension("./awk-signal.so", "dlload") }'

awk-signal.so: awk-signal.c makefile
	${CC} $< ${CFLAGS}
	${CC} awk-signal.o -o $@ ${LDFLAGS}

