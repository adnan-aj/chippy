# Makefile for the Linux graphics to console drivers.

CC = gcc
CFLAGS = -c -Wall -I ./
LDFLAGS =

TARGET = fonts.a

SRCDIR = ./
SOURCES := $(wildcard $(SRCDIR)/font_*.c)
SOURCES += fonts.c

OBJECTS := $(addprefix ,$(notdir $(SOURCES:.c=.o)))

$(TARGET): $(OBJECTS) fonts.o
	ar rvs $(TARGET) $?

%.o: %.c
	gcc $(CFLAGS) -c -o $@ $<

clean:
	-rm -rf $(TARGET) *.o *~
