/*
 * my_uart.c
 *
 * Created: 17/01/2022 10:44:48
 *  Author: Martin
 */ 

#include "system.h"
#include "cmsis_gcc.h"
#include "component\sercom.h"
#include "..\sercom\usart\usart_interrupt.h"
#include <string.h>
#include "my_uart.h"
#include "system_interrupt.h"

struct usart_module serials[MAX_UARTS];

volatile uint16_t rx_dbg[MAX_RX_BUFFER_LENGTH];
volatile uint16_t rx_modem[MAX_RX_BUFFER_LENGTH];
volatile uint8_t rx_dbg_buffer[MAX_RX_BUFFER_LENGTH] = {0};
volatile uint8_t rx_modem_buffer[MAX_RX_BUFFER_LENGTH] = {0};
	
uint8_t dbg_buff_ind = 0;
uint8_t modem_buff_ind = 0;	
uint8_t read_dbg_buff_ind = 0;
uint8_t read_modem_buff_ind = 0;

volatile bool rx_dbg_available = false;
volatile bool rx_modem_available = false;
	
void dbg_read_callback(struct usart_module *const usart_module);
void dbg_write_callback(struct usart_module *const usart_module);
void modem_read_callback(struct usart_module *const usart_module);
void modem_write_callback(struct usart_module *const usart_module);	

void UART_init(UARTmap_t uart, uint32_t baudrate) {
	UART_configure(uart, baudrate);
}

void UART_configure(UARTmap_t uart, uint32_t baudrate)
{
	struct usart_config config_usart;
		
	switch(uart) {
		case SERIAL_DBG:
			// Config Serial Debug
			usart_get_config_defaults(&config_usart);
			config_usart.baudrate    = baudrate;
			config_usart.mux_setting = USART_RX_1_TX_0_XCK_1;
			config_usart.pinmux_pad0 = PINMUX_PB08D_SERCOM4_PAD0;
			config_usart.pinmux_pad1 = PINMUX_PB09D_SERCOM4_PAD1;
			config_usart.pinmux_pad2 = PINMUX_UNUSED;
			config_usart.pinmux_pad3 = PINMUX_UNUSED;
			config_usart.start_frame_detection_enable = true;
			while (usart_init(&serials[uart], SERCOM4, &config_usart) != STATUS_OK) {}			
			usart_enable(&serials[uart]);
			usart_register_callback(&serials[uart], dbg_write_callback, USART_CALLBACK_BUFFER_TRANSMITTED);
			usart_register_callback(&serials[uart], dbg_read_callback, USART_CALLBACK_BUFFER_RECEIVED);
			usart_enable_callback(&serials[uart], USART_CALLBACK_BUFFER_TRANSMITTED);
			usart_enable_callback(&serials[uart], USART_CALLBACK_BUFFER_RECEIVED);	
			
			usart_read_job(&serials[uart], rx_dbg);	
			break;
			
		case SERIAL_MODEM:
			// Config Serial for modem
			usart_get_config_defaults(&config_usart);		
			config_usart.baudrate    = baudrate;
			config_usart.mux_setting = USART_RX_1_TX_0_XCK_1;
			config_usart.pinmux_pad0 = PINMUX_PA12C_SERCOM2_PAD0;
			config_usart.pinmux_pad1 = PINMUX_PA13C_SERCOM2_PAD1;
			config_usart.pinmux_pad2 = PINMUX_UNUSED;
			config_usart.pinmux_pad3 = PINMUX_UNUSED;
			while (usart_init(&serials[uart], SERCOM2, &config_usart) != STATUS_OK) {}			
			usart_enable(&serials[uart]);
			usart_register_callback(&serials[uart], modem_write_callback, USART_CALLBACK_BUFFER_TRANSMITTED);
			usart_register_callback(&serials[uart], modem_read_callback, USART_CALLBACK_BUFFER_RECEIVED);
			usart_enable_callback(&serials[uart], USART_CALLBACK_BUFFER_TRANSMITTED);
			usart_enable_callback(&serials[uart], USART_CALLBACK_BUFFER_RECEIVED);	
			
			usart_read_job(&serials[uart], rx_modem);
			break;
	}
}

