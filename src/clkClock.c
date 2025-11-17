#include "clkClock.h"

#include "cores.h"
#include "ntl_uart.h"
#include "cores.h"
#include "time.h"

CLK_CLOCK_T CLK_CLOCK;

NTL_PROPERTY_T clkProperties[CLK_CLOCK_NUM_PROPS] = {

    [CLK_CLOCK_Version] = {readClkClockVersion, readOnly, CLK_CLOCK.Version},
    [CLK_CLOCK_Status] = {readClkClockStatus, readOnly, CLK_CLOCK.Status},
    [CLK_CLOCK_Seconds] = {readClkClockSeconds, writeClkClockSeconds, CLK_CLOCK.Seconds},
    [CLK_CLOCK_Nanoseconds] = {readClkClockNanoseconds, writeClkClockNanoseconds, CLK_CLOCK.Nanoseconds},
    [CLK_CLOCK_InSync] = {readClkClockInSync, readOnly, CLK_CLOCK.InSync},
    [CLK_CLOCK_InHoldover] = {readClkClockInHoldover, readOnly, CLK_CLOCK.InHoldover},
    [CLK_CLOCK_InSyncThreshold] = {readClkClockInSyncThreshold, writeClkClockInSyncThreshold, CLK_CLOCK.InSyncThreshold},
    [CLK_CLOCK_Source] = {readClkClockSource, readOnly, CLK_CLOCK.Source},
    [CLK_CLOCK_Drift] = {readClkClockDrift, writeClkClockDrift, CLK_CLOCK.Drift},
    [CLK_CLOCK_DriftInterval] = {readClkClockDriftInterval, writeClkClockDriftInterval, CLK_CLOCK.DriftInterval},
    [CLK_CLOCK_Offset] = {readClkClockOffset, writeClkClockOffset, CLK_CLOCK.Offset},
    [CLK_CLOCK_OffsetInterval] = {readClkClockOffsetInterval, writeClkClockOffsetInterval, CLK_CLOCK.OffsetInterval},
    [CLK_CLOCK_CorrectedOffset] = {readClkClockCorrectedOffset, readOnly, CLK_CLOCK.CorrectedOffset},
    [CLK_CLOCK_CorrectedDrift] = {readClkClockCorrectedDrift, readOnly, CLK_CLOCK.CorrectedDrift},
    [CLK_CLOCK_Date] = {readClkClockDate, readOnly, CLK_CLOCK.Date},

};

int readClkClockVersion(char *value, size_t size)
{
    temp_data = 0x00000000;

    temp_addr = CLK_CLOCK.address_range_low;

    if (0 != readRegister(temp_addr + Ucm_ClkClock_VersionReg, &temp_data))
    {
        snprintf(value, size, "%s", "NA");

        return -1;
    }
    snprintf(value, size, "0x%llx", temp_data);

    return 0;
}

int readClkClockStatus(char *status, size_t size)
{
    temp_addr = CLK_CLOCK.address_range_low;

    // enabled
    if (0 != readRegister(temp_addr + Ucm_ClkClock_ControlReg, &temp_data))
    {
        snprintf(status, size, "%s", "err");

        return -1;
    }

    if ((temp_data & 0x00000001) == 0)
    {
        snprintf(status, size, "%s", "disabled");
    }

    else
    {
        snprintf(status, size, "%s", "enabled");
    }

    return 0;
}

