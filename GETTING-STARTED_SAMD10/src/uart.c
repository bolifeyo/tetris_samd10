#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <system_interrupt.h>

#include "uart.h"


static uint8_t fifo_rx_head;
static uint8_t fifo_rx_count;
static uint8_t fifo_rx_tail;
static unsigned char fifo_rx[16];
static uint8_t fifo_tx_head;
static uint8_t fifo_tx_count;
static uint8_t fifo_tx_tail;
static unsigned char fifo_tx[32];


void fifo_init(void)
{
	fifo_rx_head = 0;
	fifo_rx_count = 0;
	fifo_rx_tail = 0;
	
	fifo_tx_head = 0;
	fifo_tx_count = 0;
	fifo_tx_tail = 0;
	
	/*UART setup*/
	/*UCSR0B |= (1 << RXEN0 ) | (1 << TXEN0); // Turn on the transmission and reception circuitry
	UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01 ); // Use 8-bit character sizes
	UCSR0A |= (1 << U2X0);
	UBRR0H = 0; // Load upper 8-bits of the baud rate value into the high byte of the UBRR register
	UBRR0L = 7; // Load lower 8 -bits of the baud rate value into the low byte of the UBRR register
	UCSR0B |= (1 << RXCIE0 ); // Enable the USART Receive Complete interrupt (USART_RXC)*/
}

void uart_reset(void)
{
//	system_interrupt_disable_global();
	
	fifo_rx_head = 0;
	fifo_rx_count = 0;
	fifo_rx_tail = 0;
	
	fifo_tx_head = 0;
	fifo_tx_count = 0;
	fifo_tx_tail = 0;
//	system_interrupt_enable_global();
}


bool uart_putchar(unsigned char c)
{
	bool ret;
  //  system_interrupt_disable_global();
	if (fifo_tx_count != sizeof(fifo_tx)) 
	{
		++fifo_tx_count;
		fifo_tx[fifo_tx_head++] = c;
		if (fifo_tx_head == sizeof(fifo_tx)) 
		{
			fifo_tx_head = 0;
		}
	//	UCSR0B |= (1 << UDRE0);
		ret = true;
	} 
	else 
	{
		ret = false;
	}
//	system_interrupt_enable_global();
	
	return ret;
}


void uart_putc(unsigned char c, FILE *f)
{
	while (!putchar(c)) { ; }
}


bool uart_getc(unsigned char *c)
{
	*c='\0';
	system_interrupt_disable_global();
	*c=getchar();
	/*bool result = false;
	system_interrupt_disable_global();
	if (fifo_rx_count != 0) 
	{
		--fifo_rx_count;
		*c = fifo_rx[fifo_rx_tail++];
		if (fifo_rx_tail == sizeof(fifo_rx)) 
		{
			fifo_rx_tail = 0;
		}
		result = true;
	}
	system_interrupt_enable_global();
	*/
	system_interrupt_enable_global();
	if (*c!='\0')
	{
		return 1;
	}
	return 0;
}


bool uart_rx_pending(void)
{
	return false;
}


bool uart_tx_full(void)
{
	return false;
}

/*
ISR(USART_RX_vect, ISR_BLOCK)
{
	if (fifo_rx_count != sizeof(fifo_rx)) 
	{
		++fifo_rx_count;
	//	fifo_rx[fifo_rx_head++] = UDR0;
		if (fifo_rx_head == sizeof(fifo_rx)) 
		{
			fifo_rx_head = 0;
		}
	} 
	else 
	{
//		fifo_rx[fifo_rx_head] = UDR0;
	}
}

/*ISR(USART_UDRE_vect, ISR_BLOCK)
{
	if (fifo_tx_count != 0) 
	{
		--fifo_tx_count;
//		UDR0 = fifo_tx[fifo_tx_tail++];
		if (fifo_tx_tail == sizeof(fifo_tx)) 
		{
			fifo_tx_tail = 0;
		}
	} 
	else 
	{
	//	UCSR0B &= ~(1 << UDRE0);
	}
}*/