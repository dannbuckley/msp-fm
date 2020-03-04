#include <msp430.h>
#include "tea5767.h" // Library for TEA5767 radio
#include "i2c.h"

/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	init_TEA5767N();

	toggle_mute();
	toggle_stereo_noise_cancelling();
	select_channel(209, false);
	toggle_mute();

	while(true) {}
}
