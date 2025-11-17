/*
 * ntl_uart.c
 *
 *  Created on: Nov 3, 2025
 *      Author: JacobOwens
 */

#include "ntl_uart.h"
#include "clkClock.h"
#include "confSlave.h"
#include "ntpServer.h"
#include "ppsSlave.h"
#include "ptpOc.h"
#include "todSlave.h"

#ifdef NTL_TIME_SERVER

volatile int ntlRspReceived = 0;
volatile int ntlCmdReceived = 0;
volatile int FORWARD_MESSAGES = 0;
int ntlWriteSuccess = 0;
int ntlOperation = 0; // 0 = read, 1 = write
int ntlModule = 0;    // ntl type
int ntlProperty = 0;
int updated = 0;
char ntlCommandBuff[32] = {0};
char ntlResponseBuff[32] = {0};

char ntlCmd[64] = {0};
char ntlRsp[64] = {0};
char *ntlValue;

int64_t temp_data = 0x00000000;
int64_t temp_addr = 0x00000000;

#define MAX_NUM_PROP 256
#define MAX_NUM_MODS 64
#define MAX_NUM_OPS 2 // 0 read // 1 write
#define Read 0
#define Write 1

int readOnly(char *ro, size_t size)
{
    snprintf(ro, size, "READ-ONLY");

    return 0;
}

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
        sprintf(hexAddr, "0x%08llx", addr);
        strcat(writeData, hexAddr);
        strcat(writeData, ",");
        sprintf(hexData, "0x%08llx", *data);
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

int ntlForwardResponse(void)
{
    if (ntlRspReceived)
    {
        ntlRspReceived = 0;
        UART_Send(STDIO_UART, ntlResponseBuff, strlen(ntlResponseBuff));
    }
    return 0;
}

int ntlForwardCommand(void)
{
    if (ntlCmdReceived)
    {
        ntlCmdReceived = 0;

        if (memcmp((void *)&ntlCommandBuff[0], "$GPNTL", 4) == 0)
        {
            NTL_COM_HANDLER();
        }
        else
        {
            UART_Send(NTL_UART, ntlCommandBuff, strlen(ntlCommandBuff));
        }
    }
    return 0;
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

unsigned char calculateChecksum(char *data)
{
    unsigned char checksum = 0;
    for (int i = 1; i < strlen(data); i++)
    {
        if ('*' == data[i])
        {
            break;
        }
        checksum = checksum ^ data[i];
    }
    return checksum;
}
//               0 or 1   0...22  0...50
//         0123456789
//         $GPNTL,0,22,5,10.1.10.205
// expects $GPNTL,OPERATION,MODULE,PROPERTY,VALUE
void NTL_COM_HANDLER(void)
{

    char *tok;
    int field = 0;

    strncpy(ntlCmd, ntlCommandBuff, sizeof(ntlCmd));

    ntlCmd[sizeof(ntlCmd) - 1] = '\0';

    tok = strtok(ntlCmd, ","); // returns $GPNTL

    tok = strtok(NULL, ","); // returns modules
    ntlModule = strtol(tok, NULL, 10);

    tok = strtok(NULL, ","); // returns prop
    ntlProperty = strtol(tok, NULL, 10);

    ntlValue = NULL;
    ntlValue = strtok(NULL, ","); // value

    // ntlValue[strcspn(str, "\r\n")] = '\0';

    switch (ntlModule)
    {
    case Ucm_CoreConfig_TodSlaveCoreType:

        readWriteProperty(Ucm_CoreConfig_TodSlaveCoreType, todProperties, ntlProperty, ntlValue);

        break;

    case Ucm_CoreConfig_PtpOrdinaryClockCoreType:

        readWriteProperty(Ucm_CoreConfig_PtpOrdinaryClockCoreType, ptpProperties, ntlProperty, ntlValue);

        break;

    case Ucm_CoreConfig_PpsSlaveCoreType:

        readWriteProperty(Ucm_CoreConfig_PpsSlaveCoreType, ppsProperties, ntlProperty, ntlValue);

        // readWritePpsProperty(ntlProperty, ntlValue);

        break;

    case Ucm_CoreConfig_NtpServerCoreType:

        // readWriteNtpProperty(ntlProperty, ntlValue);

        readWriteProperty(Ucm_CoreConfig_NtpServerCoreType, ntpProperties, ntlProperty, ntlValue);

        break;

    case Ucm_CoreConfig_ConfSlaveCoreType: // remove?

        // readWriteProperty(Ucm_CoreConfig_ConfSlaveCoreType, confSlaveProperties, ntlProperty, ntlValue);

        break;

    case Ucm_CoreConfig_ClkClockCoreType:

        readWriteProperty(Ucm_CoreConfig_ClkClockCoreType, clkProperties, ntlProperty, ntlValue);

        break;

    case 0:
        int err = ntlConnect();
        if (err != 0)
        {
            snprintf(ntlRsp, 31, "$GPNTL,ERR,CONNECTION FAILED\r\n");
            UART_Send(STDIO_UART, ntlRsp, strlen(ntlRsp));
        }
        err = getCores();
        if (err != 0)
        {
            snprintf(ntlRsp, 30, "$GPNTL,ERR,GET_CORES FAILED\r\n");
            UART_Send(STDIO_UART, ntlRsp, strlen(ntlRsp));
        }

        snprintf(ntlRsp, 15, "$GPNTL,CC,GC\r\n");
        UART_Send(STDIO_UART, ntlRsp, strlen(ntlRsp));
        break;
    }
}

int readWriteProperty(int core_type, NTL_PROPERTY_T *properties, int p, char *newValue)
{
    size_t size = 64;

    if (properties[p].read == NULL || properties[p].write == NULL || properties[p].value == NULL)
    {
        snprintf(ntlRsp, size, "$GPNTL,%d,%d,ERR\r\n", core_type, p);
        UART_Send(STDIO_UART, ntlRsp, strlen(ntlRsp));
        return -1;
    }

    // read fpga, return struct
    if (strncmp(newValue, "?", 1) == 0)
    {
        properties[p].read(properties[p].value, size);
    }

    // write new value to fpga, read fpga, populate struct
    else if (strlen(newValue) != 0)
    {

        properties[p].write(newValue, size);

        if(strcmp(newValue, "READ-ONLY") != 0) {
            properties[p].read(properties[p].value, size);
        } else {
            snprintf(ntlRsp, size, "$GPNTL,%d,%d,%s\r\n", core_type, p, newValue);
            UART_Send(STDIO_UART, ntlRsp, strlen(ntlRsp));
            return 0;
        }
    }

    snprintf(ntlRsp, size, "$GPNTL,%d,%d,%s\r\n", core_type, p, properties[p].value);
    UART_Send(STDIO_UART, ntlRsp, strlen(ntlRsp));

    return 0;
}

#endif
