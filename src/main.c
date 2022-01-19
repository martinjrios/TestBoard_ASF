/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# Minimal main function that starts with a call to system_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#include <asf.h>
#include "system.h"
//#include "component\sercom.h"
//#include <string.h>
//#include "..\sercom\usart\usart_interrupt.h"
//#include "cmsis_gcc.h"
#include "my_uart.h"

volatile bool dbgLineReceived = false;
volatile bool modemLineReceived = false;
uint8_t rx_buff[MAX_RX_BUFFER_LENGTH] = {0};
uint8_t modem_buff[MAX_RX_BUFFER_LENGTH] = {0};

int main (void)
{
	system_init();
	delay_init();	// init delay
	UART_init(SERIAL_DBG, DEFAULT_BAUDRATE_DBG);
	UART_init(SERIAL_MODEM, DEFAULT_BAUDRATE_MODEM);
	system_interrupt_enable_global();

    uint8_t string[] = "Prueba AVLPLK90000\r\n";
    UART_write_line(SERIAL_DBG, string);
	
	/* Turn on modem */
	port_pin_set_output_level(PWRKEY, 1);
	delay_ms(500);
	port_pin_set_output_level(PWRKEY, 0);
	
	while (1) {
		///* Is button pressed? */
		//if (port_pin_get_input_level(USR_BUT) == 0) {
			///* Yes, so turn LED on. */
			//port_pin_set_output_level(USR_LED1, LED_ACTIVE);
		//} 
		//else {
			///* No, so turn LED off. */
			//port_pin_set_output_level(USR_LED1, LED_INACTIVE);
		//}
		
		if(UART_is_available(SERIAL_DBG)) {
            port_pin_set_output_level(USR_LED1, LED_ACTIVE);
			uint8_t len = UART_read_line(SERIAL_DBG, rx_buff, MAX_RX_BUFFER_LENGTH);
			UART_write_line(SERIAL_MODEM, rx_buff);
		}		
        else port_pin_set_output_level(USR_LED1, LED_INACTIVE);

		if(UART_is_available(SERIAL_MODEM)) {
    		port_pin_set_output_level(USR_LED2, LED_ACTIVE);
    		uint8_t len = UART_read_line(SERIAL_MODEM, modem_buff, MAX_RX_BUFFER_LENGTH);
    		UART_write_line(SERIAL_DBG, modem_buff);
		}
		else port_pin_set_output_level(USR_LED2, LED_INACTIVE);
		
		__WFI();
	}	
}
