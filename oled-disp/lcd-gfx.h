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
