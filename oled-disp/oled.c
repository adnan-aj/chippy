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
#include <time.h>

#include "ssd1306.h"
#include "lcd-gfx.h"

//#include "fonts/linux/font.h"
//extern const struct font_desc *fonts[];

#define I2CADDR_OLED SSD1306_I2C_ADDRESS
#define msleep(MS)	usleep((MS) * 1000)

int  statusbar(void);

extern void lcd_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,
		     uint8_t color, uint8_t mode);
extern void circle(uint8_t x0, uint8_t y0, uint8_t radius, uint8_t color, uint8_t mode);


int  show_clock(void);

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
    display();
    invertDisplay(1);
    msleep(200);
    invertDisplay(0);
    msleep(200);
    invertDisplay(1);
    msleep(200);
    invertDisplay(0);
    msleep(200);
    
    while(1) {
	statusbar();
	
#if 0
	lcd_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,
		     uint8_t color, uint8_t mode);
	circle(uint8_t x0, uint8_t y0, uint8_t radius,
	       uint8_t color, uint8_t mode) {
#endif	
	lcd_line(0, 8, 150, 8, 1, 1);
        lcd_line(0, 0, 150, 100, 1, 1);
	circle(50, 50, 10, 1, 1);
	    
	show_clock();
	msleep(200);
    }
    
    
    return 0;
    
}


int statusbar(void)
{
    static time_t prev_time;
    time_t current_time;
    struct tm * time_info;
    char timeString[32];  // space for "HH:MM:SS\0"
    
    time(&current_time);
    if (current_time == prev_time) {
	return -1;
    }
    
    prev_time = current_time;
    time_info = localtime(&current_time);
    strftime(timeString, sizeof(timeString), "%a %h %e %H:%M", time_info);

    lcd_setfont(0);
    lcd_gotoxy(0,0);
    lcd_puts(timeString);
    return 0;
}

int show_clock(void)
{
    static time_t prev_time;
    time_t current_time;
    struct tm * time_info;
    char timeString[16];  // space for "HH:MM:SS\0"
    
    time(&current_time);
    if (current_time == prev_time) {
	return -1;
    }
    
    prev_time = current_time;
    time_info = localtime(&current_time);
    strftime(timeString, sizeof(timeString), "%H:%M:%S", time_info);

#if 0
    lcd_setfont(1);
    lcd_gotoxy(32,16);
    lcd_puts(timeString);

    lcd_setfont(3);
    lcd_gotoxy(32,32);
    lcd_puts(timeString);
#endif
    lcd_setfont(5);
    lcd_gotoxy((128 - (12 * 8))/2, (64 - 22) / 2 + 8);
    lcd_puts(timeString);
  
    return 0;
}