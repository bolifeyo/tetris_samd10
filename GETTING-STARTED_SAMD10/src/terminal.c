#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>


#include "uart.h"
#include "terminal.h"


void terminal_init(void)
{
	terminal_clear();
}


void terminal_clear(void)
{
	/*Clear screen*/
	uart_putc(27, NULL);
	uart_putc('[', NULL);
	uart_putc('2', NULL);
	uart_putc('J', NULL);
}


void terminal_cursor_off(void)
{
	uart_putc(27, NULL);
	uart_putc('[', NULL);
	uart_putc('?', NULL);
	uart_putc('2', NULL);
	uart_putc('5', NULL);
	uart_putc('l', NULL);
}

void terminal_cursor_home(void)
{
	uart_putc(27, NULL);
	uart_putc('[', NULL);
	uart_putc('H', NULL);
}


void terminal_putc(unsigned char c)
{
	uart_putc(c, NULL);
}

