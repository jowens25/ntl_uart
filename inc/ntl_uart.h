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

#ifndef __linux__
#include <headers.h>
#endif

#ifdef __linux__
#include <fcntl.h>   // linux
#include <unistd.h>  // linux
#include <termios.h> // linux
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <stddef.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

extern struct termios tty;

int axiConnect(void);

int serOpen(char fileDescriptor[]);
int serClose(int fileDescriptor);

int serRead(int ser, char data[], size_t dataLength);
int serWrite(int ser, char data[], size_t dataLength);

unsigned char calculateChecksum(char *data);

int isWriteResponse(char *message);
int isReadResponse(char *message);
int isErrorResponse(char *message);
int isChecksumCorrect(char *message);

int setupTermios(int);

int readRegister(int64_t addr, int64_t *data);
int writeRegister(int64_t addr, int64_t *data);
int RawWrite(char *addr, char *data);

// int AxiRead(char *core, char *property, char *value);
int axiRead(char *core, char *property, char *value);
int axiWrite(char *core, char *property, char *value);

extern int64_t temp_data;
extern int64_t temp_addr;

typedef int (*read_write_func)(char *value, size_t size);

extern char *FPGA_PORT;

#endif /* MAIN_INC_NTL_UART_H_ */

#endif