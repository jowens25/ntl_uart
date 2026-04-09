/*
 * ntl_uart.c
 *
 *  Created on: Nov 3, 2025
 *      Author: JacobOwens
 */

#include "ntl_uart.h"

#ifdef USE_SOCKET

#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/select.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <syslog.h>

#else
#include "uartCircBuff.h"
#endif

#include "stdlib.h"
#include "string.h"
#include "stdbool.h"

volatile int ntlRspReceived = 0;
volatile int ntlCmdReceived = 0;

volatile uint32_t cycles = 10;

volatile int FORWARD_MESSAGES = 0;
int ntlWriteSuccess = 0;
int ntlOperation = 0; // 0 = read, 1 = write
int ntlModule = 0;    // ntl type
int ntlProperty = 0;
int updated = 0;
char ntlCommandBuff[32] = {0};
char ntlResponseBuff[32] = {0};

char ntlCmd[64] = {0};
char gpntlBuff[64] = {0};
char ntlRsp[64] = {0};
char *ntlValue;

int get_length_by_char(char *myString, char *myChar)
{
    int max = 256;
    char *start = myString;
    char *end = myString;

    if (myString == NULL || myChar == NULL)
    {
        return -1;
    }

    while (*end != '\0' && *end != *myChar)
    {
        end++;

        if (end - start >= max)
        {
            return -1; // not found within max length
        }
    }

    // Check if we found the character or hit null terminator
    if (*end != *myChar)
    {
        return -1; // character not found
    }

    return end - start;
}

#ifndef USE_SOCKET

void NTL_COM_HANDLER(void)
{

    // GPNTL,op,module,data

    if (strncmp(gpntlBuff, "NTPGET", 6) == 0)
    {

        char buff[64] = {0};

        snprintf(buff, 63, "%s\r\n", ntlts.ntpServer.ipAddr);

        UART0_SendBuff(buff, 64);
        return;
    }

    char *tok;
    // int field = 0;

    gpntlBuff[sizeof(gpntlBuff) - 1] = '\0';

    tok = strtok(gpntlBuff, ","); // returns $GPNTL

    tok = strtok(NULL, ",");
    ntlOperation = strtol(tok, NULL, 10);

    tok = strtok(NULL, ","); // module
    ntlModule = strtol(tok, NULL, 10);

    ntlValue = NULL;
    ntlValue = strtok(NULL, ","); // value

    switch (ntlModule)
    {
    case Ucm_CoreConfig_TodSlaveCoreType:
        break;

    case Ucm_CoreConfig_PtpOrdinaryClockCoreType:
        break;

    case Ucm_CoreConfig_PpsSlaveCoreType:
        break;

    case Ucm_CoreConfig_NtpServerCoreType:

        ntp_server_read_values(&ntlts);

        break;

    case Ucm_CoreConfig_ConfSlaveCoreType: // remove?
        break;

    case Ucm_CoreConfig_ClkClockCoreType:
        break;

    case 0:

        /* int err = ntlConnect();
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
        */
    }
}
#endif

uint8_t read_reg(const uint32_t addr, uint32_t *data)
{
    int debug = 0;
    // uint8_t data_length;
    char temp_data[STRING_SIZE] = {0};
    char read_data[STRING_SIZE] = {0};
    char write_data[STRING_SIZE] = {0};
    uint8_t checksum;
    char temp_string[STRING_SIZE] = {0};

    char hexAddr[STRING_SIZE] = {0};
    char hexChecksum[STRING_SIZE] = {0};

    strncat(write_data, "$RC,", strlen("$RC,") + 1);

    sprintf(hexAddr, "0x%08x", addr);
    strncat(write_data, hexAddr, strlen(hexAddr));

    checksum = 0;
    for (uint8_t i = 1; i < strlen(write_data); i++)
    {
        checksum = checksum ^ write_data[i];
    }
    strncat(write_data, "*", strlen("*") + 1);

    sprintf(hexChecksum, "%02X", checksum); // convert to hex string
    strncat(write_data, hexChecksum, strlen(hexChecksum) + 1);

    strncat(write_data, "\r", 2);
    strncat(write_data, "\n", 2);

    if (debug)
    {
        printf("%s\n", write_data);
    }

#ifdef USE_SOCKET
    int data_length = write(socket_fd, write_data, strlen(write_data));

    if (data_length == -1)
    {
        printf("write failed\n");
        return -1;
    }
    else if (data_length != strlen(write_data))
    {
        printf("write incomplete\n");
        return -1;
    }
#else // UART
    UART2_SendBuff(write_data, strlen(write_data));
#endif

#ifdef USE_SOCKET
    readSocket(socket_fd, read_data);

#else // UART
    cycles = 50000;
    while (cycles--)
        ;

    if (ntlRspReceived)
    {
        ntlRspReceived = 0;
        memcpy(read_data, ntlResponseBuff, strlen(ntlResponseBuff));
    }
    else
    {

        UART2_SendBuff("NO RSP\r\n", strlen("NO RSP\r\n"));
    }
#endif

    if (debug)
    {
        printf("%s\n", read_data);
    }

    if (strncmp("$RR,0x", read_data, strlen("$RR,0x")) != 0)
    {
        // printf("no correct response received\n");
        return -1;
    }

    checksum = 0;
    for (int i = 1; i < strlen(read_data); i++)
    {
        if (read_data[i] == '*')
        {
            break;
        }
        else
        {
            checksum = checksum ^ read_data[i];
        }
    }

    memset(temp_data, 0, sizeof(temp_data));

    sprintf(hexChecksum, "%02X", checksum); // convert to hex string
    strncat(temp_data, hexChecksum, strlen(hexChecksum) + 1);

    strncat(temp_data, "\r", 2);
    strncat(temp_data, "\n", 2);

    int offset = (strlen(read_data) - strlen(temp_data));

    if (strncmp(read_data + offset, temp_data, strlen(temp_data)) != 0)
    {
        // printf("checksum no matchy\n");

        return -1;
    }

    for (int i = 0; i < 8; i++)
    {
        temp_string[i] = read_data[i + 17];
    }

    temp_string[8] = '\0';

    *data = strtoull(temp_string, NULL, 16);

    return 0;
}

