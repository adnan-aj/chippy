#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <linux/i2c-dev.h>


#include "ssd1306.h"
#include "logo.h"
#define _BV( bit ) ( 1<<(bit) ) 
static unsigned tempbuf[16*32];

// the most basic function, set a single pixel
void drawPixel(int16_t x, int16_t y, uint16_t color)
{
#if 0
    if ((x < 0) || (x >= width()) || (y < 0) || (y >= height()))
	return;
    // check rotation, move pixel around if necessary
    switch (getRotation()) {
    case 1:
	swap(x, y);
	x = WIDTH - x - 1;
	break;
    case 2:
	x = WIDTH - x - 1;
	y = HEIGHT - y - 1;
	break;

    case 3:
	swap(x, y);
	y = HEIGHT - y - 1;
	break;
  }  
#endif
    // x is which column
    if (color == WHITE) 
	buffer[x + (y / 8) * SSD1306_LCDWIDTH] |= _BV(( y % 8));  
    else
	buffer[x + (y / 8) * SSD1306_LCDWIDTH] &= ~_BV((y % 8)); 
}

int fd_oled = -1;

static int i2c_writeblock(unsigned char *buf, int len)
{
    if (len > 32) {
	printf("%s: len param >32 (%d).\n", __func__, len);
	return -1;
    }
    if (fd_oled < 0) {
	printf("%s: fd_oled not set (%d).\n", __func__, fd_oled);
	return -2;
    }
    return i2c_smbus_write_i2c_block_data(fd_oled, buf[0], len - 1, buf + 1);
}

void lcd_WD(unsigned char value)
{
    /* write single command to LCD */
    unsigned char obuf[16];
    obuf[0] = 0x40;					// Co=0, A0=0
    obuf[1] = value;
    i2c_writeblock(obuf, 2);
}

void lcd_WC(unsigned char value)
{
    /* write single command to LCD */
    unsigned char obuf[16];
    obuf[0] = 0x00;					// Co=0, A0=0
    obuf[1] = value;
    i2c_writeblock(obuf, 2);
}

void lcd_WC2(unsigned char value1, unsigned char value2)
{
    /* write double command to LCD */
    unsigned char obuf[16];
    obuf[0] = 0x80;					// Co=1, A0=0
    obuf[1] = value1;
    obuf[2] = 0x00;					// Co=0, A0=0
    obuf[3] = value2;
    i2c_writeblock(obuf, 4);
}

void lcd_WD4(unsigned char value)
{
    /*write data to LCD */
    unsigned char obuf[16];
    obuf[0] = 0xc0;					// Co=1, A0=1
    obuf[1] = value;
    obuf[2] = 0xc0;					// Co=1, A0=1
    obuf[3] = value;
    obuf[4] = 0xc0;					// Co=1, A0=1
    obuf[5] = value;
    obuf[6] = 0x40;					// Co=0, A0=1
    obuf[7] = value;
    i2c_writeblock(obuf, 8);
}

void ssd1306_command(uint8_t c)
{ 
    // I2C
#if 1
    lcd_WC(c);
#else
    uint8_t control = 0x00;   // Co = 0, D/C = 0
    Wire.beginTransmission(_i2caddr);
    Wire.write(control);
    Wire.write(c);
    Wire.endTransmission();
#endif
}

void ssd1306_data(uint8_t c) {
    // I2C
#if 1
    lcd_WD(c);
#else    
    uint8_t control = 0x40;   // Co = 0, D/C = 1
    Wire.beginTransmission(_i2caddr);
    Wire.write(control);
    Wire.write(c);
    Wire.endTransmission();
#endif
}

