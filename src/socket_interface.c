
#include "common.h"
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
#include "socket_interface.h"
#include "ntl_uart.h"

int socket_fd = 0;

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
#ifndef IGNORE_READ_WRITE_DEFS
uint8_t read_reg(const uint32_t addr, uint32_t *data)
{
    int debug = 0;
    uint8_t data_length;
    // QByteArray temp_data;
    char temp_data[STRING_SIZE] = {0};
    char read_data[STRING_SIZE] = {0};
    char write_data[STRING_SIZE] = {0};
    char checksum;
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

    data_length = write(socket_fd, write_data, strlen(write_data));

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

    readSocket(socket_fd, read_data);

    if (debug)
    {
        printf("%s\n", read_data);
    }

    if (strncmp("$RR,0x", read_data, strlen("$RR,0x")) != 0)
    {
        printf("no correct response received\n");
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
        printf("checksum no matchy\n");

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
    uint8_t data_length;
    char temp_data[STRING_SIZE] = {0};
    char read_data[STRING_SIZE] = {0};
    char write_data[STRING_SIZE] = {0};
    char checksum;
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

    data_length = write(socket_fd, write_data, strlen(write_data));

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

    readSocket(socket_fd, read_data);

    if (debug)
    {
        printf("%s\n", read_data);
    }

    if (strncmp("$WR,0x", read_data, strlen("$WR,0x")) != 0)
    {
        printf("no correct response received\n");
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
        printf("checksum no matchy\n");

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

#endif

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