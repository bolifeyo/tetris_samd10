#ifndef UART_H_INCLUDED
#define UART_H_INCLUDED


#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>


void fifo_init(void);
void uart_reset(void);
void uart_putc(unsigned char c, FILE *f);
bool uart_getc(unsigned char *c);
bool uart_rx_pending(void);
bool uart_tx_full(void);
#endif /*UART_H_INCLUDED*/