int readClkClockSeconds(char *seconds, size_t size)
{

    temp_addr = CLK_CLOCK.address_range_low;

    char status[size];
    readClkClockStatus(status, size);

    temp_data = 0x40000000;

    if (0 == strncmp(status, "enabled", 7))
    {
        temp_data |= 0x00000001;
    }



    if (0 != writeRegister(temp_addr + Ucm_ClkClock_ControlReg, &temp_data))
    {
        snprintf(seconds, size, "%s", "err");
        return -1;
    }

    for (int i = 0; i < 10; i++)
    {
        if (i == 9)
        {
            snprintf(seconds, size, "%s", "err: read did not complete");
            return -1;
        }
        if (0 != readRegister(temp_addr + Ucm_ClkClock_ControlReg, &temp_data))
        {
            snprintf(seconds, size, "%s", "read err");
            return -1;
        }

        if ((temp_data & 0x80000000) != 0)
        {
            if (0 != readRegister(temp_addr + Ucm_ClkClock_TimeValueHReg, &temp_data))
            {
                return -1;
            }

            snprintf(seconds, size, "%lld", temp_data);

            break;
        }
    }

    return 0;
}

int readClkClockNanoseconds(char *nanoseconds, size_t size)
{
    temp_addr = CLK_CLOCK.address_range_low;
    temp_data = 0x40000000;

    char status[size];
    readClkClockStatus(status, size);
    if (0 == strncmp(status, "enabled", size))
    {
        temp_data |= 0x00000001;
    }

    if (0 != writeRegister(temp_addr + Ucm_ClkClock_ControlReg, &temp_data))
    {
        snprintf(nanoseconds, size, "%s", "err");
        return -1;
    }

    for (int i = 0; i < 10; i++)
    {
        if (i == 9)
        {
            snprintf(nanoseconds, size, "%s", "err: read did not complete");
            return -1;
        }
        if (0 != readRegister(temp_addr + Ucm_ClkClock_ControlReg, &temp_data))
        {
            snprintf(nanoseconds, size, "%s", "read err");
            return -1;
        }

        if ((temp_data & 0x80000000) != 0)
        {
            if (0 != readRegister(temp_addr + Ucm_ClkClock_TimeValueLReg, &temp_data))
            {
                return -1;
            }

            snprintf(nanoseconds, size, "%lld", temp_data);

            break;
        }
    }

    return 0;
}
// int readClkClockTimeAdj(char *timeadj, size_t size) {}
int readClkClockInSync(char *insync, size_t size)
{
    temp_addr = CLK_CLOCK.address_range_low;
    temp_data = 0x00000000;

    if (0 != readRegister(temp_addr + Ucm_ClkClock_StatusReg, &temp_data))
    {
        snprintf(insync, size, "%s", "err");
        return -1;
    }

    if ((temp_data & 0x00000001) == 0)
    {
        snprintf(insync, size, "%s", "false");
    }
    else
    {
        snprintf(insync, size, "%s", "true");
    }

    return 0;
}
int readClkClockInHoldover(char *inholdover, size_t size)
{
    temp_addr = CLK_CLOCK.address_range_low;
    temp_data = 0x00000000;

    if (0 != readRegister(temp_addr + Ucm_ClkClock_StatusReg, &temp_data))
    {
        snprintf(inholdover, size, "%s", "err");
        return -1;
    }

    if ((temp_data & 0x00000002) == 0)
    {
        snprintf(inholdover, size, "%s", "false");
    }
    else
    {
        snprintf(inholdover, size, "%s", "true");
    }

    return 0;
}
int readClkClockInSyncThreshold(char *insyncthreshold, size_t size)
{
    temp_addr = CLK_CLOCK.address_range_low;
    temp_data = 0x00000000;

    if (0 != readRegister(temp_addr + Ucm_ClkClock_InSyncThresholdReg, &temp_data))
    {
        snprintf(insyncthreshold, size, "%s", "NA");
        return -1;
    }

    snprintf(insyncthreshold, size, "%lld", temp_data);

    return 0;
}
int readClkClockSource(char *source, size_t size)
{
    temp_addr = CLK_CLOCK.address_range_low;
    temp_data = 0x00000000;

    if (0 != readRegister(temp_addr + Ucm_ClkClock_SelectReg, &temp_data))
    {
        snprintf(source, size, "%s", "NA");
        return -1;
    }

    switch ((temp_data >> 16) & 0x0000FFFF)
    {
    case 0x00000000:
        snprintf(source, size, "%s", "NONE");
        break;
    case 0x0000001:
        snprintf(source, size, "%s", "TOD");
        break;
    case 0x00000002:
        snprintf(source, size, "%s", "IRIG");
        break;
    case 0x00000003:
        snprintf(source, size, "%s", "PPS");
        break;
    case 0x00000004:
        snprintf(source, size, "%s", "PTP");
        break;
    case 0x00000005:
        snprintf(source, size, "%s", "RTC");
        break;
    case 0x00000006:
        snprintf(source, size, "%s", "DCF");
        break;
    case 0x00000007:
        snprintf(source, size, "%s", "NTP");
        break;
    case 0x000000FE:
        snprintf(source, size, "%s", "REG");
        break;
    case 0x000000FF:
        snprintf(source, size, "%s", "EXT");
        break;
    default:
        snprintf(source, size, "%s", "NA");
        break;
    }
    return 0;
}

