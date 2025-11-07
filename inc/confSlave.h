
#ifndef CONF_SLAVE_H

#define CONF_SLAVE_H
#include <stddef.h>
#include <stdint.h>
#define CoreTypeAndInstanceReg 0x00000000
#define CoreAddrLowReg 0x00000004
#define CoreAddrHighReg 0x00000008
#define CoreIrqMaskReg 0x0000000C

typedef struct
{
    int64_t core_type;
    int64_t core_instance_nr;
    int64_t address_range_low;
    int64_t address_range_high;
    int64_t interrupt_mask;
} CONF_SLAVE_T;

extern CONF_SLAVE_T CONF_SLAVE;

#endif // CONF_SLAVE_H
