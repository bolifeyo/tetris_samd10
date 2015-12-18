#ifndef TERMINAL_H_INCLUDED
#define TERMINAL_H_INCLUDED


void terminal_init(void);
void terminal_clear(void);
void terminal_cursor_off(void);
void terminal_cursor_home(void);
void terminal_putc(unsigned char c);
#endif /*TERMINAL_H_INCLUDED*/