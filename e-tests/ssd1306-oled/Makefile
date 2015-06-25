############################################################################
# Makefile for Adafruit 128x32 I2C OLED Display Module on the Raspberry Pi
#
# Depends on linux video console fonts sources in /usr/src/linux/drivers...
# Bitmap fonts are compiled as library in a separate directory as
#   ../fonts/fonts.a
############################################################################

CC = gcc
CFLAGS = -c -Wall -I ../c_examples -DSSD1306_128_32
LDFLAGS =

TARGET   = lcd
SOURCES  = lcd-test.c i2cdev-ssd1306.c
HEADERS  = ssd1306.h i2c-userspace.h ../fonts/font.h
FONTS    = fonts/fonts.a

OBJECTS = $(SOURCES:.c=.o)

$(TARGET): $(OBJECTS) $(FONTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(FONTS) -o $@

$(FONTS): fonts/Makefile fonts/linux/font.h fonts/fonts.c
	cd fonts/ ; make

.c.o: $(HEADERS)
	$(CC) $(CFLAGS) $< -o $@

clean:
	-rm -rf $(TARGET) *.o *~

distclean: clean
	cd fonts/ ; make clean
