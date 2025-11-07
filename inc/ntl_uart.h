/*
 * ntl_uart.h
 *
 *  Created on: Nov 3, 2025
 *      Author: JacobOwens
 */

#ifndef MAIN_INC_NTL_UART_H_
#define MAIN_INC_NTL_UART_H_
#include <stdint.h>
#include <string.h>
#include <stdio.h>


#include <headers.h>


#include "cores.h"
#include "clkClock.h"
#include "confSlave.h"
#include "ntl_uart.h"
#include "ppsSlave.h"
#include "ptpOc.h"
#include "todSlave.h"
#include "ntpServer.h"

extern volatile int ntlRspReceived;
extern volatile int ntlCmdReceived;

extern char ntlBuff[100];
int ntlFordwardMessages(void);
extern int ntlWriteSuccess;

extern int64_t temp_data;
extern int64_t temp_addr;

#define NTL_RESPONSE_TIMEOUT 5

int sendCommand(char *command, int64_t addr, int64_t *data);
int readRegister(int64_t addr, int64_t *data);
int writeRegister(int64_t addr, int64_t *data);
int parseResponse(char *response, int64_t *data);
unsigned char calculateChecksum(char *data);

int ntlConnect(void);

#endif /* MAIN_INC_NTL_UART_H_ */
