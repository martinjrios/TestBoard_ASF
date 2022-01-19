/**
 * \file
 *
 * \brief User board definition template
 *
 */

 /* This file is intended to contain definitions and configuration details for
 * features and devices that are available on the board, e.g., frequency and
 * startup time for an external crystal, external memory devices, LED and USART
 * pins.
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef USER_BOARD_H
#define USER_BOARD_H

#include <conf_board.h>
#include "..\port\port.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup group_common_boards
 * \defgroup user_board_group User board
 *
 * @{
 */

void system_board_init(void);

/** Name string macro */
#define BOARD_NAME                "USER_BOARD"

/** @} */

#define USR_BUT			PIN_PA11
#define USR_LED1		PIN_PB03
#define USR_LED2		PIN_PA27
#define PWRKEY			PIN_PA08
#define RESET_N			PIN_PA09
#define MODEM_RTS		PIN_PA14
#define MODEM_CTS		PIN_PA15

#define LED_ACTIVE		0
#define LED_INACTIVE	1

#ifdef __cplusplus
}
#endif

#endif // USER_BOARD_H
