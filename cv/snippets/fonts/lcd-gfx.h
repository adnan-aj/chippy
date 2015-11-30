/*********************************************************************
 Library for text and graphics for LCD

 *********************************************************************/

void lcd_setfont(int f);
int  lcd_getfontwidth(void);
int  lcd_getfontheight(void);

void lcd_display(void);
void lcd_clear(void);
void lcd_home(void);
void lcd_gotoxy(int x, int y);
int  lcd_getx(void);
int  lcd_gety(void);
void lcd_putc(int c);
void lcd_puts(char *s);
void lcd_putchar(int c, int update);

void lcd_line(int x0, int y0, int x1, int y1, int color, int mode);
void lcd_circle(int x0, int y0, int radius, int color, int mode);

/* 
 * http://stackoverflow.com/questions/3982348/
 * implement-generic-swap-macro-in-c
 */
#define swap(x,y) do \
    { unsigned char swap_temp[sizeof(x) == sizeof(y) ? (signed)sizeof(x) : -1]; \
	memcpy(swap_temp,&y,sizeof(x)); \
	memcpy(&y,&x,       sizeof(x)); \
	memcpy(&x,swap_temp,sizeof(x)); \
    } while(0)


/* http://stackoverflow.com/questions/3437404/min-and-max-in-c */
#define max(a,b) \
    ({ __typeof__ (a) _a = (a); \
	__typeof__ (b) _b = (b); \
	_a > _b ? _a : _b; })

#define min(a,b) \
    ({ __typeof__ (a) _a = (a); \
	__typeof__ (b) _b = (b); \
	_a < _b ? _a : _b; })

