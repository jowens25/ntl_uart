#include "main.h"
#ifndef LINUX
#include "uartCircBuff.h"

#else
#include "socket_interface.h"
#endif

#include "ntl_uart.h"

#include "stdlib.h"
#include "string.h"
#include "stdbool.h"

#ifdef NTL_TIME_SERVER

int8_t clk_clock_read_values(NTL_TS_T *ntlts)
{
    uint32_t temp_data = 0;
    uint32_t temp_addr = 0;

    double temp_drift = 0;
    double temp_offset = 0;

    temp_addr = ntlts->clkRegs.StartAddr;

    // enabled
    if (0 == read_reg(temp_addr + Ucm_ClkClock_ControlReg, &temp_data))
    {
        ntlts->clkRegs.ControlReg = temp_data;
        if ((temp_data & 0x00000001) == 0)
        {
            ntlts->clkClock.Enable = 0;
        }
        else
        {
            ntlts->clkClock.Enable = 1;
        }
    }
    else
    {
        ntlts->clkClock.Enable = 0;
    }

    temp_data = 0x40000000;
    if (ntlts->clkClock.Enable)
    {
        temp_data |= 0x00000001;
    }
    if (0 == write_reg(temp_addr + Ucm_ClkClock_ControlReg, &temp_data))
    {
        for (int i = 0; i < 10; i++)
        {
            if (0 == read_reg(temp_addr + Ucm_ClkClock_ControlReg, &temp_data))
            {
                if ((temp_data & 0x80000000) != 0)
                {
                    // seconds
                    if (0 == read_reg(temp_addr + Ucm_ClkClock_TimeValueHReg, &temp_data))
                    {
                        ntlts->clkClock.Seconds = temp_data;
                    }
                    else
                    {
                        // ntlts->clkClock.Seconds = -1;
                    }

                    // date
                    // QDateTime temp_date;
                    // temp_date.setSecsSinceEpoch(temp_data);
                    // ntlts->clkClock.Date->setText(temp_date.toUTC().toString("dd.MM.yyyy hh:mm:ss"));

                    // nanoseconds
                    if (0 == read_reg(temp_addr + Ucm_ClkClock_TimeValueLReg, &temp_data))
                    {
                        ntlts->clkClock.Nanoseconds = temp_data;
                    }
                    else
                    {
                        // ntlts->clkClock.Nanoseconds = -1;
                    }

                    break;
                }
                else if (i == 9)
                {
                    // cout << "ERROR: " << "read did not complete" << endl;
                    // ntlts->clkClock.Seconds->setText("NA");
                    // ntlts->clkClock.Nanoseconds->setText("NA");
                }
            }
            else
            {
                // ntlts->clkClock.Seconds->setText("NA");
                // ntlts->clkClock.Nanoseconds->setText("NA");
            }
        }
    }
    else
    {
        // ntlts->clkClock.Seconds->setText("NA");
        // ntlts->clkClock.Nanoseconds->setText("NA");
    }

    // in sync
    if (0 == read_reg(temp_addr + Ucm_ClkClock_StatusReg, &temp_data))
    {
        ntlts->clkRegs.StatusReg = temp_data;

        if ((temp_data & 0x00000001) == 0)
        {
            ntlts->clkClock.InSync = 0;
        }
        else
        {
            ntlts->clkClock.InSync = 1;
        }

        if ((temp_data & 0x00000002) == 0)
        {
            ntlts->clkClock.InHoldover = 0;
        }
        else
        {
            ntlts->clkClock.InHoldover = 1;
        }
    }
    else
    {
        // ntlts->clkClock.InSync->setText("NA");
        // ntlts->clkClock.InHoldover->setText("NA");
    }

    // in sync Threshold
    if (0 == read_reg(temp_addr + Ucm_ClkClock_InSyncThresholdReg, &temp_data))
    {
        ntlts->clkClock.InSyncThreshold = temp_data;
        ntlts->clkRegs.InSyncThresholdReg = temp_data;
    }
    else
    {
        // ntlts->clkClock.InSyncThreshold->setText("NA");
    }

    // offset
    if (0 == read_reg(temp_addr + Ucm_ClkClock_OffsetAdjValueReg, &temp_data))
    {
        ntlts->clkRegs.OffsetAdjValueReg = temp_data;

        int32_t temp_offset = temp_data & 0x7FFFFFFF;
        if ((temp_data & 0x80000000) != 0)
        {
            temp_offset = -1 * temp_offset;
        }
        // ui->ClkClockOffsetValue->setText(QString::number(temp_offset));
        ntlts->clkClock.Offset = temp_offset;

        if (0 == read_reg(temp_addr + Ucm_ClkClock_OffsetAdjIntervalReg, &temp_data))
        {
            // ui->ClkClockOffsetIntervalValue->setText(QString::number(temp_data));
            ntlts->clkClock.OffsetInterval = temp_data;
        }
        else
        {
            // ui->ClkClockOffsetValue->setText("NA");
            // ui->ClkClockOffsetIntervalValue->setText("NA");
            // ui->ClkClockOffsetAdjCheckBox->setChecked(false);
        }
    }
    else
    {
        // ui->ClkClockOffsetValue->setText("NA");
        // ui->ClkClockOffsetIntervalValue->setText("NA");
        // ui->ClkClockOffsetAdjCheckBox->setChecked(false);
    }

    // drift
    if (0 == read_reg(temp_addr + Ucm_ClkClock_DriftAdjValueReg, &temp_data))
    {
        ntlts->clkRegs.DriftAdjValueReg = temp_data;

        bool is_negative = temp_data & 0x80000000;
        if (is_negative != 0)
        {
            temp_drift = -1 * (temp_data & 0x7FFFFFFF);
        }
        else
        {
            temp_drift = (temp_data & 0x7FFFFFFF);
        }

        if (0 == read_reg(temp_addr + Ucm_ClkClock_DriftAdjFractionsReg, &temp_data))
        {
            // ntlts->clkRegs.StatusDriftFractionsReg = temp_data;

            double temp_fract;
            double temp_fract_drift;
            temp_fract = (double)temp_data;
            temp_fract = temp_fract / 65536.0;
            if (is_negative != 0)
            {
                temp_fract *= -1.0;
            }
            temp_fract_drift = (double)temp_drift + temp_fract;
            // ui->ClkClockDriftValue->setText(QString::number(temp_fract_drift, 'f', 4));
            ntlts->clkClock.Drift = temp_fract_drift;
        }
        else
        {
            // ui->ClkClockDriftValue->setText(QString::number(temp_drift));
        }

        if (0 == read_reg(temp_addr + Ucm_ClkClock_DriftAdjIntervalReg, &temp_data))
        {
            // ui->ClkClockDriftIntervalValue->setText(QString::number(temp_data));
            ntlts->clkClock.DriftInterval = temp_data;
        }
        else
        {
            // ui->ClkClockDriftValue->setText("NA");
            // ui->ClkClockDriftIntervalValue->setText("NA");
            // ui->ClkClockDriftAdjCheckBox->setChecked(false);
        }
    }
    else
    {
        // ui->ClkClockDriftValue->setText("NA");
        // ui->ClkClockDriftIntervalValue->setText("NA");
        // ui->ClkClockDriftAdjCheckBox->setChecked(false);
    }

    // source
    if (0 == read_reg(temp_addr + Ucm_ClkClock_SelectReg, &temp_data))
    {
        ntlts->clkRegs.SelectReg = temp_data;
        switch ((temp_data >> 16) & 0x0000FFFF)
        {
        case 0x00000000:
            // ntlts->clkClock.Source->setCurrentText("NONE");
            snprintf(ntlts->clkClock.Source, sizeof("NONE"), "NONE");
            break;
        case 0x0000001:
            // ntlts->clkClock.Source->setCurrentText("TOD");
            snprintf(ntlts->clkClock.Source, sizeof("TOD"), "TOD");
            break;
        case 0x00000002:
            // ntlts->clkClock.Source->setCurrentText("IRIG");
            snprintf(ntlts->clkClock.Source, sizeof("IRIG"), "IRIG");
            break;
        case 0x00000003:
            // ntlts->clkClock.Source->setCurrentText("PPS");
            snprintf(ntlts->clkClock.Source, sizeof("PPS"), "PPS");
            break;
        case 0x00000004:
            // ntlts->clkClock.Source->setCurrentText("PTP");
            snprintf(ntlts->clkClock.Source, sizeof("PTP"), "PTP");
            break;
        case 0x00000005:
            // ntlts->clkClock.Source->setCurrentText("RTC");
            snprintf(ntlts->clkClock.Source, sizeof("RTC"), "RTC");
            break;
        case 0x00000006:
            // ntlts->clkClock.Source->setCurrentText("DCF");
            snprintf(ntlts->clkClock.Source, sizeof("DCF"), "DCF");
            break;
        case 0x00000007:
            // ntlts->clkClock.Source->setCurrentText("NTP");
            snprintf(ntlts->clkClock.Source, sizeof("NTP"), "NTP");
            break;
        case 0x000000FE:
            // ntlts->clkClock.Source->setCurrentText("REG");
            snprintf(ntlts->clkClock.Source, sizeof("REG"), "REG");
            break;
        case 0x000000FF:
            // ntlts->clkClock.Source->setCurrentText("EXT");
            snprintf(ntlts->clkClock.Source, sizeof("EXT"), "EXT");
            break;
        default:
            // ntlts->clkClock.Source->setCurrentText("NA");
            snprintf(ntlts->clkClock.Source, sizeof("NA"), "NA");
            break;
        }
    }
    else
    {
        // ntlts->clkClock.Source->setCurrentText("NA");
        snprintf(ntlts->clkClock.Source, sizeof("NA"), "NA");
    }

    // PI servo parameters
    // we consider them Write Only since after the calculations we can not reproduce the value again
    // corrected offset
    if (0 == read_reg(temp_addr + Ucm_ClkClock_StatusOffsetReg, &temp_data))
    {
        ntlts->clkRegs.StatusOffsetReg = temp_data;

        if ((temp_data & 0x80000000) != 0)
        {
            temp_offset = -1 * (temp_data & 0x7FFFFFFF);
        }
        else
        {
            temp_offset = (temp_data & 0x7FFFFFFF);
        }

        if (0 == read_reg(temp_addr + Ucm_ClkClock_StatusOffsetFractionsReg, &temp_data))
        {
            double temp_fract;
            double temp_fract_offset;
            temp_fract = (double)temp_data;
            temp_fract = temp_fract / 65536.0;
            if (temp_offset < 0)
            {
                temp_fract *= -1.0;
            }
            temp_fract_offset = (double)temp_offset + temp_fract;
            // ntlts->clkClock.CorrectedOffset->setText(QString::number(temp_fract_offset, 'f', 4));
            ntlts->clkClock.CorrectedOffset = temp_fract_offset;
        }
        else
        {
            // ntlts->clkClock.CorrectedOffset->setText(QString::number(temp_offset));
            ntlts->clkClock.CorrectedOffset = temp_offset;
        }
    }
    else
    {
        // ntlts->clkClock.CorrectedOffset->setText("NA");
    }

    // corrected drift
    if (0 == read_reg(temp_addr + Ucm_ClkClock_StatusDriftReg, &temp_data))
    {
        ntlts->clkRegs.StatusDriftReg = temp_data;

        if ((temp_data & 0x80000000) != 0)
        {
            temp_drift = -1 * (temp_data & 0x7FFFFFFF);
        }
        else
        {
            temp_drift = (temp_data & 0x7FFFFFFF);
        }

        if (0 == read_reg(temp_addr + Ucm_ClkClock_StatusDriftFractionsReg, &temp_data))
        {
            double temp_fract;
            double temp_fract_drift;
            temp_fract = (double)temp_data;
            temp_fract = temp_fract / 65536.0;
            if (temp_drift < 0)
            {
                temp_fract *= -1.0;
            }
            temp_fract_drift = (double)temp_drift + temp_fract;
            ////ntlts->clkClock.CorrectedDrift->setText(QString::number(temp_fract_drift, 'f', 4));
            ntlts->clkClock.CorrectedDrift = temp_fract_drift;
        }
        else
        {
            // ntlts->clkClock.CorrectedDrift->setText(QString::number(temp_drift));
            ntlts->clkClock.CorrectedDrift = temp_drift;
        }
    }
    else
    {
        // ntlts->clkClock.CorrectedDrift->setText("NA");
    }

    // version
    if (0 == read_reg(temp_addr + Ucm_ClkClock_VersionReg, &temp_data))
    {
        // ntlts->clkClock.Version->setText(QString("0x%1").arg(temp_data, 8, 16, QLatin1Char('0')));
        ntlts->clkClock.Version = temp_data;
    }
    else
    {
        // ntlts->clkClock.Version->setText("NA");
    }

    return 0;
}

