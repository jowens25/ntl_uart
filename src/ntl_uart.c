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

int ntlWriteSuccess = 0;
char ntlBuff[100] = {0};

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
                parseResponse(ntlBuff, 0);
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

    // UART_Send(STDIO_UART, response, 64);

    for (int i = 0; i < 8; i++)
    {
        hexData[i] = response[i + 17];
    }

    hexData[8] = '\0';

    // printf("hex data: %s\n", hexData);
    *data = strtoull(hexData, NULL, 16);
    // printf("Read Response: %s \n", readData);

    return 0;
}

int ntlFordwardMessages(void)
{

    if (strncmp(&ntlBuff[0], "$CC", 3) == 0)
    {
        UART_Send(NTL_UART, ntlBuff, 50);
    }
    else if (strncmp(&ntlBuff[0], "$RC", 3) == 0)
    {
        UART_Send(NTL_UART, ntlBuff, 50);
    }
    else if (strncmp(&ntlBuff[0], "$WC", 3) == 0)
    {
        UART_Send(NTL_UART, ntlBuff, 50);
    }

    else if (strncmp(&ntlBuff[0], "$CR", 3) == 0)
    {
        UART_Send(STDIO_UART, ntlBuff, 50);
    }
    else if (strncmp(&ntlBuff[0], "$RR", 3) == 0)
    {
        UART_Send(STDIO_UART, ntlBuff, 50);
    }
    else if (strncmp(&ntlBuff[0], "$WR", 3) == 0)
    {
        UART_Send(STDIO_UART, ntlBuff, 50);
    }
    else if (strncmp(&ntlBuff[0], "$ER", 3) == 0)
    {
        UART_Send(STDIO_UART, ntlBuff, 50);
    }

    else
    {
        UART_Send(STDIO_UART, "NTL UART ERR\r\n", 50);
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

                parseResponse(ntlBuff, data);
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

                parseResponse(ntlBuff, data);
                ntlWriteSuccess = 1;
                timeOutAttempts = 0;
                ntlRspReceived = 0;
            }

            waitTicks(1);
        }
    }

    return 0;
}

#else

#include <stdint.h>
#include <stddef.h>
#include "clkClock.h"
#include "ntpServer.h"
#include "ppsSlave.h"
#include "ptpOc.h"
#include "todSlave.h"
#include "cores.h"

int64_t temp_data = 0x00000000;
int64_t temp_addr = 0x00000000;
char *FPGA_PORT = "/dev/ttyUSB0";

int ntlConnect(void)
{
    // printf("connect called\n");

    char connectCommand[] = "$CC*00\r\n";
    char writeData[64] = {0};
    char readData[64] = {0};

    int ser = serOpen(FPGA_PORT);
    if (ser == -1)
    {

        printf("connect Error opening serial port\n");
        return -1;
    }

    strcpy(writeData, connectCommand);

    int err = serWrite(ser, writeData, strlen(writeData));
    // usleep(2000); //

    if (err != 0)
    {
        printf("serWrite error\n");
        return -1;
    }

    err = serRead(ser, readData, sizeof(readData));
    if (err != 0)
    {
        printf("connect - serRead error\n");
        return -1;
    }

    // serClose(ser);

    if (isChecksumCorrect(readData) != 0)
    {
        printf("connect readData: %s\n", readData);
        printf("connect check sum wrong\n");
        return -1;
    }

    printf("Connect: Success\n");

    return 0;
}

