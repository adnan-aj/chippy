/***********************************************************************
 * lcd-test.c - Test program for the Adafruit I2C 128x32 OLED display 
 * module connected to the Raspberry Pi I2C pins of the GPIO header.
 * 
 * The I2C OLED module is based on the SSD1306 OLED controller.
 * The I2C display core library routines are in i2cdev-ssd1306.c
 * The text and graphic rendering depends on lcd-gfx.c
 *
 * Copyright (c) 2015, Adnan Jalaludin <adnan singnet.com.sg>
 * All rights reserved.
 * BSD license, check LICENSE for more information.
 ***********************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>

#include "ssd1306.h"
#include "lcd-gfx.h"

//#include "fonts/linux/font.h"
//extern const struct font_desc *fonts[];

#define I2CADDR_OLED SSD1306_I2C_ADDRESS
#define msleep(MS)	usleep((MS) * 1000)

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
    if (r < 0) {
	close(fd);
	return -2;
    }
    
    /* read I2C test slave */
    r = read(fd, i2c_buffer, 4);
    if (r < 0) {
	close(fd);
	return -3;
    }

    return fd;
}

int main(int argc, char * argv[])
{
    int fd_oled;

    fd_oled = i2cdev_testopen("/dev/i2c-1", I2CADDR_OLED);
    if (fd_oled < 0) {
	printf("i2cdev_testopen: /dev/i2c-%d: OLED not found.\n", 1);
	return -1;
    }
    
    lcd_init(fd_oled);
    clearDisplay();
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


