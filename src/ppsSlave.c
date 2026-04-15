#include "ntl_uart.h"
// #include "uartCircBuff.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"

int8_t pps_slave_read_values(NTL_TS_T *ntlts)
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
int8_t pps_slave_write_values(NTL_TS_T *ntlts, uint8_t fromRegisters)

{
    uint32_t temp_data = 0;
    uint32_t temp_addr = 0;
    temp_addr = ntlts->ppsRegs.StartAddr;

    // cable delay
    // temp_string = ui->PpsSlaveCableDelayValue->text();
    // temp_delay = temp_string.toInt(nullptr, 10);

    int temp_delay = ntlts->ppsSlave.CableDelay;

    if (false)
    // if (temp_string == "NA")
    {
        // nothing
    }
    else
    {
        temp_data = abs(temp_delay) & 0x3FFFFFFF;
        if (temp_delay < 0)
        {
            temp_data |= 0x80000000; // set sign bit
        }

        if (fromRegisters)
        {
            temp_data = ntlts->ppsRegs.CableDelayReg;
        }

        if (0 == write_reg(temp_addr + Ucm_PpsSlave_CableDelayReg, &temp_data))
        {
            // ui->PpsSlaveCableDelayValue->setText(QString::number(temp_delay));
        }
        else
        {
            // ui->PpsSlaveCableDelayValue->setText("NA");
            return -1;
        }
    }

    // polarity
    temp_data = 0x00000000; // nothing
    // if (false == ui->PpsSlaveInvertedCheckBox->isChecked())
    if (ntlts->ppsSlave.Inverted == 0)
    {
        temp_data |= 0x00000001; // no inversion
    }

    if (fromRegisters)
    {
        temp_data = ntlts->ppsRegs.PolarityReg;
    }

    if (0 == write_reg(temp_addr + Ucm_PpsSlave_PolarityReg, &temp_data))
    {
        // nothing
    }
    else
    {
        // ui->PpsSlaveInvertedCheckBox->setChecked(false);
        return -2;
    }

    temp_data = 0x00000000; // nothing
    // if (true == ui->PpsSlaveEnableCheckBox->isChecked())
    if (ntlts->ppsSlave.Enable)
    {
        temp_data |= 0x00000001; // enable
    }

    if (fromRegisters)
    {
        temp_data = ntlts->ppsRegs.ControlReg;
    }

    if (0 == write_reg(temp_addr + Ucm_PpsSlave_ControlReg, &temp_data))
    {
        // nothing
    }
    else
    {
        // ui->PpsSlaveEnableCheckBox->setChecked(false);
        return -3;
    }

    return 0;
}



void pps_handler(char *temp_rsp, int rsp_size, const char *prop, char *val)
{

    int write = 0;
    int err = 0;

    err = pps_slave_read_values(&ntlts);

    if (err != 0)
    {
        snprintf(temp_rsp, rsp_size, "PPS_READ_ERR: %d", err);
    }

    if (val != NULL)
    {
        val[strcspn(val, "\r\n")] = 0; // remove \r\n
        write = 1;
    }

    // GET / SET ENABLE
    if (strncmp(prop, "ENB", 3) == 0)
    {
        if (write)
        {
            ntlts.ppsSlave.Enable = strtoul(val, NULL, 10);
        }

        // return ram value
        snprintf(temp_rsp, rsp_size, "$PPS,ENB,%d", ntlts.ppsSlave.Enable);
    }

    // GET / SET PULSE WIDTH
    else if (strncmp(prop, "PWD", 3) == 0)
    {
        if (write)
        {
            ntlts.ppsSlave.PulseWidth = strtoul(val, NULL, 10);
        }

        // return ram value
        snprintf(temp_rsp, rsp_size, "$PPS,PWD,%d", ntlts.ppsSlave.PulseWidth);
    }

    // GET / SET CABLE DELAY
    else if (strncmp(prop, "CDY", 3) == 0)
    {
        if (write)
        {
            ntlts.ppsSlave.CableDelay = strtoul(val, NULL, 10);
        }

        // return ram value
        snprintf(temp_rsp, rsp_size, "$PPS,CDY,%d", ntlts.ppsSlave.CableDelay);
    }

    // GET INPUT OK
    else if (strncmp(prop, "IOK", 3) == 0)
    {

        // return ram value
        snprintf(temp_rsp, rsp_size, "$PPS,IOK,%d", ntlts.ppsSlave.InputOk);
    }

    // WRITE FPGA REGS WITH UPDATED RAM
    if (write)
    {
        err = pps_slave_write_values(&ntlts, 0);
        if (err != 0)
        {
            snprintf(temp_rsp, rsp_size, "PPS_WRITE_ERR: %d", err);
        }
        write = 0;

        err = pps_slave_read_values(&ntlts);

        if (err != 0)
        {
            snprintf(temp_rsp, rsp_size, "PPS_READ_ERR: %d", err);
        }
    }
}