uint8_t UART_read_line(UARTmap_t uart, uint8_t *read_buff, uint8_t max_len) {
	uint8_t i = 0, *read_ind, *buff_ind;
    uint8_t *rx_buffer;
    volatile bool *rx_available;
    
    switch(uart) {
        case SERIAL_DBG:
            read_ind = &read_dbg_buff_ind;
            buff_ind = &dbg_buff_ind;
            rx_buffer = rx_dbg_buffer;
            rx_available = &rx_dbg_available;
            break;

        case SERIAL_MODEM:
            read_ind = &read_modem_buff_ind;
            buff_ind = &modem_buff_ind;
            rx_buffer = rx_modem_buffer;
            rx_available = &rx_modem_available;
		    break;

        default:
            return 0;   // Error invalid UART
            break;
    }

	while( (*read_ind <= *buff_ind) && (i < max_len)) {
		read_buff[i++] = rx_buffer[*read_ind];
		*read_ind = (*read_ind + 1) % MAX_RX_BUFFER_LENGTH;
		
        system_interrupt_enter_critical_section();
		if(*read_ind == *buff_ind) {
			*rx_available = false;
            *buff_ind = 0;
            *read_ind = 0;
		}
        system_interrupt_leave_critical_section();

		if((read_buff[i-1] == '\n')) {
            read_buff[i] = '\0';
			break;
		}		
	}	
	
	return i;
}

void UART_write_line(UARTmap_t uart, uint8_t *line) {
	usart_write_buffer_job(&serials[uart], line, strlen(line));
}

bool UART_is_available(UARTmap_t uart) {
	switch(uart) {
		case SERIAL_DBG:
			return rx_dbg_available;
			break;
			
		case SERIAL_MODEM:
			return rx_modem_available;
			break;
			
		default:
			return false;
			break;
	}
}

//void UART_configure_callbacks(UARTmap_t uart)
//{
	//usart_register_callback(serials[uart], dbg_write_callback, USART_CALLBACK_BUFFER_TRANSMITTED);
	//usart_register_callback(serials[uart], dbg_read_callback, USART_CALLBACK_BUFFER_RECEIVED);
	//usart_enable_callback(serials[uart], USART_CALLBACK_BUFFER_TRANSMITTED);
	//usart_enable_callback(serials[uart], USART_CALLBACK_BUFFER_RECEIVED);
	//usart_register_callback(&serial_modem, modem_write_callback, USART_CALLBACK_BUFFER_TRANSMITTED);
	//usart_register_callback(&serial_modem, modem_read_callback, USART_CALLBACK_BUFFER_RECEIVED);
	//usart_enable_callback(&serial_modem, USART_CALLBACK_BUFFER_TRANSMITTED);
	//usart_enable_callback(&serial_modem, USART_CALLBACK_BUFFER_RECEIVED);
//}

/*************************************************************************************/
/* CALLBACKS */
/*************************************************************************************/
void dbg_read_callback(struct usart_module *const usart_module)
{	
	rx_dbg_buffer[dbg_buff_ind] = (uint8_t)rx_dbg[0];
	dbg_buff_ind = (dbg_buff_ind + 1) % MAX_RX_BUFFER_LENGTH;
	
    if(rx_dbg[0] == '\n') {
    	rx_dbg_available = true;
    }

	usart_read_job(&serials[SERIAL_DBG], rx_dbg);
}

void dbg_write_callback(struct usart_module *const usart_module)
{
	//port_pin_toggle_output_level(USR_LED1);
}

void modem_read_callback(struct usart_module *const usart_module)
{
    //usart_write_job(&serials[SERIAL_DBG], rx_modem);

	rx_modem_buffer[modem_buff_ind] = (uint8_t)rx_modem[0];
	modem_buff_ind = (modem_buff_ind + 1) % MAX_RX_BUFFER_LENGTH;
	
	if(rx_modem[0] == '\n') {
    	rx_modem_available = true;
	}

	usart_read_job(&serials[SERIAL_MODEM], rx_modem);
}

void modem_write_callback(struct usart_module *const usart_module)
{
	//port_pin_toggle_output_level(USR_LED2);
}