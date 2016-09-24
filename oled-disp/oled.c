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
#include <sys/stat.h>
#include <math.h>
#include "ssd1306.h"
#include "lcd-gfx.h"

#define I2CADDR_OLED SSD1306_I2C_ADDRESS
#define msleep(MS)	usleep((MS) * 1000)

int  statusbar(void);
int  show_time(void);
int  show_analog(void);

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

time_t prev_time = 0;
time_t current_time;
struct tm * time_info;
time_t config_time = 0;
/* https://raw.githubusercontent.com/sparkfun/
 * SparkFun_Micro_OLED_Arduino_Library/master/examples/
 * MicroOLED_Clock/MicroOLED_Clock.ino
 */

int hours = 11;
int minutes = 55;
int seconds = 35;

// Global variables to help draw the clock face:
const int MIDDLE_X = 128 / 4;
const int MIDDLE_X2 = 128 / 4 * 3;
const int MIDDLE_Y = ((64 - 8)  / 2 + 8);

int CLOCK_RADIUS;
int POS_12_X, POS_12_Y;
int POS_3_X, POS_3_Y;
int POS_6_X, POS_6_Y;
int POS_9_X, POS_9_Y;
int S_LENGTH;
int M_LENGTH;
int H_LENGTH;

unsigned long lastDraw = 0;

void drawArms(int h, int m, int s);
void drawFace(void);
void initClockVariables(void);

int main(int argc, char * argv[])
{
    int fd_oled, fd_config;
    struct stat sb;
    int display_mode = 0;
    int time_changed;
    int config_changed;
    int wait;
    FILE *f_config;

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
    
    initClockVariables();

    wait = 0;
    config_time = 0;
    display_mode = 0;
    
    while(1) {
	if (stat("/tmp/.oled", &sb) == 0)
	{
	    if (sb.st_mtime != config_time) {
		config_time = sb.st_mtime;
		//printf("Last file modification:   %s", ctime(&config_time));	config_changed = 0;
		if ((f_config = fopen("/tmp/.oled", "r")) != NULL) {
		    fscanf(f_config, "%d", &display_mode);
		    //printf("Current display mode: %d\n", display_mode);
		    fclose(f_config);
		}
	    }
	}
	
	time_changed = 0;
	time(&current_time);
	if (current_time != prev_time) {
	    time_changed = 1;
	    time_info = localtime(&current_time);
	    prev_time = current_time;
	}

	switch(display_mode) {
	case 0:
	    if (time_changed) {
		lcd_clear();
		show_time();
	    }
	    break;
	case 1:
	    if (time_changed) {
		lcd_clear();
		show_analog();
	    }
	    break;
	default:
	    break;
	}

	if (time_changed) {
	    statusbar();
	}
	lcd_display();
	msleep(200);
    }
    
    return 0;
}

int statusbar(void)
{
    char timeString[32];  // space for "HH:MM:SS\0"
    
    strftime(timeString, sizeof(timeString), "%a %h %e %H:%M", time_info);
    lcd_setfont(0);
    lcd_gotoxy(0,0);
    lcd_puts(timeString);
    return 0;
}

int show_time(void)
{
    char timeString[32];  // space for "HH:MM:SS\0"
    
    strftime(timeString, sizeof(timeString), "%H:%M:%S", time_info);
    lcd_setfont(5);
    // TODO: show defintions of centre justify calculations
    lcd_gotoxy((128 - (12 * 8))/2, (64 - 22) / 2 + 8);
    lcd_puts(timeString);
    return 0;
}
    
int show_analog(void)
{
    hours = time_info->tm_hour;
    minutes = time_info->tm_min;
    seconds = time_info->tm_sec;
    
    drawFace();
    drawArms(hours, minutes, seconds);
    return 0;
}
    
void initClockVariables()
{
    // Calculate constants for clock face component positions:
    lcd_setfont(0);
//    CLOCK_RADIUS = min(MIDDLE_X, MIDDLE_Y) - 1;
    CLOCK_RADIUS = (64 - 8) / 2 - 2;
    POS_12_X = MIDDLE_X - lcd_getfontwidth();
    POS_12_Y = MIDDLE_Y - CLOCK_RADIUS + 2;
    POS_3_X  = MIDDLE_X + CLOCK_RADIUS - lcd_getfontwidth() - 1;
    POS_3_Y  = MIDDLE_Y - lcd_getfontheight()/2;
    POS_6_X  = MIDDLE_X - lcd_getfontwidth()/2;
    POS_6_Y  = MIDDLE_Y + CLOCK_RADIUS - lcd_getfontheight() - 1;
    POS_9_X  = MIDDLE_X - CLOCK_RADIUS + lcd_getfontwidth() - 2;
    POS_9_Y  = MIDDLE_Y - lcd_getfontheight()/2;
    
    // Calculate clock arm lengths
    S_LENGTH = CLOCK_RADIUS - 2;
    M_LENGTH = S_LENGTH * 0.7;
    H_LENGTH = S_LENGTH * 0.5;
}

