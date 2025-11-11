/*
 * ntl_uart.c
 *
 *  Created on: Nov 3, 2025
 *      Author: JacobOwens
 */



#include "ntl_uart.h"

#ifdef NTL_TIME_SERVER

volatile int ntlRspReceived = 0;
volatile int ntlCmdReceived = 0;
volatile int FORWARD_MESSAGES = 0;
int ntlWriteSuccess = 0;
char ntlCommandBuff[32] = {0};
char ntlResponseBuff[32] = {0};

int64_t temp_data = 0x00000000;
int64_t temp_addr = 0x00000000;

#define MAX_NUM_PROP 256
#define MAX_NUM_MODS 64
#define MAX_NUM_OPS 2 // 0 read // 1 write
#define Read 0
#define Write 1

int sendCommand(char *command, int64_t addr, int64_t *data)
{

    char writeData[64] = {0};
    char hexAddr[64] = {0};
    char hexChecksum[3] = {0};
    char hexData[64] = {0};

    strcat(writeData, command);

    if (strcmp(command, "$RC") == 0)
    {

        strcat(writeData, ",");

        sprintf(hexAddr, "0x%08lx", addr);

        strcat(writeData, hexAddr);
    }

    else if (strcmp(command, "$WC") == 0)
    {

        strcat(writeData, ",");

        sprintf(hexAddr, "0x%08lx", addr);

        strcat(writeData, hexAddr);

        strcat(writeData, ",");

        sprintf(hexData, "0x%08lx", *data);

        strcat(writeData, hexData);
    }

    char checksum = calculateChecksum(writeData);
    sprintf(hexChecksum, "%02X", checksum); // convert to hex string
    strcat(writeData, "*");

    strcat(writeData, hexChecksum);
    strcat(writeData, "\r\n");

    UART_Send(NTL_UART, writeData, strlen(writeData));

    return 0;
}

int ntlConnect(void)
{

    ntlWriteSuccess = 0;
    uint8_t i, timeOutAttempts = 3;

    while (ntlWriteSuccess == 0 && timeOutAttempts--)
    {
        sendCommand("$CC", 0, 0);

        /* wait for 0.1 seconds */
        for (i = 0; i < NTL_RESPONSE_TIMEOUT; i++)
        {

            if (ntlRspReceived)
            {
                parseResponse(ntlResponseBuff, 0);
                ntlWriteSuccess = 1;
                timeOutAttempts = 0;
                ntlRspReceived = 0;
            }

            waitTicks(1);
        }
    }

    if (ntlWriteSuccess)
    {

        return 0;
    }
    else
    {
    	UART_Send(STDIO_UART, "FPGA CONNECT FAILED\r\n", 100);

        return -1;
    }
}



int parseResponse(char *response, int64_t *data)
{

	ntlRspReceived = 0;
	ntlCmdReceived = 0;

    char hexData[64] = {0};

    if (strncmp(response, "$CR", 3) == 0)
    {
    	UART_Send(STDIO_UART, "FPGA CONNECTED\r\n", 20);

        return 0;
    }

    if (strncmp(response, "$ER", 3) == 0)
    {
    	UART_Send(STDIO_UART, "FPGA ERROR\r\n", 20);

        return 0;
    }


    for (int i = 0; i < 8; i++)
    {
        hexData[i] = response[i + 17];
    }

    hexData[8] = '\0';

    // printf("hex data: %s\n", hexData);
    *data = strtoull(hexData, NULL, 16);

    return 0;
}


int ntlForwardResponse(void) {
	if (ntlRspReceived) {
		ntlRspReceived = 0;
		UART_Send(STDIO_UART, ntlResponseBuff, strlen(ntlResponseBuff));
	}
	return 0;
}

int ntlForwardCommand(void) {
	if (ntlCmdReceived) {
		ntlCmdReceived = 0;
		UART_Send(NTL_UART, ntlCommandBuff, strlen(ntlCommandBuff));
	}
	return 0;
}


unsigned char calculateChecksum(char *data)
{
    // char out[3] = {0};

    unsigned char checksum = 0;
    for (int i = 1; i < strlen(data); i++)
    {
        if ('*' == data[i])
        {
            break;
        }
        checksum = checksum ^ data[i];
    }

    // sprintf(out, "%02X", checksum); // convert to two chars wide

    return checksum;
}


int readRegister(int64_t addr, int64_t *data)
{
    ntlWriteSuccess = 0;
    uint8_t i, timeOutAttempts = 3;
    while (ntlWriteSuccess == 0 && timeOutAttempts--)
    {
        sendCommand("$RC", addr, data);
        /* wait for 0.05 seconds */
        for (i = 0; i < NTL_RESPONSE_TIMEOUT; i++)
        {
            if (ntlRspReceived)
            {
                parseResponse(ntlResponseBuff, data);
                ntlWriteSuccess = 1;
                timeOutAttempts = 0;
                ntlRspReceived = 0;
            }
            waitTicks(1);
        }
    }
    return 0;
}


int writeRegister(int64_t addr, int64_t *data)
{
		ntlWriteSuccess = 0;
	    uint8_t i, timeOutAttempts = 3;
	    while (ntlWriteSuccess == 0 && timeOutAttempts--)
	    {
	        sendCommand("$WC", addr, data);
	        /* wait for 0.05 seconds */
	        for (i = 0; i < NTL_RESPONSE_TIMEOUT; i++)
	        {
	            if (ntlRspReceived)
	            {
	                parseResponse(ntlResponseBuff, data);
	                ntlWriteSuccess = 1;
	                timeOutAttempts = 0;
	                ntlRspReceived = 0;
	            }
	            waitTicks(1);
	        }
	    }
    return 0;
}


#endif




