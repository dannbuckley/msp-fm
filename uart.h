/*
 * uart.h
 *
 * MSP430G2xx3 Demo - USCI_A0, UART Echo received character
 *                     (ACLK 9600/SMCLK 9600/SMCLK 115200)
 *
 *  Created on: Mar 3, 2020
 *      Author: Daniel Buckley
 */

#ifndef UART_H_
#define UART_H_

#define SMCLK_11500     0
#define SMCLK_9600      1
#define ACLK_9600       2

#define UART_MODE       SMCLK_11500

void initUART(void);

#endif /* UART_H_ */