int readClkClockDrift(char *drift, size_t size)
{
    temp_addr = CLK_CLOCK.address_range_low;
    temp_data = 0x00000000;

    if (0 != readRegister(temp_addr + Ucm_ClkClock_DriftAdjValueReg, &temp_data))
    {
        snprintf(drift, size, "%s", "NA");
        return -1;
    }

    int temp_drift = temp_data & 0x7FFFFFFF;
    if ((temp_data & 0x80000000) != 0)
    {
        temp_drift = -1 * temp_drift;
    }

    snprintf(drift, size, "%d", temp_drift);

    return 0;
}
int readClkClockDriftInterval(char *driftinterval, size_t size)
{
    temp_addr = CLK_CLOCK.address_range_low;
    temp_data = 0x00000000;

    if (0 != readRegister(temp_addr + Ucm_ClkClock_DriftAdjIntervalReg, &temp_data))
    {
        snprintf(driftinterval, size, "%s", "NA");
        return -1;
    }

    snprintf(driftinterval, size, "%ld", temp_data);

    return 0;
}

int readClkClockOffset(char *offset, size_t size)
{
    temp_addr = CLK_CLOCK.address_range_low;
    temp_data = 0x00000000;

    if (0 != readRegister(temp_addr + Ucm_ClkClock_OffsetAdjValueReg, &temp_data))
    {
        snprintf(offset, size, "%s", "NA");
        return -1;
    }

    int temp_offset = temp_data & 0x7FFFFFFF;
    if ((temp_data & 0x80000000) != 0)
    {
        temp_offset = -1 * temp_offset;
    }

    snprintf(offset, size, "%d", temp_offset);

    return 0;
}
int readClkClockOffsetInterval(char *offsetinterval, size_t size)
{
    temp_addr = CLK_CLOCK.address_range_low;
    temp_data = 0x00000000;

    if (0 != readRegister(temp_addr + Ucm_ClkClock_OffsetAdjIntervalReg, &temp_data))
    {
        snprintf(offsetinterval, size, "%s", "NA");
        return -1;
    }

    snprintf(offsetinterval, size, "%ld", temp_data);

    return 0;
}

