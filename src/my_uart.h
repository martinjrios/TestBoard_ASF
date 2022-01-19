/*
 * my_uart.h
 *
 * Created: 17/01/2022 10:45:16
 *  Author: Martin
 */ 


#ifndef MY_UART_H_
#define MY_UART_H_

#define MAX_RX_BUFFER_LENGTH		128

#define DEFAULT_BAUDRATE_MODEM			115200
#define DEFAULT_BAUDRATE_DBG			115200

typedef enum{
	SERIAL_DBG,
	SERIAL_MODEM,	
	
	MAX_UARTS,
}UARTmap_t;

void UART_init(UARTmap_t uart, uint32_t baudrate);
void UART_configure(UARTmap_t uart, uint32_t baudrate);
uint8_t UART_read_line(UARTmap_t uart, uint8_t *read_buff, uint8_t max_len);
void UART_write_line(UARTmap_t uart, uint8_t *line);
bool UART_is_available(UARTmap_t uart);
//void UART_configure_callbacks(void);

#endif /* MY_UART_H_ */