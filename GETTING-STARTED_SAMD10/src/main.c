/**
 * \file
 *
 * \brief Getting Started Application.
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

/**
 * \mainpage Getting Started Application
 *
 * \section Purpose
 *
 * The Getting Started example will help new users get familiar with Atmel's
 * SAM family of microcontrollers. This basic application shows the startup
 * sequence of a chip and how to use its core peripherals.
 *
 * \section Requirements
 *
 * This application has been tested on following boards:
 * - SAM D21/R21/L21/L22/C21 Xplained Pro
 * - SAM D10 Xplained Mini
 *
 * \section Description
 *
 * The program demo how LED,button,delay,interrupt and timer/counter work .
 *
 * \section Usage
 *
 * -# Build the program and download it inside the evaluation board.
 * -# On the computer, open and configure a terminal application
 *    (e.g. HyperTerminal on Microsoft Windows) with these settings:
 *   - 38400 bauds
 *   - 8 bits of data
 *   - No parity
 *   - 1 stop bit
 *   - No flow control
 * -# Start the application.
 * -# The LED(s) should start blinking on the board. In the terminal window, the
 *    following text should appear (values depend on the board and chip used):
 *    \code
 *     -- Getting Started Example xxx --
 *     -- xxxxxx-xx
 *     -- Compiled: xxx xx xxxx xx:xx:xx --
 *    \endcode
 * -# Pressing and release button SW0 should make LED0 on and off
 *    blinking.
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "asf.h"
#include "stdio_serial.h"
#include "conf_uart_serial.h"

#include "uart.h"
#include "terminal.h"
#include "tetris.h"

#ifdef __cplusplus
extern "C" {
#endif
//--------------------Define------------------
#define TC_COUNT_VALUE 55535

//--------------------Variables---------------

static struct usart_module cdc_uart_module;
static struct tc_module tc_instance;

static bool is_running = false;
static bool volatile iterate_game = false;
static uint16_t volatile tick;

//--------------------Functions------------------
void USART_RX_callback(const struct usart_module *const usart_module)
{
   port_pin_toggle_output_level(LED0_PIN);
}
/**
 *  Configure UART console.
 */
static void configure_console(void)
{
	struct usart_config usart_conf;

	usart_get_config_defaults(&usart_conf);
	usart_conf.mux_setting = CONF_STDIO_MUX_SETTING;
	usart_conf.pinmux_pad0 = CONF_STDIO_PINMUX_PAD0;
	usart_conf.pinmux_pad1 = CONF_STDIO_PINMUX_PAD1;
	usart_conf.pinmux_pad2 = CONF_STDIO_PINMUX_PAD2;
	usart_conf.pinmux_pad3 = CONF_STDIO_PINMUX_PAD3;
	usart_conf.baudrate    = CONF_STDIO_BAUDRATE;

	stdio_serial_init(&cdc_uart_module, CONF_STDIO_USART_MODULE, &usart_conf);
	
}
enum status_code usart_enable_rx_interrupt(	struct usart_module *const module,	uint8_t *rx_data)
{
	// Sanity check arguments
	Assert(module);
	Assert(rx_data);

	// Issue internal asynchronous read
	// Get a pointer to the hardware module instance
	SercomUsart *const usart_hw = &(module->hw->USART);
	module->rx_buffer_ptr = rx_data;
	// Enable the RX Complete Interrupt
	usart_hw->INTENSET.reg = SERCOM_USART_INTFLAG_RXC;
	return STATUS_OK;
}
void configure_usart_callbacks(void)
{
	usart_register_callback(&cdc_uart_module, USART_RX_callback, USART_CALLBACK_BUFFER_RECEIVED);
	usart_enable_callback(&cdc_uart_module, USART_CALLBACK_BUFFER_RECEIVED);
}

/** TC Callback function.
 */
static void tc_callback_to_counter(	struct tc_module *const module_inst)
{
	++tick;
	iterate_game = true;
	
}

/** Configures  TC function with the  driver.
 */
static void configure_tc(void)
{
	struct tc_config config_tc;

	tc_get_config_defaults(&config_tc);
	config_tc.counter_size    = TC_COUNTER_SIZE_16BIT;
	config_tc.wave_generation = TC_WAVE_GENERATION_MATCH_FREQ;
	config_tc.counter_16_bit.compare_capture_channel[0] = 2000; 
	config_tc.clock_prescaler=TC_CLOCK_PRESCALER_DIV1024;

	tc_init(&tc_instance, CONF_TC_INSTANCE, &config_tc);
	tc_enable(&tc_instance);
}

/** Registers TC callback function with the  driver.
 */
static void configure_tc_callbacks(void)
{
	tc_register_callback(&tc_instance,	tc_callback_to_counter,	TC_CALLBACK_CC_CHANNEL0);
	tc_enable_callback(&tc_instance, TC_CALLBACK_CC_CHANNEL0);
}

/**
 *  \brief getting-started Application entry point.
 *
 *  \return Unused (ANSI-C compatibility).
*/
int main(void)
{
	struct port_config pin;
	unsigned char c;
	
	system_init();

	//Configure UART console.
	configure_console();
	
	configure_usart_callbacks();
	usart_enable_rx_interrupt(&cdc_uart_module,&c);
	
    usart_enable(&cdc_uart_module);
	//Configures  TC driver
	configure_tc();

	//Configures TC callback
	configure_tc_callbacks();

	//Initialize the delay driver
	delay_init();

	//Enable system interrupt
	system_interrupt_enable_global();

    //Configures PORT for LED0
	port_get_config_defaults(&pin);
	pin.direction = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(LED0_PIN, &pin);

	port_pin_set_output_level(LED0_PIN, LED0_INACTIVE);
	port_pin_set_output_level(LED0_PIN, LED0_INACTIVE);

	/*main loop*/
	while(1)
	{
		if (is_running) 
		{
			//Handle user's input
	//		if (uart_getc(&c)) 
	//		{
				switch (c) 
				{
					case 'w':
					case ' ':
					   //ROTATE
					   tetris_rotate();
					break;
					case 's':
					   //DOWN
					   tetris_gravity();	
					break;
					case 'd':
					   //RIGHT
					   tetris_move_right();
					break;
					case 'a':
					   //LEFT
					   tetris_move_left();
					break;
					default: break;
				}
				c=0;
	//		}
			// was here if(!iterate_game) 
			if(iterate_game) 
			{
				//Update game
				iterate_game = false;
				tetris_gravity();
				tetris_check_lines();
				terminal_cursor_home();
				tetris_print();			
				if (tetris_is_game_over()) 
				{
					is_running = false;
				}
			}
		} 
		else 
		{
		//	if (uart_getc(&c)) 
		//	{
				if (c == 'n') 
				{
					c=0;
					//Seed random function so we do not get same start condition
					//for each new game. In essence we will not start a new game
					//exactly at the same time.
					srand(tick);
					
					//New Game
					is_running = true;
					terminal_cursor_off();
					terminal_clear();
					tetris_init();
					tetris_new_block();
					terminal_cursor_home();
					tetris_print();
				}	
		//	}
		}
	}
}

#ifdef __cplusplus
}
#endif