int RawWrite(char *addr, char *data)
{
    char *err;
    long raw_addr = strtol(addr, &err, 16);

    if (err == addr || *err != '\0')
    {
        return -1;
    }

    long raw_data = strtol(data, &err, 16);

    if (err == data || *err != '\0')
    {
        return -1;
    }

    if (writeRegister(raw_addr, &raw_data) != 0)
    {
        return -1;
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

int isChecksumCorrect(char *message)
{
    char calculatedChecksum[64];
    char *messageChecksum;
    char *cmdAddressData;

    if (0 == strlen(message))
    {
        return -1;
    }

    cmdAddressData = strtok(message, "*");

    if (cmdAddressData == NULL)
    {
        return -2;
    }

    messageChecksum = strtok(NULL, "*"); // assign token to pointer then add a zero to end the string right after the two checksum digits

    if (messageChecksum == NULL)
    {
        return -3;
    }

    messageChecksum[2] = 0;

    // printf("cmd AddressData var: %s\n", cmdAddressData);

    sprintf(calculatedChecksum, "%02X", calculateChecksum(cmdAddressData)); // important formats checksum

    if (strncmp(calculatedChecksum, messageChecksum, 3) == 0)
    {
        return 0;
    }
    return -1;
}

int readRegister(int64_t addr, int64_t *data)
{
    char writeData[64] = {0};
    char readData[64] = {0};
    // char tempData[32] = {0};
    char hexAddr[64] = {0};
    char hexData[64] = {0};
    char hexChecksum[3] = {0};

    int ser = serOpen(FPGA_PORT);
    if (ser == -1)
    {

        printf("r Error opening serial port\n");
        return -1;
    }

    // build message
    strcat(writeData, "$RC,");

    sprintf(hexAddr, "0x%08lx", addr); // convert to hex string

    strcat(writeData, hexAddr);
    // printf("write data array: %s\n", writeData);

    char checksum = calculateChecksum(writeData);
    sprintf(hexChecksum, "%02X", checksum); // convert to hex string
    strcat(writeData, "*");

    strcat(writeData, hexChecksum);
    strcat(writeData, "\r\n");

    // printf("writeRegister: %s", writeData);

    // send message
    int err = serWrite(ser, writeData, strlen(writeData));
    if (err != 0)
    {
        printf("serWrite error");
        return -1;
    }

    // usleep(50000);
    //    receive message
    err = serRead(ser, readData, sizeof(readData));
    // printf("read data: %s \n", readData);

    if (err != 0)
    {
        printf("read - serRead error\n");
        return -1;
    }
    // close
    serClose(ser);

    if (isErrorResponse(readData))
    {
        printf("error response: %s \n", readData);
        return -1;
    }

    if (!isReadResponse(readData))
    {
        printf("missing read response\n");
        return -1;
    }

    if (isChecksumCorrect(readData) != 0)
    {
        printf("read reg - wrong checksum\n");
        return -1;
    }

    for (int i = 0; i < 8; i++)
    {
        hexData[i] = readData[i + 17];
    }

    hexData[8] = '\0';

    // printf("hex data: %s\n", hexData);
    *data = (int64_t)strtol(hexData, NULL, 16);
    // printf("Read Response: %s \n", readData);

    return 0;
}

int writeRegister(int64_t addr, int64_t *data)
{

    char writeData[64] = {0};
    char readData[64] = {0};
    // char tempData[32] = {0};
    char hexAddr[64] = {0};
    char hexData[64] = {0};
    char hexChecksum[3] = {0};

    int ser = serOpen(FPGA_PORT);
    if (ser == -1)
    {

        printf("w Error opening serial port\n");
        return -1;
    }

    // build message
    strcat(writeData, "$WC,");

    sprintf(hexAddr, "0x%08lx", addr); // convert to hex string

    strcat(writeData, hexAddr);

    strcat(writeData, ",");

    sprintf(hexData, "0x%08lx", *data);

    strcat(writeData, hexData);

    // printf("write data array: %s\n", writeData);

    char checksum = calculateChecksum(writeData);
    sprintf(hexChecksum, "%02X", checksum); // convert to hex string
    strcat(writeData, "*");

    strcat(writeData, hexChecksum);
    strcat(writeData, "\r\n");

    // printf("write data array: %s\n", writeData);

    // send message
    int err = serWrite(ser, writeData, strlen(writeData));
    if (err != 0)
    {
        printf("serWrite error \n");
        return -1;
    }

    // usleep(2000);
    //   receive message
    err = serRead(ser, readData, sizeof(readData));
    if (err != 0)
    {
        printf("write - serRead error \n");
        return -1;
    }
    // close
    // serClose(ser);

    if (isErrorResponse(readData))
    {
        printf("error response: %s", readData);
        return -1;
    }

    if (!isWriteResponse(readData))
    {

        printf("missing write response \n");
        // printf("read data: %s\n", readData);
        return -1;
    }

    if (isChecksumCorrect(readData) != 0)
    {
        printf("wrong checksum \n");
        return -1;
    }

    // printf("Write Response: %s \n", readData);

    return 0;
}

int isErrorResponse(char *message)
{
    if (strncmp("$ER", message, 3) == 0)
    {
        return 1;
    }

    return 0;
}

int isReadResponse(char *message)
{
    if (strncmp("$RR", message, 3) == 0)
    {
        return 1;
    }

    return 0;
}

int isWriteResponse(char *message)
{
    if (strncmp("$WR", message, 3) == 0)
    {
        return 1;
    }

    return 0;
}

struct termios tty;

int serOpen(char fileDescriptor[])
{
    int fd = open(fileDescriptor, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0)
    {
        printf("ser open error\n");
        return -1;
    }

    setupTermios(fd);

    return fd;
}

int serClose(int fileDescriptor)
{

    close(fileDescriptor);

    return 0;
}

int serRead(int ser, char data[], size_t dataLength)
{
    char temp;
    int index = 0;
    int totalRead = 0;
    int consecutiveTimeouts = 0;

    // memset(data, 0, dataLength);

    while (index < dataLength - 1)
    {
        int numRead = read(ser, &temp, 1);

        if (numRead < 0)
        {
            perror("serial read error");
            return -1;
        }
        else if (numRead == 0)
        {
            // Timeout - but maybe more data is coming
            consecutiveTimeouts++;
            if (consecutiveTimeouts > 5) // Give up after 5 timeouts
            {
                printf("serRead timeout after %d bytes\n", totalRead);
                break;
            }
            continue;
        }

        consecutiveTimeouts = 0; // Reset timeout counter
        totalRead++;

        // Check for line ending
        if (temp == '\n')
        {
            break; // Complete line received
        }
        else if (temp == '\r')
        {
            continue; // Skip \r, don't store it
        }

        data[index] = temp;
        index++;
    }

    data[index] = '\0';

    if (totalRead > 0)
    {
        // printf("Serial Read %d bytes: '%s'\n", totalRead, data);
    }

    return 0;
}

int serWrite(int ser, char data[], size_t dataLength)
{
    int numWrote = write(ser, data, dataLength);
    if (numWrote <= 0)
    {
        printf("serial write error\n");
        return -1;
    }
    // printf("Serial Write %d bytes: %s", numWrote, data);
    return 0;
}

int setupTermios(int fd)
{
    // printf("setup termios\n");
    memset(&tty, 0, sizeof tty);

    if (tcgetattr(fd, &tty) != 0)
    {
        printf("tcgetattr");
        close(fd);
        return -1;
    }

    cfsetospeed(&tty, B115200); // Use a standard baud rate unless you know otherwise
    cfsetispeed(&tty, B115200);

    // 8N1 configuration
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    // tty.c_cflag &= ~CRTSCTS; // Disable hardware flow control
    tty.c_cflag |= (CLOCAL | CREAD);

    // Input processing - disable all special processing
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);

    // Output processing - raw output
    tty.c_oflag &= ~OPOST;

    // CRITICAL FIX: Disable canonical mode for character-by-character reading
    tty.c_lflag &= ~ICANON; // Raw mode - read character by character
    tty.c_lflag &= ~(ECHO | ECHONL | ISIG | IEXTEN);

    // Timeout settings for raw mode
    tty.c_cc[VMIN] = 0;  // Don't wait for minimum characters
    tty.c_cc[VTIME] = 5; // Timeout in deciseconds (0.5 seconds)

    if (tcsetattr(fd, TCSANOW, &tty) != 0)
    {
        printf("tcsetattr error? \n");
        perror("tcsetattr");

        close(fd);
        return -1;
    }

    tcflush(fd, TCIOFLUSH);
    return 0;
}

#endif