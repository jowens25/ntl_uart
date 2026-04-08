#ifndef SOCKET_INTERFACE_H
#define SOCKET_INTERFACE_H
#include <stdint.h>
#include "main.h"
extern int socket_fd;

int setup_socket(void);
void readSocket(int socket_fd, char *msg);
#ifndef IGNORE_READ_WRITE_DEFS

uint8_t read_reg(const uint32_t addr, uint32_t *data);
uint8_t write_reg(const uint32_t addr, uint32_t *data);
#endif
#endif