void lcd_init(int fd_i2cdev)
{
    int i;
//    int vccstate = SSD1306_EXTERNALVCC;
    int vccstate = 0;

    fd_oled = fd_i2cdev;
    //   #if defined SSD1306_128_32
    // Init sequence for 128x32 OLED module
    ssd1306_command(SSD1306_DISPLAYOFF);                    // 0xAE
    ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
    ssd1306_command(0x80);                                  // the suggested ratio 0x80
    ssd1306_command(SSD1306_SETMULTIPLEX);                  // 0xA8
    ssd1306_command(0x3f);
    ssd1306_command(SSD1306_SETDISPLAYOFFSET);              // 0xD3
    ssd1306_command(0x0);                                   // no offset
    ssd1306_command(SSD1306_SETSTARTLINE | 0x0);            // line #0
    ssd1306_command(SSD1306_CHARGEPUMP);                    // 0x8D
    if (vccstate == SSD1306_EXTERNALVCC) {
	ssd1306_command(0x10);
    }
    else {
	ssd1306_command(0x14);
    }
    ssd1306_command(SSD1306_MEMORYMODE);                    // 0x20
    ssd1306_command(0x00);                                  // 0x0 act like ks0108
    ssd1306_command(SSD1306_SEGREMAP | 0x1);
    ssd1306_command(SSD1306_COMSCANDEC);
    ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
    ssd1306_command(0x12);
    ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
    ssd1306_command(0x8F);
    ssd1306_command(SSD1306_SETPRECHARGE);                  // 0xd9
    if (vccstate == SSD1306_EXTERNALVCC) {
	ssd1306_command(0x22);
    }
    else {
	ssd1306_command(0xF1);
    }
    ssd1306_command(SSD1306_SETVCOMDETECT);                 // 0xDB
    ssd1306_command(0x40);
    ssd1306_command(SSD1306_DISPLAYALLON_RESUME);           // 0xA4
    ssd1306_command(SSD1306_NORMALDISPLAY);                 // 0xA6
    //  #endif
    // 
    ssd1306_command(SSD1306_DISPLAYON);//--turn on oled panel
    
    for (i = 0; i < (16*32); i++) {
	tempbuf[i] = buffer[i];
	buffer[i] = 0;
    }
    display();
    for (i = 0; i < (16*32); i++) {
	buffer[i] = tempbuf[i];
    }
}


void invertDisplay(uint8_t i) {
    if (i) {
	ssd1306_command(SSD1306_INVERTDISPLAY);
    } else {
	ssd1306_command(SSD1306_NORMALDISPLAY);
    }
}

void setContrast(uint8_t contrast) {
    ssd1306_command(SSD1306_SETCONTRAST);
    ssd1306_command(contrast);
}

void display(void)
{
    unsigned char obuf[32];
    int i, page = 0, blk, col = 0;

    ssd1306_command(0xb0 | page); // line #0
    ssd1306_command(SSD1306_SETHIGHCOLUMN | ((col >> 4) & 0x0f) );  // hi col = 0
    ssd1306_command(SSD1306_SETLOWCOLUMN | (col & 0x0f) );  // low col = 0
    
    for (page = 0; page < 8; page++) {
	for (blk = 0; blk < (128/16); blk++) {
	    obuf[0] = 0x40;
	    for (i = 0; i < 16; i++) {
		obuf[i + 1] = buffer[page * 128 + blk * 16 + i];
	    }
	    i2c_writeblock(obuf, 16 + 1);
	}
    }
}

// clear everything
void clearDisplay(void)
{
    memset(buffer, 0, (SSD1306_LCDWIDTH*SSD1306_LCDHEIGHT/8));
}

void stopscroll(void)
{
    ssd1306_command(SSD1306_DEACTIVATE_SCROLL);
}

void startscrollright(uint8_t start, uint8_t stop)
{
    ssd1306_command(SSD1306_RIGHT_HORIZONTAL_SCROLL);
    ssd1306_command(0X00);
    ssd1306_command(start);
    ssd1306_command(0X00);
    ssd1306_command(stop);
    ssd1306_command(0X01);
    ssd1306_command(0XFF);
    ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}

void startscrollleft(uint8_t start, uint8_t stop)
{
    ssd1306_command(SSD1306_LEFT_HORIZONTAL_SCROLL);
    ssd1306_command(0X00);
    ssd1306_command(start);
    ssd1306_command(0X00);
    ssd1306_command(stop);
    ssd1306_command(0X01);
    ssd1306_command(0XFF);
    ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}

void startscrolldiagright(uint8_t start, uint8_t stop)
{
    ssd1306_command(SSD1306_SET_VERTICAL_SCROLL_AREA);	
    ssd1306_command(0X00);
    ssd1306_command(SSD1306_LCDHEIGHT);
    ssd1306_command(SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL);
    ssd1306_command(0X00);
    ssd1306_command(start);
    ssd1306_command(0X00);
    ssd1306_command(stop);
    ssd1306_command(0X01);
    ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}

void startscrolldiagleft(uint8_t start, uint8_t stop)
{
    ssd1306_command(SSD1306_SET_VERTICAL_SCROLL_AREA);	
    ssd1306_command(0X00);
    ssd1306_command(SSD1306_LCDHEIGHT);
    ssd1306_command(SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL);
    ssd1306_command(0X00);
    ssd1306_command(start);
    ssd1306_command(0X00);
    ssd1306_command(stop);
    ssd1306_command(0X01);
    ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}

