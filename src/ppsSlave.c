#include "ntl_uart.h"
#include "cores.h"
// #include "uartCircBuff.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"

#ifdef NTL_TIME_SERVER

uint8_t pps_slave_read_values(NTL_TS_T *ntlts)
{

    int32_t temp_delay = 0;
    uint32_t temp_data = 0;
    uint32_t temp_addr = 0;

    temp_addr = ntlts->ppsRegs.StartAddr;

    // enabled
    if (0 == read_reg(temp_addr + Ucm_PpsSlave_ControlReg, &temp_data))
    {
        if ((temp_data & 0x00000001) == 0)
        {
            ntlts->ppsSlave.Enable = 0;
        }
        else
        {
            ntlts->ppsSlave.Enable = 1;
        }
    }
    else
    {
        ntlts->ppsSlave.Enable = 0;
    }

    // polarity
    if (0 == read_reg(temp_addr + Ucm_PpsSlave_PolarityReg, &temp_data))
    {
        if ((temp_data & 0x00000001) == 0)
        {
            ntlts->ppsSlave.Inverted = 1;
        }
        else
        {
            ntlts->ppsSlave.Inverted = 0;
        }
    }
    else
    {
        ntlts->ppsSlave.Inverted = 0;
    }

    // input ok
    if (0 == read_reg(temp_addr + Ucm_PpsSlave_StatusReg, &temp_data))
    {
        if (temp_data == 0)
        {
            ntlts->ppsSlave.InputOk = 1;
        }
        else
        {
            ntlts->ppsSlave.InputOk = 0;
        }

        // clear after read
        write_reg(temp_addr + Ucm_PpsSlave_StatusReg, &temp_data);
    }
    else
    {
        ntlts->ppsSlave.InputOk = 0;
    }

    // pulse width
    if (0 == read_reg(temp_addr + Ucm_PpsSlave_PulseWidthReg, &temp_data))
    {
        ntlts->ppsSlave.PulseWidth = temp_data;
    }
    else
    {
        // ntlts->ppsSlave.PulseWidthValue->setText("NA");
    }

    // cable delay
    if (0 == read_reg(temp_addr + Ucm_PpsSlave_CableDelayReg, &temp_data))
    {
        temp_delay = (int)(temp_data & 0x3FFFFFFF);
        if ((temp_data & 0x80000000) != 0)
        {
            temp_delay = -1 * temp_delay;
        }
        ntlts->ppsSlave.CableDelay = temp_data; ///->setText(QString::number(temp_delay));
    }
    else
    {
        // ntlts->ppsSlave.CableDelayValue->setText("NA");
    }

    // version
    if (0 == read_reg(temp_addr + Ucm_PpsSlave_VersionReg, &temp_data))
    {
        ntlts->ppsSlave.Version = temp_data; //->setText(QString("0x%1").arg(temp_data, 8, 16, QLatin1Char('0')));
    }
    else
    {
        // ntlts->ppsSlave.VersionValue->setText("NA");
    }

    return 0;
}
uint8_t pps_slave_write_values(NTL_TS_T *ntlts, uint8_t fromRegisters) { return 0; }

#endif