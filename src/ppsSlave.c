#include "cores.h"
#include "ppsSlave.h"

#include "cores.h"
#include "ntl_uart.h"

PPS_SLAVE_T PPS_SLAVE;

NTL_PROPERTY_T ppsProperties[PPS_SLAVE_NUM_PROPS] = {

    [PPS_SLAVE_Version] = {readPpsSlaveVersion, readOnly, PPS_SLAVE.Version},
    [PPS_SLAVE_EnableStatus] = {readPpsSlaveEnableStatus, writePpsSlaveEnableStatus, PPS_SLAVE.EnableStatus},
    [PPS_SLAVE_Polarity] = {readPpsSlavePolarity, writePpsSlavePolarity, PPS_SLAVE.Polarity},
    [PPS_SLAVE_InputOkStatus] = {readPpsSlaveInputOkStatus, readOnly, PPS_SLAVE.InputOkStatus},
    [PPS_SLAVE_PulseWidthValue] = {readPpsSlavePulseWidthValue, readOnly, PPS_SLAVE.PulseWidthValue},
    [PPS_SLAVE_CableDelayValue] = {readPpsSlaveCableDelayValue, writePpsSlaveCableDelayValue, PPS_SLAVE.CableDelayValue},

};



int readPpsSlaveVersion(char *value, size_t size)
{
    temp_data = 0x00000000;

    temp_addr = PPS_SLAVE.address_range_low;

    if (0 != readRegister(temp_addr + Ucm_PpsSlave_VersionReg, &temp_data))
    {
        snprintf(value, size, "%s", "NA");

        return -1;
    }
    snprintf(value, size, "0x%llx", temp_data);

    return 0;
}

int readPpsSlaveEnableStatus(char *status, size_t size)
{
    temp_data = 0x00000000;

    temp_addr = PPS_SLAVE.address_range_low;

    // enabled
    if (0 != readRegister(temp_addr + Ucm_PpsSlave_ControlReg, &temp_data))
    {
        snprintf(status, size, "%s", "disabled");

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

int readPpsSlavePolarity(char *status, size_t size)
{
    snprintf(status, size, "%s", "err");

    temp_data = 0x00000000;

    temp_addr = PPS_SLAVE.address_range_low;

    // enabled
    if (0 != readRegister(temp_addr + Ucm_PpsSlave_PolarityReg, &temp_data))
    {
        snprintf(status, size, "%s", "disabled");

        return -1;
    }

    // printf("read temp_data in polarity reg: 0x%08lx\n", temp_data);

    if ((temp_data & 0x00000001) == 0)
    {
        snprintf(status, size, "%s", "enabled");
    }

    else
    {
        snprintf(status, size, "%s", "disabled");
    }

    return 0;
}

int readPpsSlaveInputOkStatus(char *status, size_t size)
{
    temp_data = 0x00000000;
    temp_addr = PPS_SLAVE.address_range_low;

    // enabled
    if (0 != readRegister(temp_addr + Ucm_PpsSlave_StatusReg, &temp_data))
    {
        snprintf(status, size, "%s", "disabled"); // not ok

        return -1;
    }

    if (temp_data == 0)
    {
        snprintf(status, size, "%s", "enabled"); // ok
    }

    else
    {
        snprintf(status, size, "%s", "disabled");
    }
    // clear
    writeRegister(temp_addr + Ucm_PpsSlave_StatusReg, &temp_data);

    return 0;
}

int readPpsSlavePulseWidthValue(char *value, size_t size)
{
    temp_data = 0x00000000;
    temp_addr = PPS_SLAVE.address_range_low;
    snprintf(value, size, "%s", "NA"); // not ok

    // enabled
    if (0 != readRegister(temp_addr + Ucm_PpsSlave_PulseWidthReg, &temp_data))
    {
        snprintf(value, size, "%s", "NA"); // not ok

        return -1;
    }

    snprintf(value, size, "%lld", temp_data);

    return 0;
}

int readPpsSlaveCableDelayValue(char *value, size_t size)
{
    temp_data = 0x00000000;
    temp_addr = PPS_SLAVE.address_range_low;
    int64_t temp_delay;
    snprintf(value, size, "%s", "NA"); // not ok

    // enabled
    if (0 != readRegister(temp_addr + Ucm_PpsSlave_CableDelayReg, &temp_data))
    {
        snprintf(value, size, "%s", "NA"); // not ok

        return -1;
    }
    temp_delay = (int)(temp_data & 0x3FFFFFFF);
    if ((temp_data & 0x80000000) != 0)
    {
        temp_delay = -1 * temp_delay;
    }
    snprintf(value, size, "%lld", temp_delay);

    return 0;
}

//

int writePpsSlaveCableDelayValue(char *cable_delay, size_t size)
{

    long temp_cable_delay = (strtol(cable_delay, NULL, 10)); // takes 0x44 and puts in the top of the ds3 reg -> 0x44000000

    temp_addr = PPS_SLAVE.address_range_low;
    temp_data = 0x00000000;

    // if (0 != readRegister(temp_addr + Ucm_PpsSlave_CableDelayReg, &temp_data))
    //{
    //     return -2; // read current settings fails
    // }

    temp_data = abs(temp_cable_delay) & 0x3FFFFFFF;
    if (temp_cable_delay < 0)
    {
        temp_data |= 0x80000000; // set sign bit
    }

    if (0 != writeRegister(temp_addr + Ucm_PpsSlave_CableDelayReg, &temp_data))
    {
        return -3;
    }

    return 0;
}

int writePpsSlavePolarity(char *status, size_t size)
{
    temp_addr = PPS_SLAVE.address_range_low;
    temp_data = 0x00000000;

    // printf("status: %s\n", status);

    if (0 == strncmp(status, "enabled", 7))
    {
        temp_data = 0x00000001 | temp_data;
    }

    else if (0 == strncmp(status, "disabled", 8))
    {
        temp_data = 0x00000000 | temp_data; // disable
    }
    else
    {
        return -2;
    }

    // printf("temp_data in inverted func: 0x%08lx\n", temp_data);
    if (0 != writeRegister(temp_addr + Ucm_PpsSlave_PolarityReg, &temp_data))
    {
        return -3;
    }

    return 0;
}

int writePpsSlaveEnableStatus(char *status, size_t size)
{
    temp_addr = PPS_SLAVE.address_range_low;
    temp_data = 0x00000000;

    if (0 == strncmp(status, "enabled", 7))
    {
        temp_data = 0x00000001 | temp_data;
    }
    else if (0 == strncmp(status, "disabled", 8))
    {
        temp_data = 0x00000000 | temp_data; // disable
    }
    else
    {
        return -2;
    }

    if (0 != writeRegister(temp_addr + Ucm_PpsSlave_ControlReg, &temp_data))
    {
        return -3;
    }

    return 0;
}
