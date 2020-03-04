#include <msp430.h>
#include "usci.h"       // USCI setup functions
#include "tea5767.h"    // Library for TEA5767 radio
#include "i2c.h"        // I2C initialization
#include "uart.h"       // UART initialization

/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	initClockTo16MHz();
	initGPIO();
	init_TEA5767N();

	toggle_mute();
	toggle_stereo_noise_cancelling();
	select_channel(209, false);
	toggle_mute();

	while(true) {}
}