uint8_t write_reg(const uint32_t addr, uint32_t *data)
{
    int debug = 0;
    // uint8_t data_length;
    char temp_data[STRING_SIZE] = {0};
    char read_data[STRING_SIZE] = {0};
    char write_data[STRING_SIZE] = {0};
    uint8_t checksum;
    // char temp_string[STRING_SIZE] = {0};

    char hexAddr[STRING_SIZE] = {0};
    char hexData[STRING_SIZE] = {0};
    char hexChecksum[STRING_SIZE] = {0};

    strncat(write_data, "$WC,", strlen("$WC,") + 1);

    sprintf(hexAddr, "0x%08x", addr);
    strncat(write_data, hexAddr, strlen(hexAddr));

    strncat(write_data, ",", strlen(",") + 1);

    sprintf(hexData, "0x%08x", *data);
    strncat(write_data, hexData, strlen(hexData));

    checksum = 0;
    for (uint8_t i = 1; i < strlen(write_data); i++)
    {
        checksum = checksum ^ write_data[i];
    }
    strncat(write_data, "*", strlen("*") + 1);

    sprintf(hexChecksum, "%02X", checksum); // convert to hex string
    strncat(write_data, hexChecksum, strlen(hexChecksum) + 1);

    strncat(write_data, "\r", 2);
    strncat(write_data, "\n", 2);

    if (debug)
    {
        printf("%s\n", write_data);
    }

#ifdef USE_SOCKET
    int data_length = write(socket_fd, write_data, strlen(write_data));

    if (data_length == -1)
    {
        printf("write failed\n");
        return -1;
    }
    else if (data_length != strlen(write_data))
    {
        printf("write incomplete\n");
        return -1;
    }
#else // UART
    UART2_SendBuff(write_data, strlen(write_data));
#endif

#ifdef USE_SOCKET
    readSocket(socket_fd, read_data);
#else // UART
    cycles = 50000;
    while (cycles--)
        ;

    if (ntlRspReceived)
    {
        ntlRspReceived = 0;
        memcpy(read_data, ntlResponseBuff, strlen(ntlResponseBuff));
    }
    else
    {

        UART2_SendBuff("NO RSP\r\n", strlen("NO RSP\r\n"));
    }
#endif

    if (debug)
    {
        printf("%s\n", read_data);
    }

    if (strncmp("$WR,0x", read_data, strlen("$WR,0x")) != 0)
    {
        // printf("no correct response received\n");
        return -1;
    }

    checksum = 0;
    for (int i = 1; i < strlen(read_data); i++)
    {
        if (read_data[i] == '*')
        {
            break;
        }
        else
        {
            checksum = checksum ^ read_data[i];
        }
    }

    memset(temp_data, 0, sizeof(temp_data));

    sprintf(hexChecksum, "%02X", checksum); // convert to hex string
    strncat(temp_data, hexChecksum, strlen(hexChecksum) + 1);

    strncat(temp_data, "\r", 2);
    strncat(temp_data, "\n", 2);

    int offset = (strlen(read_data) - strlen(temp_data));

    if (strncmp(read_data + offset, temp_data, strlen(temp_data)) != 0)
    {
        // printf("checksum no matchy\n");

        return -1;
    }

    // for (int i = 0; i < 8; i++)
    //{
    //     temp_string[i] = read_data[i + 17];
    // }
    //
    // temp_string[8] = '\0';
    //
    //*data = strtoull(temp_string, NULL, 16);

    return 0;
}

#ifdef USE_SOCKET

int socket_fd = 0;
#define CHUNK_SIZE 32

void readSocket(int socket_fd, char *msg)
{
    char out[STRING_SIZE] = {0};

    fd_set read_fds;

    for (int i = 0; i < 50; i++)
    {

        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 100000; // 100ms

        FD_ZERO(&read_fds);
        FD_SET(socket_fd, &read_fds);

        int status = select(socket_fd + 1, &read_fds, NULL, NULL, &tv);
        if (status == -1)
        {
            exit(4);
        }
        else
        {
            if (!status)
            {
                printf("timeout exiting\n");
                break;
                continue;
            }
        }

        if (FD_ISSET(socket_fd, &read_fds))
        {

            int n = read(socket_fd, msg, CHUNK_SIZE);

            msg[n] = '\0';
            strncat(out, msg, strlen(msg));

            if (msg[n - 1] == '\n')
            {
                // printf("%s", out);
                strncpy(msg, out, strlen(out));
                break;
            }
        }
    }
}

void set_nonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags != -1)
        fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int setup_socket()
{
    // setup socket
    int socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, "/var/lib/ns/ns-serial-mux.sock", sizeof(addr.sun_path) - 1);

    connect(socket_fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un));

    set_nonblocking(socket_fd);

    return socket_fd;
}

#endif