int readClkClockCorrectedOffset(char *correctedoffset, size_t size)
{
    temp_addr = CLK_CLOCK.address_range_low;
    temp_data = 0x00000000;
    int64_t temp_offset = 0x00000000;

    if (0 != readRegister(temp_addr + Ucm_ClkClock_StatusOffsetReg, &temp_data))
    {
        snprintf(correctedoffset, size, "%s", "NA");
        return -1;
    }

    if ((temp_data & 0x80000000) != 0)
    {
        temp_offset = -1 * (temp_data & 0x7FFFFFFF);
    }
    else
    {
        temp_offset = (temp_data & 0x7FFFFFFF);
    }

    if (0 != readRegister(temp_addr + Ucm_ClkClock_StatusOffsetFractionsReg, &temp_data))
    {
        snprintf(correctedoffset, size, "%s", "NA");
        return -1;
    }
    double temp_fract;
    double temp_fract_offset;
    temp_fract = (double)temp_data;
    temp_fract = temp_fract / 65536.0;
    if (temp_offset < 0)
    {
        temp_fract *= -1.0;
    }
    temp_fract_offset = (double)temp_offset + temp_fract;

    snprintf(correctedoffset, size, "%f", temp_fract_offset);

    // ui->ClkClockCorrectedOffsetValue->setText(QString::number(temp_fract_offset, 'f', 4));

    // ui->ClkClockCorrectedOffsetValue->setText(QString::number(temp_offset));

    return 0;
}

int readClkClockCorrectedDrift(char *correcteddrift, size_t size)
{
    temp_addr = CLK_CLOCK.address_range_low;
    temp_data = 0x00000000;
    int64_t temp_drift = 0x00000000;

    // corrected drift
    if (0 != readRegister(temp_addr + Ucm_ClkClock_StatusDriftReg, &temp_data))
    {
        return -1;
    }

    if ((temp_data & 0x80000000) != 0)
    {
        temp_drift = -1 * (temp_data & 0x7FFFFFFF);
    }
    else
    {
        temp_drift = (temp_data & 0x7FFFFFFF);
    }

    if (0 != readRegister(temp_addr + Ucm_ClkClock_StatusDriftFractionsReg, &temp_data))
    {
        return -1;
    }
    double temp_fract;
    double temp_fract_drift;
    temp_fract = (double)temp_data;
    temp_fract = temp_fract / 65536.0;
    if (temp_drift < 0)
    {
        temp_fract *= -1.0;
    }
    temp_fract_drift = (double)temp_drift + temp_fract;
    // ui->ClkClockCorrectedDriftValue->setText(QString::number(temp_fract_drift, 'f', 4));
    snprintf(correcteddrift, size, "%f", temp_fract_drift);

    return 0;
}

int readClkClockDate(char *date, size_t size)
{
    char seconds[size];
    readClkClockSeconds(seconds, size);

    time_t temp_time = strtol(seconds, NULL, 10);

    struct tm *timeinfo = gmtime(&temp_time);

    // Format time as dd.mm.yyyy hh:mm:ss
    strftime(date, size, "%d.%m.%Y %H:%M:%S", timeinfo);

    return 0;
}

int writeClkClockSource(char *source, size_t size)
{
    temp_addr = CLK_CLOCK.address_range_low;
    temp_data = 0x00000000;

    if (0 == strncmp(source, "NONE", size))
    {
        temp_data = 0x00000000;
    }
    else if (0 == strncmp(source, "TOD", size))
    {
        temp_data = 0x00000001;
    }
    else if (0 == strncmp(source, "IRIG", size))
    {
        temp_data = 0x00000002;
    }
    else if (0 == strncmp(source, "PPS", size))
    {
        temp_data = 0x00000003;
    }
    else if (0 == strncmp(source, "PTP", size))
    {
        temp_data = 0x00000004;
    }
    else if (0 == strncmp(source, "RTC", size))
    {
        temp_data = 0x00000005;
    }
    else if (0 == strncmp(source, "DCF", size))
    {
        temp_data = 0x00000006;
    }
    else if (0 == strncmp(source, "NTP", size))
    {
        temp_data = 0x00000007;
    }
    else if (0 == strncmp(source, "REG", size))
    {
        temp_data = 0x000000FE;
    }
    else if (0 == strncmp(source, "EXT", size))
    {
        temp_data = 0x000000FF;
    }
    else
    {
        temp_data = 0x00000000;
    }

    if (0 != writeRegister(temp_addr + Ucm_ClkClock_SelectReg, &temp_data))
    {
        return -1;
    }

    return 0;
}

