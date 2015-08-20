#include <stdlib.h>
#include <string.h>
#include "ssd1306.h"
#include "lcd-gfx.h"
#include "fonts/linux/font.h"

/* http://stackoverflow.com/questions/3982348/implement-generic-swap-macro-in-c */
#define swap(x,y) do \
    { unsigned char swap_temp[sizeof(x) == sizeof(y) ? (signed)sizeof(x) : -1]; \
	memcpy(swap_temp,&y,sizeof(x)); \
	memcpy(&y,&x,       sizeof(x)); \
	memcpy(&x,swap_temp,sizeof(x)); \
    } while(0)

#define pixel(X, Y, COLOR, MODE)	drawPixel((X), (Y), (COLOR))

extern const struct font_desc *fonts[];
static int cur_x = 0, cur_y = 0, fontn = 0;

void lcd_setfont(int f)
{
    if (f < 0)
	f = 0;
    if (f > 9)
	f = 9;
    fontn = f;
}

int  lcd_getfontwidth(void)
{
    //FIXME!!!
    return 8;
}

int  lcd_getfontheight(void)
{
    //FIXME!!!
    return 8;
}


void lcd_display(void)
{
    display();
}

void lcd_clear(void)
{
    clearDisplay();
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

void lcd_putc(int c)
{
    lcd_putchar(c, 0);
}

void lcd_puts(char *s)
{
    while (*s) {
	lcd_putchar(*s++, 0);
    }
}

void lcd_putchar(int c, int update)
{
    int x, y, height, width;
    unsigned char *p;
    const struct font_desc *font;
  
    font = fonts[fontn];
    width = font->width;
    height = font->height;
    p = (unsigned char *) font->data;

    if (width <= 8)
	p += (c * height); // font height
    else
	p += (c * height * 2); // font height
    
    // font height
    for (y = 0; y < height; y++) {
	// font width
	for (x = 0; x < width; x++) {
	    if (width <= 8) {
		drawPixel(cur_x + x, cur_y + y,
		      ((1 << (8 - 1 - x)) & *(p + y)) ? WHITE : 0);
	    }
	    else {
		if (x < 8)
		    drawPixel(cur_x + x, cur_y + y,
			      ((1 << (8 - 1 - x)) & *(p + (y * 2))) ? WHITE : 0);
		else
		    drawPixel(cur_x + x, cur_y + y,
			      ((1 << (16 - x - 1)) & *(p + (y * 2) + 1)) ? WHITE : 0);
	    }
	}
    }
    
    // update pixel cursor
    cur_x += width;
    if (cur_x > SSD1306_LCDWIDTH) {
	cur_x = 0;
	cur_y += height;
    }
    if (cur_y > SSD1306_LCDHEIGHT) {
	cur_y = 0;
    }

    if (update)
	display();
}


void lcd_line(int x0, int y0, int x1, int y1, int color, int mode)
{
    uint8_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
	swap(x0, y0);
	swap(x1, y1);
    }
    
    if (x0 > x1) {
	swap(x0, x1);
	swap(y0, y1);
    }
    
    uint8_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);
    
    int8_t err = dx / 2;
    int8_t ystep;
    
    if (y0 < y1) {
	ystep = 1;
    } else {
	ystep = -1;}
    
    for (; x0<x1; x0++) {
	if (steep) {
	    pixel(y0, x0, color, mode);
	} else {
	    pixel(x0, y0, color, mode);
	}
	err -= dy;
	if (err < 0) {
	    y0 += ystep;
	    err += dx;
	}
    }
}

void lcd_circle(int x0, int y0, int radius, int color, int mode)
{
    int8_t f = 1 - radius;
    int8_t ddF_x = 1;
    int8_t ddF_y = -2 * radius;
    int8_t x = 0;
    int8_t y = radius;
    
    pixel(x0, y0+radius, color, mode);
    pixel(x0, y0-radius, color, mode);
    pixel(x0+radius, y0, color, mode);
    pixel(x0-radius, y0, color, mode);
    
    while (x < y) {
	if (f >= 0) {
	    y--;
	    ddF_y += 2;
	    f += ddF_y;
	}
	x++;
	ddF_x += 2;
	f += ddF_x;
	
	pixel(x0 + x, y0 + y, color, mode);
	pixel(x0 - x, y0 + y, color, mode);
	pixel(x0 + x, y0 - y, color, mode);
	pixel(x0 - x, y0 - y, color, mode);
	
	pixel(x0 + y, y0 + x, color, mode);
	pixel(x0 - y, y0 + x, color, mode);
	pixel(x0 + y, y0 - x, color, mode);
	pixel(x0 - y, y0 - x, color, mode);
	
    }
}
