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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <syslog.h>
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"
#else
#include "uartCircBuff.h"
#include "uart.h"
#endif

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

void fpga_read_all(void)
{
    clk_clock_read_values(&ntlts);
    tod_slave_read_values(&ntlts);
    pps_slave_read_values(&ntlts);
    ptp_oc_read_values(&ntlts);
    ntp_server_read_values(&ntlts);
}

void fpga_write_all(uint8_t fromRegisters)
{
    clk_clock_write_values(&ntlts, fromRegisters);
    tod_slave_write_values(&ntlts, fromRegisters);
    pps_slave_write_values(&ntlts, fromRegisters);
    ptp_oc_write_values(&ntlts, fromRegisters);
    ntp_server_write_values(&ntlts, fromRegisters);
}

void NTL_COM_HANDLER(char *temp_rsp, uint32_t rsp_size)
{

    gpntlBuff[sizeof(gpntlBuff) - 1] = '\0';
    char *mod = strtok(gpntlBuff, ","); // return "NTP"
    char *prop = strtok(NULL, ",");     // return "IP"
    char *val = strtok(NULL, ",");      // returns ? or value

    if (memcmp(mod, "CLK", 3) == 0)
    {
    }
    else if (memcmp(mod, "TOD", 3) == 0)
    {
    }
    else if (memcmp(mod, "PPS", 3) == 0)
    {
    }
    else if (memcmp(mod, "PTP", 3) == 0)
    {
    }
    else if (memcmp(mod, "NTP", 3) == 0)
    {

        ntp_handler(temp_rsp, rsp_size, prop, val);
    }
}

void ntp_handler(char *temp_rsp, int rsp_size, const char *prop, char *val)
{
    int write = 0;
    int err = 0;
    if (val != NULL)
    {
        val[strcspn(val, "\r\n")] = 0; // remove \r\n
        write = 1;
    }

    if (memcmp(prop, "IP", 2) == 0)
    {
        if (write)
        {
            memcpy(ntlts.ntpServer.ipAddr, val, sizeof(ntlts.ntpServer.ipAddr));
        }

        // return ram value
        memcpy(temp_rsp, ntlts.ntpServer.ipAddr, rsp_size);
    }

    if (memcmp(prop, "MAC", 3) == 0)
    {
        if (write)
        {
            memcpy(ntlts.ntpServer.MacAddr, val, sizeof(ntlts.ntpServer.MacAddr));
        }

        // return ram value
        memcpy(temp_rsp, ntlts.ntpServer.MacAddr, rsp_size);
    }

    if (write)
    {
        err = ntp_server_write_values(&ntlts, 0);
        if (err != 0){
        	snprintf(temp_rsp, rsp_size, "NTP_WRITE_ERR: %d", err);
        }
        write = 0;
    }
}

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
