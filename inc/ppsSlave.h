
#ifndef PPS_SLAVE_H
#define PPS_SLAVE_H

#include <stddef.h>
#include <stdint.h>
#include "ntl_uart.h"

#define Ucm_PpsSlave_ControlReg 0x00000000
#define Ucm_PpsSlave_StatusReg 0x00000004
#define Ucm_PpsSlave_PolarityReg 0x00000008
#define Ucm_PpsSlave_VersionReg 0x0000000C
#define Ucm_PpsSlave_PulseWidthReg 0x00000010
#define Ucm_PpsSlave_CableDelayReg 0x00000020

#define PPS_SLAVE_Version 0
#define PPS_SLAVE_EnableStatus 1
#define PPS_SLAVE_Polarity 2
#define PPS_SLAVE_InputOkStatus 3
#define PPS_SLAVE_PulseWidthValue 4
#define PPS_SLAVE_CableDelayValue 5
#define PPS_SLAVE_NUM_PROPS 6

int readPpsSlaveVersion(char *value, size_t size);
int readPpsSlaveEnableStatus(char *status, size_t size);
int readPpsSlavePolarity(char *status, size_t size);
int readPpsSlaveInputOkStatus(char *status, size_t size);
int readPpsSlavePulseWidthValue(char *value, size_t size);
int readPpsSlaveCableDelayValue(char *value, size_t size);
int writePpsSlaveCableDelayValue(char *cable_delay, size_t size);
int writePpsSlavePolarity(char *status, size_t size);
int writePpsSlaveEnableStatus(char *status, size_t size);

int readPpsSlaveAll(void);

int readWritePpsProperty(int prop, char *value);

typedef struct
{
    int64_t core_type;
    int64_t core_instance_nr;
    int64_t address_range_low;
    int64_t address_range_high;
    int64_t interrupt_mask;

    char Version[32];
    char InstanceNumber[32];
    char EnableStatus[32];
    char Polarity[32];
    char InputOkStatus[32];
    char PulseWidthValue[32];
    char CableDelayValue[32];
} PPS_SLAVE_T;

extern PPS_SLAVE_T PPS_SLAVE;
extern NTL_PROPERTY_T ppsProperties[PPS_SLAVE_NUM_PROPS];

#endif
