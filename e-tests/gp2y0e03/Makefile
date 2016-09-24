#TOOL_DIR := /pool/arm/toolchain/root/usr/bin
#CROSS_PREFIX := arm-none-linux-gnueabi-
#CC := $(TOOL_DIR)/$(CROSS_PREFIX)gcc
CC := gcc
CFLAGS := -Wall

all: gp2y0e03
gp2y0e03: gp2y0e03.o
gp2y0e03.o: gp2y0e03.c

clean:
	@rm -f gp2y0e03 *.o *~