int writeClkClockInSyncThreshold(char *insyncthreshold, size_t size)
{
    temp_addr = CLK_CLOCK.address_range_low;
    temp_data = 0x00000000;

    long temp_threshold = strtol(insyncthreshold, NULL, 10);

    temp_data = temp_threshold;

    if (0 != writeRegister(temp_addr + Ucm_ClkClock_InSyncThresholdReg, &temp_data))
    {
        snprintf(insyncthreshold, size, "%s", "NA");
        return -1;
    }

    // snprintf(insyncthreshold, size, "%ld", temp_data);

    return 0;
}

int writeClkClockSeconds(char *seconds, size_t size)
{
    temp_addr = CLK_CLOCK.address_range_low;
    temp_data = 0x00000000;

    temp_data = strtol(seconds, NULL, 10);

    if (0 != writeRegister(temp_addr + Ucm_ClkClock_TimeAdjValueHReg, &temp_data))
    {
        snprintf(seconds, size, "%s", "NA");
        return -1;
    }

    // snprintf(insyncthreshold, size, "%ld", temp_data);

    // writeClkClockTimeAdj("disabled", size);

    return 0;
}

int writeClkClockNanoseconds(char *nanoseconds, size_t size)
{
    temp_addr = CLK_CLOCK.address_range_low;
    temp_data = 0x00000000;

    temp_data = strtol(nanoseconds, NULL, 10);

    if (0 != writeRegister(temp_addr + Ucm_ClkClock_TimeAdjValueLReg, &temp_data))
    {
        snprintf(nanoseconds, size, "%s", "NA");
        return -1;
    }

    // snprintf(insyncthreshold, size, "%ld", temp_data);

    // writeClkClockTimeAdj("disabled", size);

    return 0;
}

int writeClkClockOffset(char *offset, size_t size)
{
    temp_addr = CLK_CLOCK.address_range_low;
    temp_data = 0x00000000;

    long temp_offset = strtol(offset, NULL, 10);

    if (temp_offset < 0)
    {
        temp_data = abs(temp_offset) | 0x80000000;
    }
    else
    {
        temp_data = abs(temp_offset);
    }

    if (0 != writeRegister(temp_addr + Ucm_ClkClock_OffsetAdjValueReg, &temp_data))
    {
        snprintf(offset, size, "%s", "NA");
        return -1;
    }

    return 0;
}

int writeClkClockOffsetInterval(char *interval, size_t size)
{
    temp_addr = CLK_CLOCK.address_range_low;
    temp_data = 0x00000000;

    int64_t temp_interval = strtol(interval, NULL, 10);

    if (0 != writeRegister(temp_addr + Ucm_ClkClock_OffsetAdjIntervalReg, &temp_interval))
    {
        snprintf(interval, size, "%s", "NA");
        return -1;
    }

    return 0;
}

int writeClkClockDrift(char *drift, size_t size)
{
    temp_addr = CLK_CLOCK.address_range_low;
    temp_data = 0x00000000;

    long temp_drift = strtol(drift, NULL, 10);

    if (temp_drift < 0)
    {
        temp_data = abs(temp_drift) | 0x80000000;
    }
    else
    {
        temp_data = abs(temp_drift);
    }

    if (0 != writeRegister(temp_addr + Ucm_ClkClock_DriftAdjValueReg, &temp_data))
    {
        snprintf(drift, size, "%s", "NA");
        return -1;
    }

    return 0;
}
int writeClkClockDriftInterval(char *driftinterval, size_t size)
{
    temp_addr = CLK_CLOCK.address_range_low;
    temp_data = 0x00000000;

    int64_t temp_interval = strtol(driftinterval, NULL, 10);

    if (0 != writeRegister(temp_addr + Ucm_ClkClock_DriftAdjIntervalReg, &temp_interval))
    {
        snprintf(driftinterval, size, "%s", "NA");
        return -1;
    }

    return 0;
}
