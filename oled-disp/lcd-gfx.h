/*********************************************************************
 Library for text and graphics for LCD

 *********************************************************************/


int  lcd_x, lcd_y;
int  lcd_fontn;

void lcd_setfont(int f);
int  lcd_getfontwidth(void);
int  lcd_getfontheight(void);

void lcd_home(void);
void lcd_gotoxy(int x, int y);
int  lcd_getx(void);
int  lcd_gety(void);
void lcd_putc(int c);
void lcd_puts(char *s);
void lcd_putchar(int c, int update);
