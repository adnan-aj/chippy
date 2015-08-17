/***********************************************************************
 * lcd-test.c - Test program for the Adafruit I2C 128x32 OLED display 
 * module connected to the Raspberry Pi I2C pins of the GPIO header.
 * 
 * The Adafruit I2C OLED module is based on the SSD1306 OLED controller.
 * The I2C display core library routines are in adafruit-i2c-oled.c
 * The I2C routines depends on i2c-userspace.h inline functions.
 *
 * Copyright (c) 2012-2013, Adnan Jalaludin <adnan singnet.com.sg>
 * All rights reserved.
 * BSD license, check LICENSE for more information.
 ***********************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "i2c-userspace.h"
#include "ssd1306.h"
#include "fonts/linux/font.h"

extern const struct font_desc *fonts[];

#define I2CADDR_OLED 0x3c

#define msleep(MS)	usleep((MS) * 1000)

void lcd_home(void);
int lcd_putchar(int c);
int lcd_putc(int c, int update);
int lcd_puts(char *s);
int lcd_setfont(int v);
void lcd_gotoxy(int x, int y);

int i2cdev_testopen(const char *devbusname, int i2caddr_test)
{
    int fd, r;
    unsigned char i2c_buffer[16];
    
    /* test bus */
    fd = open(devbusname, O_RDWR);
    if (fd < 0)
	return -1;
    
    /* setup test device as slave*/
    r = ioctl(fd, I2C_SLAVE, i2caddr_test);
    if (r < 0)
       return -2;
    
    /* read I2C test slave */
    r = read(fd, i2c_buffer, 4);
    if (r < 0)
	return -3;

    return fd;
}

int main(int argc, char * argv[])
{
    int rpi_rev, fd_adafruit;

    rpi_rev = 1;
    /* Test of I2C bus file and connectivity to Adafruit OLED chip */
    fd_adafruit = i2cdev_testopen("/dev/i2c-1", I2CADDR_OLED);
    
    if (fd_adafruit < 0) {
	rpi_rev = 2;
	fd_adafruit = i2cdev_testopen("/dev/i2c-1", I2CADDR_OLED);
    }
    if (fd_adafruit < 0) {
	printf("i2cdev_testopen: /dev/i2c-x unsuccessful, OLED not found.\n");
	return -1;
    }
    printf("i2cdev_testopen: successful, fd_i2cbus = %d, rpi_rev = %d\n",
	   fd_adafruit,
	   rpi_rev);
    
    lcd_init(fd_adafruit);
    invertDisplay(1);
    msleep(200);
    invertDisplay(0);
    msleep(200);
    invertDisplay(1);
    msleep(200);
    invertDisplay(0);
    msleep(200);
    
    // display Adafruit's startup buffer
    display();
    startscrollleft(0, 127);
    msleep(1000);
    stopscroll();
    msleep(200);
 
//    clearDisplay();
    display();
#if 1
    if (argc > 1)
	lcd_puts(argv[1]);  
#endif

    return 0;
    
}


static int cur_x = 0, cur_y = 0, fontn = 0;

int lcd_setfont(int f)
{
    if (f < 0)
	f = 0;
    if (f > 9)
	f = 9;
    fontn = f;
    return f;
}

void lcd_home(void)
{
    cur_x = cur_y = 0;
}

void lcd_gotoxy(int x, int y)
{
    cur_x = x;
    cur_y = y;
}  

#if 1
int lcd_putchar(int c)
{
    return lcd_putc(c, 0);
}

int lcd_puts(char *s)
{
    while (*s) {
	lcd_putc(*s++, 0);
    }
    display();
    return 0;
}

int lcd_putc(int c, int update)
{
    int x, y, height, width;
    unsigned char *p;
    struct font_desc *font;
  
    font = fonts[fontn];
    width = font->width;
    height = font->height;
    p = (unsigned char *) font->data;
    
    p += (c * height); // font height
    
    // font height
    for (y = 0; y < height; y++) {
	// font width
	for (x = 0; x < width; x++) {
	    drawPixel(cur_x + x, cur_y + y,
		      ((1 << (8 - 1 - x)) & *(p + y)) ? WHITE : 0);
	}
    }
    if (update)
	display();
    
    cur_x += width; // font width
    return 0;
}

#endif