// Draw the clock's three arms: seconds, minutes, hours.
void drawArms(int h, int m, int s)
{
    double midHours;  // this will be used to slightly adjust the hour hand
    static int hx, hy, mx, my, sx, sy, sx2, sy2;
    char text[8];

    sprintf(text, ":%02d", s);
    
    
    // Adjust time to shift display 90 degrees ccw
    // this will turn the clock the same direction as text:
    h -= 3;
    m -= 15;
    s -= 15;
    if (h <= 0)
	h += 12;
    if (m < 0)
	m += 60;
    if (s < 0)
	s += 60;
    
//mapping    Y = (X-A)/(B-A) * (D-C) + C
#define map(X, A, B, C, D) (((float)(X)-(float)(A))/((float)(B)-(float)(A)) * ((float)(D)-(float)(C)) + (float)(C))
    
    // Calculate and draw new lines:
    s = map(s, 0, 60, 0, 360);  // map the 0-60, to "360 degrees"
    sx = S_LENGTH * cos(M_PI * ((float)s) / 180);  // woo trig!
    sy = S_LENGTH * sin(M_PI * ((float)s) / 180);  // woo trig!
    sx2 = 0.7 * S_LENGTH * cos(M_PI * ((float)s) / 180);  // woo trig!
    sy2 = 0.7 * S_LENGTH * sin(M_PI * ((float)s) / 180);  // woo trig!
    // draw the second hand:
    lcd_line(MIDDLE_X, MIDDLE_Y, MIDDLE_X + sx, MIDDLE_Y + sy, 1, 0);

    m = map(m, 0, 60, 0, 360);  // map the 0-60, to "360 degrees"
    mx = M_LENGTH * cos(M_PI * ((float)m) / 180);  // woo trig!
    my = M_LENGTH * sin(M_PI * ((float)m) / 180);  // woo trig!
    // draw the minute hand
    lcd_line(MIDDLE_X, MIDDLE_Y, MIDDLE_X + mx, MIDDLE_Y + my, 1, 0);
    
    midHours = minutes/12;  // midHours is used to set the hours hand to middling levels between whole hours
    h *= 5;  // Get hours and midhours to the same scale
    h += midHours;  // add hours and midhours
    h = map(h, 0, 60, 0, 360);  // map the 0-60, to "360 degrees"
    hx = H_LENGTH * cos(M_PI * ((float)h) / 180);  // woo trig!
    hy = H_LENGTH * sin(M_PI * ((float)h) / 180);  // woo trig!
    // draw the hour hand:
    lcd_line(MIDDLE_X, MIDDLE_Y, MIDDLE_X + hx, MIDDLE_Y + hy, 1, 0);


    // Seconds face of dual analog display
    lcd_line(MIDDLE_X2 + sx2, MIDDLE_Y + sy2, MIDDLE_X2 + sx, MIDDLE_Y + sy, 1, 0);
    lcd_setfont(0);
    lcd_gotoxy(MIDDLE_X2 - (lcd_getfontwidth()*1.5), MIDDLE_Y - lcd_getfontheight() /2);
    lcd_puts(text);

}

// Draw an analog clock face
void drawFace()
{
    // Draw the clock border
    lcd_circle(MIDDLE_X, MIDDLE_Y, CLOCK_RADIUS, 1, 0);
    lcd_circle(MIDDLE_X2, MIDDLE_Y, CLOCK_RADIUS, 1, 0);
    // Draw the clock numbers
    lcd_setfont(0); // set font type 0, please see declaration in SFE_MicroOLED.cpp
    lcd_gotoxy(POS_12_X, POS_12_Y); // points cursor to x=27 y=0
    lcd_puts("12");
    lcd_gotoxy(POS_6_X, POS_6_Y);
    lcd_puts("6");
    lcd_gotoxy(POS_9_X, POS_9_Y);
    lcd_puts("9");
    lcd_gotoxy(POS_3_X, POS_3_Y);
    lcd_puts("3");
}