// void clk_clock_write_values(NTL_TS_T *ntlts, uint8_t fromRegisters)
int8_t clk_clock_write_values(NTL_TS_T *ntlts, uint8_t fromRegisters)

{

    uint32_t temp_data = 0;
    uint32_t temp_addr = 0;

    temp_addr = ntlts->clkRegs.StartAddr;

    // source
    // temp_string = ui->ClkClockSourceValue->currentText();
    // if (temp_string == "NONE")
    if (strncmp(ntlts->clkClock.Source, "NONE", strlen("NONE")) == 0)
    {
        temp_data = 0x00000000;
    }
    else if (strncmp(ntlts->clkClock.Source, "TOD", strlen("TOD")) == 0)
    {
        temp_data = 0x00000001;
    }
    else if (strncmp(ntlts->clkClock.Source, "IRIG", strlen("IRIG")) == 0)
    {
        temp_data = 0x00000002;
    }
    else if (strncmp(ntlts->clkClock.Source, "PPS", strlen("PPS")) == 0)
    {
        temp_data = 0x00000003;
    }
    else if (strncmp(ntlts->clkClock.Source, "PTP", strlen("PTP")) == 0)
    {
        temp_data = 0x00000004;
    }
    else if (strncmp(ntlts->clkClock.Source, "RTC", strlen("RTC")) == 0)
    {
        temp_data = 0x00000005;
    }
    else if (strncmp(ntlts->clkClock.Source, "DCF", strlen("DCF")) == 0)
    {
        temp_data = 0x00000006;
    }
    else if (strncmp(ntlts->clkClock.Source, "NTP", strlen("NTP")) == 0)
    {
        temp_data = 0x00000007;
    }
    else if (strncmp(ntlts->clkClock.Source, "REG", strlen("REG")) == 0)
    {
        temp_data = 0x000000FE;
    }
    else if (strncmp(ntlts->clkClock.Source, "EXT", strlen("EXT")) == 0)
    {
        temp_data = 0x000000FF;
    }
    else
    {
        temp_data = 0x00000000;
    }

    if (strncmp(ntlts->clkClock.Source, "NA", strlen("NA")) == 0)
    // if (temp_string == "NA")
    {
        // nothing
    }
    if (fromRegisters)
    {
        temp_data = ntlts->clkRegs.SelectReg;
    }
    else if (0 == write_reg(temp_addr + Ucm_ClkClock_SelectReg, &temp_data))
    {
        // switch (temp_data & 0x0000FFFF)
        //{
        // case 0x00000000:
        //     ui->ClkClockSourceValue->setCurrentText("NONE");
        //     break;
        // case 0x00000001:
        //     ui->ClkClockSourceValue->setCurrentText("TOD");
        //     break;
        // case 0x00000002:
        //     ui->ClkClockSourceValue->setCurrentText("IRIG");
        //     break;
        // case 0x00000003:
        //     ui->ClkClockSourceValue->setCurrentText("PPS");
        //     break;
        // case 0x00000004:
        //     ui->ClkClockSourceValue->setCurrentText("PTP");
        //     break;
        // case 0x00000005:
        //     ui->ClkClockSourceValue->setCurrentText("RTC");
        //     break;
        // case 0x00000006:
        //     ui->ClkClockSourceValue->setCurrentText("DCF");
        //     break;
        // case 0x00000007:
        //     ui->ClkClockSourceValue->setCurrentText("NTP");
        //     break;
        // case 0x000000FE:
        //     ui->ClkClockSourceValue->setCurrentText("REG");
        //     break;
        // case 0x000000FF:
        //     ui->ClkClockSourceValue->setCurrentText("EXT");
        //     break;
        // default:
        //     ui->ClkClockSourceValue->setCurrentText("NA");
        //     break;
        // }
    }
    else
    {
        // ui->ClkClockSourceValue->setCurrentText("NA");
    }

    // in sync Threshold
    // temp_string = ui->ClkClockInSyncThresholdValue->text();
    // temp_data = temp_string.toUInt(nullptr, 10);

    temp_data = ntlts->clkClock.InSyncThreshold;

    if (fromRegisters)
    {
        temp_data = ntlts->clkRegs.InSyncThresholdReg;
    }
    // if (temp_string == "NA")
    if (false)
    {
        // nothing
    }
    else if (0 == write_reg(temp_addr + Ucm_ClkClock_InSyncThresholdReg, &temp_data))
    {
        // ui->ClkClockInSyncThresholdValue->setText(QString::number(temp_data));
    }
    else
    {
        // ui->ClkClockInSyncThresholdValue->setText("NA");
        return -1;
    }

    // seconds
    // temp_string = ui->ClkClockSecondsValue->text();
    // temp_data = temp_string.toUInt(nullptr, 10);

    temp_data = ntlts->clkClock.Seconds;

    if (fromRegisters)
    {
        temp_data = ntlts->clkRegs.TimeAdjValueHReg;
    }

    // if (temp_string == "NA")
    if (false)
    {
        // nothing
    }
    else if (0 == write_reg(temp_addr + Ucm_ClkClock_TimeAdjValueHReg, &temp_data))
    {
        // ui->ClkClockSecondsValue->setText(QString::number(temp_data));
    }
    else
    {
        // ui->ClkClockTimeAdjCheckBox->setChecked(false);
        // ui->ClkClockSecondsValue->setText("NA");
    }

    // nanoseconds
    // temp_string = ui->ClkClockNanosecondsValue->text();
    // temp_data = temp_string.toUInt(nullptr, 10);
    temp_data = ntlts->clkClock.Nanoseconds;

    if (fromRegisters)
    {
        temp_data = ntlts->clkRegs.TimeAdjValueLReg;
    }
    // if(false)
    // if (temp_string == "NA")
    if (false)
    {
        // nothing
    }
    else if (0 == write_reg(temp_addr + Ucm_ClkClock_TimeAdjValueLReg, &temp_data))
    {
        // ui->ClkClockNanosecondsValue->setText(QString::number(temp_data));
    }
    else
    {
        // ui->ClkClockTimeAdjCheckBox->setChecked(false);
        // ui->ClkClockNanosecondsValue->setText("NA");
    }

    // offset
    // temp_string = ui->ClkClockOffsetValue->text();
    // int temp_offset = temp_string.toInt(nullptr, 10);

    int temp_offset = ntlts->clkClock.Offset;

    if (fromRegisters)
    {
        temp_offset = ntlts->clkRegs.OffsetAdjValueReg;
    }

    if (temp_offset < 0)
    {
        temp_data = abs(temp_offset) | 0x80000000;
    }
    else
    {
        temp_data = abs(temp_offset);
    }

    if (false)
    {
        // nothing
    }
    else if (0 == write_reg(temp_addr + Ucm_ClkClock_OffsetAdjValueReg, &temp_data))
    {

        temp_offset = temp_data & 0x7FFFFFFF;
        if ((temp_data & 0x80000000) != 0)
        {
            temp_offset = -1 * temp_offset;
        }
        // ui->ClkClockOffsetValue->setText(QString::number(temp_offset));

        // temp_string = ui->ClkClockOffsetIntervalValue->text();

        // temp_data = temp_string.toUInt(nullptr, 10);

        temp_data = ntlts->clkClock.OffsetInterval;

        if (fromRegisters)
        {
            temp_data = ntlts->clkRegs.OffsetAdjIntervalReg;
        }

        if (false)
        {
            // nothing
        }
        else if (0 == write_reg(temp_addr + Ucm_ClkClock_OffsetAdjIntervalReg, &temp_data))
        {
            // ui->ClkClockOffsetIntervalValue->setText(QString::number(temp_data));
        }
        else
        {
            // ui->ClkClockOffsetValue->setText("NA");
            // ui->ClkClockOffsetIntervalValue->setText("NA");
            // ui->ClkClockOffsetAdjCheckBox->setChecked(false);
        }
    }
    else
    {
        // ui->ClkClockOffsetValue->setText("NA");
        // ui->ClkClockOffsetIntervalValue->setText("NA");
        // ui->ClkClockOffsetAdjCheckBox->setChecked(false);
    }

    // drift
    // temp_string = ui->ClkClockDriftValue->text();
    // double temp_drift = temp_string.toDouble(nullptr);

    double temp_drift = ntlts->clkClock.Drift;

    if (fromRegisters)
    {
        temp_drift = ntlts->clkRegs.DriftAdjValueReg;
    }

    if (temp_drift < 0)
    {
        temp_data = abs(((int)temp_drift)) | 0x80000000;
    }
    else
    {
        temp_data = abs(temp_drift);
    }

    // if (temp_string == "NA")
    if (false)
    {
        // nothing
    }
    else if (0 == write_reg(temp_addr + Ucm_ClkClock_DriftAdjValueReg, &temp_data))
    {
        temp_drift = abs(temp_drift);
        temp_drift = temp_drift - ((int)temp_drift);
        temp_data = (int)(temp_drift * 65536.0);
        if (0 == write_reg(temp_addr + Ucm_ClkClock_DriftAdjFractionsReg, &temp_data))
        {
            // nothing might fail
        }

        temp_data = ntlts->clkClock.DriftInterval;

        if (fromRegisters)
        {
            temp_data = ntlts->clkRegs.DriftAdjIntervalReg;
        }

        // temp_string = ui->ClkClockDriftIntervalValue->text();
        // temp_data = temp_string.toUInt(nullptr, 10);
        // if (temp_string == "NA")
        if (false)
        {
            // nothing
        }
        else if (0 == write_reg(temp_addr + Ucm_ClkClock_DriftAdjIntervalReg, &temp_data))
        {
            // nothing
        }
        else
        {
            // ui->ClkClockDriftValue->setText("NA");
            // ui->ClkClockDriftIntervalValue->setText("NA");
            // ui->ClkClockDriftAdjCheckBox->setChecked(false);
        }
    }
    else
    {
        // ui->ClkClockDriftValue->setText("NA");
        // ui->ClkClockDriftIntervalValue->setText("NA");
        // ui->ClkClockDriftAdjCheckBox->setChecked(false);
    }

    // offset outlier config
    // temp_string = ui->ClkClockOffsetOutlierThresholdValue->text();
    // temp_data = (temp_string.toInt(nullptr, 10) & 0x7FFFFFFF);
    // if (temp_data < 1)
    //{
    //    temp_data = 1;
    //}
    // else if (temp_data > 1000000000)
    //{
    //    temp_data = 1000000000;
    //}
    //
    // if (true == ui->ClkClockOffsetOutlierEnableCheckBox->isChecked())
    //{
    //    temp_data |= 0x80000000; // enable
    //}
    //
    // if (temp_string == "NA")
    //{
    //    // nothing
    //}
    // else if (0 == write_reg(temp_addr + Ucm_ClkClock_OffsetOutlierFilterReg, &temp_data))
    //{
    //    // nothing
    //}
    // else
    //{
    //    ui->ClkClockOffsetOutlierThresholdValue->setText("NA");
    //    ui->ClkClockOffsetOutlierEnableCheckBox->setChecked(false);
    //}
    //
    //// drift outlier config
    // temp_string = ui->ClkClockDriftOutlierThresholdValue->text();
    // temp_data = (temp_string.toInt(nullptr, 10) & 0x7FFFFFFF);
    // if (temp_data < 1)
    //{
    //     temp_data = 1;
    // }
    // else if (temp_data > 1000000000)
    //{
    //     temp_data = 1000000000;
    // }
    //
    // if (true == ui->ClkClockDriftOutlierEnableCheckBox->isChecked())
    //{
    //    temp_data |= 0x80000000; // enable
    //}
    //
    // if (temp_string == "NA")
    //{
    //    // nothing
    //}
    // else if (0 == write_reg(temp_addr + Ucm_ClkClock_DriftOutlierFilterReg, &temp_data))
    //{
    //    // nothing
    //}
    // else
    //{
    //    ui->ClkClockDriftOutlierThresholdValue->setText("NA");
    //    ui->ClkClockDriftOutlierEnableCheckBox->setChecked(false);
    //}
    //
    //// offset limiter config
    // temp_string = ui->ClkClockOffsetLimiterLimitValue->text();
    // temp_data = (temp_string.toInt(nullptr, 10) & 0x7FFFFFFF);
    // if (temp_data < 1)
    //{
    //     temp_data = 1;
    // }
    // else if (temp_data > 1000000000)
    //{
    //     temp_data = 1000000000;
    // }
    //
    // if (true == ui->ClkClockOffsetLimiterEnableCheckBox->isChecked())
    //{
    //    temp_data |= 0x80000000; // enable
    //}
    //
    // if (temp_string == "NA")
    //{
    //    // nothing
    //}
    // else if (0 == write_reg(temp_addr + Ucm_ClkClock_OffsetMaxRateChangeLimiterReg, &temp_data))
    //{
    //    // nothing
    //}
    // else
    //{
    //    ui->ClkClockOffsetLimiterLimitValue->setText("NA");
    //    ui->ClkClockOffsetLimiterEnableCheckBox->setChecked(false);
    //}
    //
    //// drift limiter config
    // temp_string = ui->ClkClockDriftLimiterLimitValue->text();
    // double temp_drift_limit = temp_string.toDouble(nullptr);
    // temp_data = (int)(temp_drift_limit * 65536.0);
    // temp_data = temp_data & 0x7FFFFFFF;
    // if (temp_data < 1)
    //{
    //     temp_data = 1;
    // }
    // else if (temp_data > 1000000000)
    //{
    //     temp_data = 1000000000;
    // }
    //
    // if (true == ui->ClkClockDriftLimiterEnableCheckBox->isChecked())
    //{
    //    temp_data |= 0x80000000; // enable
    //}
    //
    // if (temp_string == "NA")
    //{
    //    // nothing
    //}
    // else if (0 == write_reg(temp_addr + Ucm_ClkClock_DriftMaxRateChangeLimiterReg, &temp_data))
    //{
    //    // nothing
    //}
    // else
    //{
    //    ui->ClkClockDriftLimiterLimitValue->setText("NA");
    //    ui->ClkClockDriftLimiterEnableCheckBox->setChecked(false);
    //}
    //
    //// PI servo parameters
    // if ((ui->ClkClockPiOffsetMulPValue->text() == "NA") ||
    //     (ui->ClkClockPiOffsetDivPValue->text() == "NA") ||
    //     (ui->ClkClockPiOffsetMulIValue->text() == "NA") ||
    //     (ui->ClkClockPiOffsetDivIValue->text() == "NA"))
    //{
    //     ui->ClkClockPiOffsetMulPValue->setText("NA");
    //     ui->ClkClockPiOffsetDivPValue->setText("NA");
    //     ui->ClkClockPiOffsetMulIValue->setText("NA");
    //     ui->ClkClockPiOffsetDivIValue->setText("NA");
    //     ui->ClkClockPiSetCustomParametersCheckBox->setChecked(false);
    // }
    // else
    //{
    //     int temp_mul;
    //     int temp_div;
    //
    //    temp_string = ui->ClkClockPiOffsetMulPValue->text();
    //    temp_mul = temp_string.toInt(nullptr, 10);
    //    temp_string = ui->ClkClockPiOffsetDivPValue->text();
    //    temp_div = temp_string.toInt(nullptr, 10);
    //
    //    if (temp_mul > 1024)
    //    {
    //        temp_mul = 1024;
    //    }
    //    else if (temp_mul < 0)
    //    {
    //        temp_mul = 0;
    //    }
    //
    //    if (temp_div > 1024)
    //    {
    //        temp_div = 1024;
    //    }
    //    else if (temp_div <= 0)
    //    {
    //        temp_div = 1;
    //    }
    //
    //    temp_data = (0x00010000 * temp_mul) / temp_div;
    //    if (temp_data >= 0x10000)
    //    {
    //        temp_data = 0xFFFF; // as close to one as possible
    //    }
    //
    //    if (0 == write_reg(temp_addr + Ucm_ClkClock_ServoOffsetFactorPReg, &temp_data))
    //    {
    //        ui->ClkClockPiOffsetMulPValue->setText(QString::number(temp_mul));
    //        ui->ClkClockPiOffsetDivPValue->setText(QString::number(temp_div));
    //    }
    //    else
    //    {
    //        ui->ClkClockPiOffsetMulPValue->setText("NA");
    //        ui->ClkClockPiOffsetDivPValue->setText("NA");
    //        ui->ClkClockPiSetCustomParametersCheckBox->setChecked(false);
    //    }
    //
    //    temp_string = ui->ClkClockPiOffsetMulIValue->text();
    //    temp_mul = temp_string.toInt(nullptr, 10);
    //    temp_string = ui->ClkClockPiOffsetDivIValue->text();
    //    temp_div = temp_string.toInt(nullptr, 10);
    //
    //    if (temp_mul > 1024)
    //    {
    //        temp_mul = 1024;
    //    }
    //    else if (temp_mul < 0)
    //    {
    //        temp_mul = 0;
    //    }
    //
    //    if (temp_div > 1024)
    //    {
    //        temp_div = 1024;
    //    }
    //    else if (temp_div <= 0)
    //    {
    //        temp_div = 1;
    ////    }
    ////
    ////    temp_data = (0x00010000 * temp_mul) / temp_div;
    ////    if (temp_data >= 0x10000)
    //    {
    //        temp_data = 0xFFFF; // as close to one as possible
    //    }
    //
    //    if (0 == write_reg(temp_addr + Ucm_ClkClock_ServoOffsetFactorIReg, &temp_data))
    //    {
    //        ui->ClkClockPiOffsetMulIValue->setText(QString::number(temp_mul));
    //        ui->ClkClockPiOffsetDivIValue->setText(QString::number(temp_div));
    //    }
    //    else
    //    {
    //        ui->ClkClockPiOffsetMulIValue->setText("NA");
    //        ui->ClkClockPiOffsetDivIValue->setText("NA");
    //        ui->ClkClockPiSetCustomParametersCheckBox->setChecked(false);
    //    }
    //}
    //
    // if ((ui->ClkClockPiDriftMulPValue->text() == "NA") ||
    //    (ui->ClkClockPiDriftDivPValue->text() == "NA") ||
    //    (ui->ClkClockPiDriftMulIValue->text() == "NA") ||
    //    (ui->ClkClockPiDriftDivIValue->text() == "NA"))
    //{
    //    ui->ClkClockPiDriftMulPValue->setText("NA");
    //    ui->ClkClockPiDriftDivPValue->setText("NA");
    //    ui->ClkClockPiDriftMulIValue->setText("NA");
    //    ui->ClkClockPiDriftDivIValue->setText("NA");
    //    ui->ClkClockPiSetCustomParametersCheckBox->setChecked(false);
    //}
    // else
    //{
    //    int temp_mul;
    //    int temp_div;
    //
    //    temp_string = ui->ClkClockPiDriftMulPValue->text();
    //    temp_mul = temp_string.toInt(nullptr, 10);
    //    temp_string = ui->ClkClockPiDriftDivPValue->text();
    //    temp_div = temp_string.toInt(nullptr, 10);
    //
    //    if (temp_mul > 1024)
    //    {
    //        temp_mul = 1024;
    //    }
    //    else if (temp_mul < 0)
    //    {
    //        temp_mul = 0;
    //    }
    //
    //    if (temp_div > 1024)
    //    {
    //        temp_div = 1024;
    //    }
    //    else if (temp_div <= 0)
    //    {
    //        temp_div = 1;
    //    }
    //
    //    temp_data = (0x00010000 * temp_mul) / temp_div;
    //
    //    if (0 == write_reg(temp_addr + Ucm_ClkClock_ServoDriftFactorPReg, &temp_data))
    //    {
    //        ui->ClkClockPiDriftMulPValue->setText(QString::number(temp_mul));
    //        ui->ClkClockPiDriftDivPValue->setText(QString::number(temp_div));
    //    }
    //    else
    //    {
    //        ui->ClkClockPiDriftMulPValue->setText("NA");
    //        ui->ClkClockPiDriftDivPValue->setText("NA");
    //        ui->ClkClockPiSetCustomParametersCheckBox->setChecked(false);
    //    }
    //
    //    temp_string = ui->ClkClockPiDriftMulIValue->text();
    //    temp_mul = temp_string.toInt(nullptr, 10);
    //    temp_string = ui->ClkClockPiDriftDivIValue->text();
    //    temp_div = temp_string.toInt(nullptr, 10);
    //
    //    if (temp_mul > 1024)
    //    {
    //        temp_mul = 1024;
    //    }
    //    else if (temp_mul < 0)
    //    {
    //        temp_mul = 0;
    //    }
    //
    //    if (temp_div > 1024)
    //    {
    //        temp_div = 1024;
    //    }
    //    else if (temp_div <= 0)
    //    {
    //        temp_div = 1;
    //    }
    //
    //    temp_data = (0x00010000 * temp_mul) / temp_div;
    //
    //    if (0 == write_reg(temp_addr + Ucm_ClkClock_ServoDriftFactorIReg, &temp_data))
    //    {
    //        ui->ClkClockPiDriftMulIValue->setText(QString::number(temp_mul));
    //        ui->ClkClockPiDriftDivIValue->setText(QString::number(temp_div));
    //    }
    //    else
    //    {
    //        ui->ClkClockPiDriftMulIValue->setText("NA");
    //        ui->ClkClockPiDriftDivIValue->setText("NA");
    //        ui->ClkClockPiSetCustomParametersCheckBox->setChecked(false);
    //    }
    //}
    //
    //// holdover samples
    // temp_string = ui->ClkClockHoldoverMaxSamplesValue->text();
    // temp_data = temp_string.toUInt(nullptr, 10);
    // if (temp_string == "NA")
    //{
    //     // nothing
    // }
    // else
    //{
    //     if (temp_data > 65536)
    //     {
    //         temp_data = 65536;
    //     }
    //
    //    if (0 == write_reg(temp_addr + Ucm_ClkClock_HoldoverMaxSamplesReg, &temp_data))
    //    {
    //        ui->ClkClockHoldoverMaxSamplesValue->setText(QString::number(temp_data));
    //    }
    //    else
    //    {
    //        ui->ClkClockHoldoverMaxSamplesValue->setText("NA");
    //    }
    //}

    temp_data = 0x00000000;
    if (ntlts->clkClock.Enable)
    {
        temp_data |= 0x00000001; // enable
    }
    if (ntlts->clkClock.TimeAdjEnable)
    {
        temp_data |= 0x00000002; // set time
    }
    // if (ntlts->clkClock.OffsetAdjEnable)
    //{
    //     temp_data |= 0x00000004; // set offset
    // }
    // if (ntlts->clkClock.DriftAdj)
    //{
    //     temp_data |= 0x00000008; // set drift
    // }
    // if (ntlts->clkClock.PiSetCustomParameters)
    //{
    //     temp_data |= 0x00000100; // set servo parameters
    // }
    // if (ntlts->clkClock.HoldoverOffset)
    //{
    //     temp_data |= 0x00020000; // include offset in holdover
    // }
    // if (ntlts->clkClock.HoldoverEnable)
    //{
    //     temp_data |= 0x00010000; // enable advance holdover setting
    // }

    temp_data = ntlts->clkClock.Enable;

    if (fromRegisters)
    {
        temp_data = ntlts->clkRegs.ControlReg;
    }

    if (0 == write_reg(temp_addr + Ucm_ClkClock_ControlReg, &temp_data))
    {
        // nothing
    }
    else
    {
        // ui->ClkClockSecondsValue->setText("NA");
        // ui->ClkClockNanosecondsValue->setText("NA");
    }

    // ui->ClkClockTimeAdjCheckBox->setChecked(false);
    // ui->ClkClockOffsetAdjCheckBox->setChecked(false);
    // ui->ClkClockDriftAdjCheckBox->setChecked(false);
    // ui->ClkClockPiSetCustomParametersCheckBox->setChecked(false);

    return 0;
}

#endif