/**
 * \file
 *
 * \brief User board initialization template
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include <asf.h>
#include <board.h>
#include <conf_board.h>
#include "user_board.h"

#if defined(__GNUC__)
void board_init(void) WEAK __attribute__((alias("system_board_init")));
#elif defined(__ICCARM__)
void board_init(void);
#  pragma weak board_init=system_board_init
#endif

void system_board_init(void)
{
	/* This function is meant to contain board-specific initialization code
	 * for, e.g., the I/O pins. The initialization can rely on application-
	 * specific board configuration, found in conf_board.h.
	 */
	
	struct port_config pin_conf; // create a strut to hold defaults
	port_get_config_defaults(&pin_conf);
		
	/* Configure LEDs as outputs, turn them off */
	pin_conf.direction = PORT_PIN_DIR_OUTPUT; // set pin to output
	port_pin_set_config(USR_LED1, &pin_conf); // configure pin direction
	port_pin_set_config(USR_LED2, &pin_conf); // configure pin direction
	port_pin_set_output_level(USR_LED1, LED_INACTIVE); // set pin output level low
	port_pin_set_output_level(USR_LED2, LED_INACTIVE); // set pin output level low
		
	/* Set buttons as inputs */
	pin_conf.direction = PORT_PIN_DIR_INPUT; // set pin direction as input
	pin_conf.input_pull = PORT_PIN_PULL_UP; // set has a pull up
	port_pin_set_config(USR_BUT, &pin_conf); // configure pin
	
	/* Configure PWR KEY for modem */
	pin_conf.direction = PORT_PIN_DIR_OUTPUT; // set pin to output
	port_pin_set_config(PWRKEY, &pin_conf); // configure pin direction	
	port_pin_set_output_level(PWRKEY, 0); // set pin output level low
	
	/* Configure RTS for modem */
	pin_conf.direction = PORT_PIN_DIR_OUTPUT; // set pin to output
	port_pin_set_config(MODEM_RTS, &pin_conf); // configure pin direction
	port_pin_set_output_level(MODEM_RTS, 1); // set pin output level high
	
	/* Configure CTS for modem */
	pin_conf.direction = PORT_PIN_DIR_INPUT; // set pin direction as input
	pin_conf.input_pull = PORT_PIN_PULL_UP; // set has a pull up
	port_pin_set_config(MODEM_CTS, &pin_conf); // configure pin	
	
	// Clock SERCOM for Serial
	PM->APBCMASK.reg |= PM_APBCMASK_SERCOM0 | PM_APBCMASK_SERCOM1 | PM_APBCMASK_SERCOM2 | PM_APBCMASK_SERCOM3 | PM_APBCMASK_SERCOM4 | PM_APBCMASK_SERCOM5 ;

	// Clock TC/TCC for Pulse and Analog
	PM->APBCMASK.reg |= PM_APBCMASK_TCC0 | PM_APBCMASK_TCC1 | PM_APBCMASK_TCC2 | PM_APBCMASK_TC3 | PM_APBCMASK_TC4 | PM_APBCMASK_TC5 ;

	// Clock ADC/DAC for Analog
	PM->APBCMASK.reg |= PM_APBCMASK_ADC | PM_APBCMASK_DAC ;
	
	// Initialize Analog Controller
	// Setting clock
	while(GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);

	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID( GCM_ADC ) | // Generic Clock ADC
	GCLK_CLKCTRL_GEN_GCLK0     | // Generic Clock Generator 0 is source
	GCLK_CLKCTRL_CLKEN ;

	while( ADC->STATUS.bit.SYNCBUSY == 1 );          // Wait for synchronization of registers between the clock domains

	ADC->CTRLB.reg = ADC_CTRLB_PRESCALER_DIV512 |    // Divide Clock by 512.
	ADC_CTRLB_RESSEL_10BIT;         // 10 bits resolution as default

	ADC->SAMPCTRL.reg = 0x3f;                        // Set max Sampling Time Length

	while( ADC->STATUS.bit.SYNCBUSY == 1 );          // Wait for synchronization of registers between the clock domains

	ADC->INPUTCTRL.reg = ADC_INPUTCTRL_MUXNEG_GND;   // No Negative input (Internal Ground)

	// Averaging (see datasheet table in AVGCTRL register description)
	ADC->AVGCTRL.reg = ADC_AVGCTRL_SAMPLENUM_1 |    // 1 sample only (no oversampling nor averaging)
	ADC_AVGCTRL_ADJRES(0x0ul);   // Adjusting result by 0

	//analogReference( AR_DEFAULT ) ; // Analog Reference is AREF pin (3.3v)

	// Initialize DAC
	// Setting clock
	while ( GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY );
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID( GCM_DAC ) | // Generic Clock ADC
	GCLK_CLKCTRL_GEN_GCLK0     | // Generic Clock Generator 0 is source
	GCLK_CLKCTRL_CLKEN ;

	while ( DAC->STATUS.bit.SYNCBUSY == 1 ); // Wait for synchronization of registers between the clock domains
	DAC->CTRLB.reg = DAC_CTRLB_REFSEL_AVCC | // Using the 3.3V reference
	DAC_CTRLB_EOEN ;        // External Output Enable (Vout)	
}