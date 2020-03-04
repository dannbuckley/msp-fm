/*
 * uart.c
 *
 *  Created on: Mar 3, 2020
 *      Author: Daniel Buckley
 */

//   MSP430G2xx3 Demo - USCI_A0, UART Echo received character
//                     (ACLK 9600/SMCLK 9600/SMCLK 115200)
//
//   Description: The device will wait in LPM0/LPM3 (based on clock source)
//   until a UART character is received.
//   Then the device will echo the received character.
//   The UART can operate using ACLK at 9600, SMCLK at 115200 or SMCLK at 9600.
//   To configure the UART mode, change the following line:
//
//      #define UART_MODE       SMCLK_11500
//      to any of:
//      #define UART_MODE       SMCLK_11500
//      #define UART_MODE       SMCLK_9600
//      #define UART_MODE       ACLK_9600
//
//   UART RX ISR is used to handle communication.
//   ACLK = 32.768kHz, MCLK = SMCLK = DCO 16MHz.
//
//
//                   MSP430G2553
//                 -----------------
//            /|\ |             P2.6|--- XIN
//             |  |                 |   |
//             ---|RST              |   32kHz
//                |                 |   |
//                |             P2.7|--- XOUT
//                |             P1.1|<------- Receive Data (UCA0RXD)
//                |             P1.2|-------> Transmit Data (UCA0TXD)
//                |                 |
//                |                 |
//
//   Nima Eskandari
//   Texas Instruments Inc.
//   April 2017
//   Built with CCS V7.0

#include <msp430.h>
#include <stdint.h>
#include "uart.h"

void initUART()
{
#if UART_MODE == SMCLK_11500
    UCA0CTL1 |= UCSSEL_2;                     // SMCLK
    UCA0BR0 = 138;                            // 16MHz 115200
    UCA0BR1 = 0;                              // 16MHz 115200
    UCA0MCTL = UCBRS_7;                       // Modulation UCBRSx = 7
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    IFG2 &= ~(UCA0RXIFG);
    IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
#elif UART_MODE == SMCLK_9600
    UCA0CTL1 |= UCSSEL_2;                     // SMCLK
    UCA0BR0 = 104;                            // 16MHz 9600
    UCA0BR1 = 0;                              // 16MHz 9600
    UCA0MCTL = UCBRS_0 + UCOS16 + UCBRF_3;    // Modulation UCBRSx = 0
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    IFG2 &= ~(UCA0RXIFG);
    IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
#elif UART_MODE == ACLK_9600
    UCA0CTL1 |= UCSSEL_1;                     // ACLK
    UCA0BR0 = 3;                              // 32768Hz 9600
    UCA0BR1 = 0;                              // 32768Hz 9600
    UCA0MCTL = UCBRS_3;                       // Modulation UCBRSx = 3
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    IFG2 &= ~(UCA0RXIFG);
    IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
#else
    #error "Select UART Baud Rate of 115200 or 9600"
#endif
}
