#ifndef CLK_CLOCK_H

#define CLK_CLOCK_H
#include <stddef.h>
#include <stdint.h>
#include "ntl_uart.h"

#define Ucm_ClkClock_ControlReg 0x00000000
#define Ucm_ClkClock_StatusReg 0x00000004
#define Ucm_ClkClock_SelectReg 0x00000008
#define Ucm_ClkClock_VersionReg 0x0000000C
#define Ucm_ClkClock_TimeValueLReg 0x00000010
#define Ucm_ClkClock_TimeValueHReg 0x00000014
#define Ucm_ClkClock_TimeAdjValueLReg 0x00000020
#define Ucm_ClkClock_TimeAdjValueHReg 0x00000024
#define Ucm_ClkClock_OffsetAdjValueReg 0x00000030
#define Ucm_ClkClock_OffsetAdjIntervalReg 0x00000034
#define Ucm_ClkClock_DriftAdjValueReg 0x00000040
#define Ucm_ClkClock_DriftAdjIntervalReg 0x00000044
#define Ucm_ClkClock_InSyncThresholdReg 0x00000050
#define Ucm_ClkClock_ServoOffsetFactorPReg 0x00000060
#define Ucm_ClkClock_ServoOffsetFactorIReg 0x00000064
#define Ucm_ClkClock_ServoDriftFactorPReg 0x00000068
#define Ucm_ClkClock_ServoDriftFactorIReg 0x0000006C
#define Ucm_ClkClock_StatusOffsetReg 0x00000070
#define Ucm_ClkClock_StatusDriftReg 0x00000074
#define Ucm_ClkClock_StatusOffsetFractionsReg 0x00000078
#define Ucm_ClkClock_StatusDriftFractionsReg 0x0000007C

#define CLK_CLOCK_Version 0
#define CLK_CLOCK_Status 1
#define CLK_CLOCK_Seconds 2
#define CLK_CLOCK_Nanoseconds 3
#define CLK_CLOCK_InSync 4
#define CLK_CLOCK_InHoldover 5
#define CLK_CLOCK_InSyncThreshold 6
#define CLK_CLOCK_Source 7
#define CLK_CLOCK_Drift 8
#define CLK_CLOCK_DriftInterval 9
#define CLK_CLOCK_Offset 10
#define CLK_CLOCK_OffsetInterval 11
#define CLK_CLOCK_CorrectedOffset 12
#define CLK_CLOCK_CorrectedDrift 13
#define CLK_CLOCK_Date 14
#define CLK_CLOCK_NUM_PROPS 15

int readClkClockVersion(char *version, size_t size);
int readClkClockStatus(char *status, size_t size);
int readClkClockSeconds(char *seconds, size_t size);
int readClkClockNanoseconds(char *nanoseconds, size_t size);
int readClkClockInSync(char *insync, size_t size);
int readClkClockInHoldover(char *inholdover, size_t size);
int readClkClockInSyncThreshold(char *insyncthreshold, size_t size);
int readClkClockSource(char *source, size_t size);
int readClkClockDrift(char *drift, size_t size);
int readClkClockDriftInterval(char *driftinterval, size_t size);
int readClkClockOffset(char *offset, size_t size);
int readClkClockOffsetInterval(char *offsetinterval, size_t size);
int readClkClockCorrectedOffset(char *correctedoffset, size_t size);
int readClkClockCorrectedDrift(char *correcteddrift, size_t size);
int readClkClockDate(char *date, size_t size);

int writeClkClockInSyncThreshold(char *insyncthreshold, size_t size);
int writeClkClockSeconds(char *seconds, size_t size);
int writeClkClockNanoseconds(char *seconds, size_t size);
int writeClkClockOffset(char *offset, size_t size);
int writeClkClockOffsetInterval(char *interval, size_t size);
int writeClkClockDrift(char *drift, size_t size);
int writeClkClockDriftInterval(char *driftinterval, size_t size);

typedef struct
{
    int64_t core_type;
    int64_t core_instance_nr;
    int64_t address_range_low;
    int64_t address_range_high;
    int64_t interrupt_mask;

    char Version[32];
    char Status[32];
    char Seconds[32];
    char Nanoseconds[32];
    char InSync[32];
    char InHoldover[32];
    char InSyncThreshold[32];
    char Source[32];
    char Drift[32];
    char DriftInterval[32];
    char DriftAdj[32];
    char Offset[32];
    char OffsetInterval[32];
    char OffsetAdj[32];
    char PiSetCustomParameters[32];
    char CorrectedOffset[32];
    char CorrectedDrift[32];
    char Date[32];

} CLK_CLOCK_T;

extern CLK_CLOCK_T CLK_CLOCK;

extern NTL_PROPERTY_T clkProperties[CLK_CLOCK_NUM_PROPS];

#endif // CLK_CLOCK
