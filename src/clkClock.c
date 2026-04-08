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

uint8_t clk_clock_read_values(NTL_TS_T *ntlts)
{
    uint32_t temp_data = 0;
    uint32_t temp_addr = 0;

    double temp_drift = 0;
    double temp_offset = 0;

    temp_addr = ntlts->clkRegs.StartAddr;

    // enabled
    if (0 == read_reg(temp_addr + Ucm_ClkClock_ControlReg, &temp_data))
    {
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
    }
    else
    {
        // ntlts->clkClock.InSyncThreshold->setText("NA");
    }

    // offset
    if (0 == read_reg(temp_addr + Ucm_ClkClock_OffsetAdjValueReg, &temp_data))
    {
        int32_t temp_offset = temp_data & 0x7FFFFFFF;
        if ((temp_data & 0x80000000) != 0)
        {
            temp_offset = -1 * temp_offset;
        }
        // ui->ClkClockOffsetValue->setText(QString::number(temp_offset));
        ntlts->clkClock.Offset = temp_offset;

        //    if (0 == read_reg(temp_addr + Ucm_ClkClock_OffsetAdjIntervalReg, &temp_data))
        //    {
        //        ui->ClkClockOffsetIntervalValue->setText(QString::number(temp_data));
        //    }
        //    else
        //    {
        //        ui->ClkClockOffsetValue->setText("NA");
        //        ui->ClkClockOffsetIntervalValue->setText("NA");
        //        ui->ClkClockOffsetAdjCheckBox->setChecked(false);
        //    }
        //}
        // else
        //{
        //    ui->ClkClockOffsetValue->setText("NA");
        //    ui->ClkClockOffsetIntervalValue->setText("NA");
        //    ui->ClkClockOffsetAdjCheckBox->setChecked(false);
        //}
    }
    // drift
    if (0 == read_reg(temp_addr + Ucm_ClkClock_DriftAdjValueReg, &temp_data))
    {
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

        //     if (0 == read_reg(temp_addr + Ucm_ClkClock_DriftAdjIntervalReg, &temp_data))
        //     {
        //         ui->ClkClockDriftIntervalValue->setText(QString::number(temp_data));
        //     }
        //     else
        //     {
        //         ui->ClkClockDriftValue->setText("NA");
        //         ui->ClkClockDriftIntervalValue->setText("NA");
        //         ui->ClkClockDriftAdjCheckBox->setChecked(false);
        //     }
        // }
        // else
        // {
        //     ui->ClkClockDriftValue->setText("NA");
        //     ui->ClkClockDriftIntervalValue->setText("NA");
        //     ui->ClkClockDriftAdjCheckBox->setChecked(false);
        // }
    }
    // source
    if (0 == read_reg(temp_addr + Ucm_ClkClock_SelectReg, &temp_data))
    {
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

/*

uint8_t clk_clock_write_values(NTL_TS_T *ntlts, uint8_t fromRegisters)
{
    uint32_t temp_data = 0;
    uint32_t temp_addr = 0;

    // source
    // temp_string = ntlts->clkClock.Source->currentText();
    if (strncmp(ntlts->clkClock.Source, "NONE", sizeof("NONE")) == 0)
    {
        temp_data = 0x00000000;
    }
    else if (strncmp(ntlts->clkClock.Source, "TOD", sizeof("TOD")) == 0)
    {
        temp_data = 0x00000001;
    }
    else if (strncmp(ntlts->clkClock.Source, "IRIG", sizeof("IRIG")) == 0)
    {
        temp_data = 0x00000002;
    }
    else if (strncmp(ntlts->clkClock.Source, "PPS", sizeof("PPS")) == 0)
    {
        temp_data = 0x00000003;
    }
    else if (strncmp(ntlts->clkClock.Source, "PTP", sizeof("PTP")) == 0)
    {
        temp_data = 0x00000004;
    }
    else if (strncmp(ntlts->clkClock.Source, "RTC", sizeof("RTC")) == 0)
    {
        temp_data = 0x00000005;
    }
    else if (strncmp(ntlts->clkClock.Source, "DCF", sizeof("DCF")) == 0)
    {
        temp_data = 0x00000006;
    }
    else if (strncmp(ntlts->clkClock.Source, "NTP", sizeof("NTP")) == 0)
    {
        temp_data = 0x00000007;
    }
    else if (strncmp(ntlts->clkClock.Source, "REG", sizeof("REG")) == 0)
    {
        temp_data = 0x000000FE;
    }
    else if (strncmp(ntlts->clkClock.Source, "EXT", sizeof("EXT")) == 0)
    {
        temp_data = 0x000000FF;
    }
    else
    {
        temp_data = 0x00000000;
    }

    if (strncmp(ntlts->clkClock.Source, "NA", sizeof("NA")) == 0)
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
        //     ntlts->clkClock.Source->setCurrentText("NONE");
        //     break;
        // case 0x00000001:
        //     ntlts->clkClock.Source->setCurrentText("TOD");
        //     break;
        // case 0x00000002:
        //     ntlts->clkClock.Source->setCurrentText("IRIG");
        //     break;
        // case 0x00000003:
        //     ntlts->clkClock.Source->setCurrentText("PPS");
        //     break;
        // case 0x00000004:
        //     ntlts->clkClock.Source->setCurrentText("PTP");
        //     break;
        // case 0x00000005:
        //     ntlts->clkClock.Source->setCurrentText("RTC");
        //     break;
        // case 0x00000006:
        //     ntlts->clkClock.Source->setCurrentText("DCF");
        //     break;
        // case 0x00000007:
        //     ntlts->clkClock.Source->setCurrentText("NTP");
        //     break;
        // case 0x000000FE:
        //     ntlts->clkClock.Source->setCurrentText("REG");
        //     break;
        // case 0x000000FF:
        //     ntlts->clkClock.Source->setCurrentText("EXT");
        //     break;
        // default:
        //     ntlts->clkClock.Source->setCurrentText("NA");
        //     break;
        // }
    }
    else
    {
        ntlts->clkClock.Source->setCurrentText("NA");
    }

    // in sync Threshold
    temp_string = ntlts->clkClock.InSyncThreshold->text();
    temp_data = temp_string.toUInt(nullptr, 10);
    if (temp_string == "NA")
    {
        // nothing
    }
    else if (0 == write_reg(temp_addr + Ucm_ClkClock_InSyncThresholdReg, &temp_data))
    {
        ntlts->clkClock.InSyncThreshold = temp_data;
    }
    else
    {
        ntlts->clkClock.InSyncThreshold->setText("NA");
    }

    // seconds
    temp_string = ntlts->clkClock.Seconds->text();
    temp_data = temp_string.toUInt(nullptr, 10);
    if (strncmp(ntlts->clkClock.Source, "NA", sizeof("NA")) == 0)
    {
        // nothing
    }
    else if (0 == write_reg(temp_addr + Ucm_ClkClock_TimeAdjValueHReg, &temp_data))
    {
        ntlts->clkClock.Seconds = temp_data;
        ;
    }
    else
    {
        ntlts->clkClock.TimeAdj = 0;
        ntlts->clkClock.Seconds->setText("NA");
    }

    // nanoseconds
    temp_string = ntlts->clkClock.Nanoseconds->text();
    temp_data = temp_string.toUInt(nullptr, 10);
    if (temp_string == "NA")
    {
        // nothing
    }
    else if (0 == write_reg(temp_addr + Ucm_ClkClock_TimeAdjValueLReg, &temp_data))
    {
        ntlts->clkClock.Nanoseconds = temp_data;
        ;
    }
    else
    {
        ntlts->clkClock.TimeAdj = 0;
        ntlts->clkClock.Nanoseconds->setText("NA");
    }

    // offset
    temp_string = ntlts->clkClock.Offset->text();
    int temp_offset = temp_string.toInt(nullptr, 10);
    if (temp_offset < 0)
    {
        temp_data = abs(temp_offset) | 0x80000000;
    }
    else
    {
        temp_data = abs(temp_offset);
    }

    if (temp_string == "NA")
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
        ntlts->clkClock.Offset->setText(QString::number(temp_offset));

        temp_string = ntlts->clkClock.OffsetInterval->text();
        temp_data = temp_string.toUInt(nullptr, 10);
        if (temp_string == "NA")
        {
            // nothing
        }
        else if (0 == write_reg(temp_addr + Ucm_ClkClock_OffsetAdjIntervalReg, &temp_data))
        {
            ntlts->clkClock.OffsetInterval = temp_data;
            ;
        }
        else
        {
            ntlts->clkClock.Offset->setText("NA");
            ntlts->clkClock.OffsetInterval->setText("NA");
            ntlts->clkClock.OffsetAdj = 0;
        }
    }
    else
    {
        ntlts->clkClock.Offset->setText("NA");
        ntlts->clkClock.OffsetInterval->setText("NA");
        ntlts->clkClock.OffsetAdj = 0;
    }

    // drift
    temp_string = ntlts->clkClock.Drift->text();
    int temp_drift = temp_string.toInt(nullptr, 10);
    if (temp_drift < 0)
    {
        temp_data = abs(temp_drift) | 0x80000000;
    }
    else
    {
        temp_data = abs(temp_drift);
    }

    if (temp_string == "NA")
    {
        // nothing
    }
    else if (0 == write_reg(temp_addr + Ucm_ClkClock_DriftAdjValueReg, &temp_data))
    {
        temp_drift = temp_data & 0x7FFFFFFF;
        if ((temp_data & 0x80000000) != 0)
        {
            temp_drift = -1 * temp_drift;
        }
        ntlts->clkClock.Drift->setText(QString::number(temp_drift));

        temp_string = ntlts->clkClock.DriftInterval->text();
        temp_data = temp_string.toUInt(nullptr, 10);
        if (temp_string == "NA")
        {
            // nothing
        }
        else if (0 == write_reg(temp_addr + Ucm_ClkClock_DriftAdjIntervalReg, &temp_data))
        {
            ntlts->clkClock.DriftInterval = temp_data;
            ;
        }
        else
        {
            ntlts->clkClock.Drift->setText("NA");
            ntlts->clkClock.DriftInterval->setText("NA");
            ntlts->clkClock.DriftAdj = 0;
        }
    }
    else
    {
        ntlts->clkClock.Drift->setText("NA");
        ntlts->clkClock.DriftInterval->setText("NA");
        ntlts->clkClock.DriftAdj = 0;
    }

    // PI servo parameters
    if ((ntlts->clkClock.PiOffsetMulP->text() == "NA") ||
        (ntlts->clkClock.PiOffsetDivP->text() == "NA") ||
        (ntlts->clkClock.PiOffsetMulI->text() == "NA") ||
        (ntlts->clkClock.PiOffsetDivI->text() == "NA"))
    {
        ntlts->clkClock.PiOffsetMulP->setText("NA");
        ntlts->clkClock.PiOffsetDivP->setText("NA");
        ntlts->clkClock.PiOffsetMulI->setText("NA");
        ntlts->clkClock.PiOffsetDivI->setText("NA");
        ntlts->clkClock.PiSetCustomParameters = 0;
    }
    else
    {
        int temp_mul;
        int temp_div;

        temp_string = ntlts->clkClock.PiOffsetMulP->text();
        temp_mul = temp_string.toInt(nullptr, 10);
        temp_string = ntlts->clkClock.PiOffsetDivP->text();
        temp_div = temp_string.toInt(nullptr, 10);

        if (temp_mul > 1024)
        {
            temp_mul = 1024;
        }
        else if (temp_mul < 0)
        {
            temp_mul = 0;
        }

        if (temp_div > 1024)
        {
            temp_div = 1024;
        }
        else if (temp_div <= 0)
        {
            temp_div = 1;
        }

        temp_data = (0x00010000 * temp_mul) / temp_div;
        if (temp_data >= 0x10000)
        {
            temp_data = 0xFFFF; // as close to one as possible
        }

        if (0 == write_reg(temp_addr + Ucm_ClkClock_ServoOffsetFactorPReg, &temp_data))
        {
            ntlts->clkClock.PiOffsetMulP->setText(QString::number(temp_mul));
            ntlts->clkClock.PiOffsetDivP->setText(QString::number(temp_div));
        }
        else
        {
            ntlts->clkClock.PiOffsetMulP->setText("NA");
            ntlts->clkClock.PiOffsetDivP->setText("NA");
            ntlts->clkClock.PiSetCustomParameters = 0;
        }

        temp_string = ntlts->clkClock.PiOffsetMulI->text();
        temp_mul = temp_string.toInt(nullptr, 10);
        temp_string = ntlts->clkClock.PiOffsetDivI->text();
        temp_div = temp_string.toInt(nullptr, 10);

        if (temp_mul > 1024)
        {
            temp_mul = 1024;
        }
        else if (temp_mul < 0)
        {
            temp_mul = 0;
        }

        if (temp_div > 1024)
        {
            temp_div = 1024;
        }
        else if (temp_div <= 0)
        {
            temp_div = 1;
        }

        temp_data = (0x00010000 * temp_mul) / temp_div;
        if (temp_data >= 0x10000)
        {
            temp_data = 0xFFFF; // as close to one as possible
        }

        if (0 == write_reg(temp_addr + Ucm_ClkClock_ServoOffsetFactorIReg, &temp_data))
        {
            ntlts->clkClock.PiOffsetMulI->setText(QString::number(temp_mul));
            ntlts->clkClock.PiOffsetDivI->setText(QString::number(temp_div));
        }
        else
        {
            ntlts->clkClock.PiOffsetMulI->setText("NA");
            ntlts->clkClock.PiOffsetDivI->setText("NA");
            ntlts->clkClock.PiSetCustomParameters = 0;
        }
    }

    if ((ntlts->clkClock.PiDriftMulP->text() == "NA") ||
        (ntlts->clkClock.PiDriftDivP->text() == "NA") ||
        (ntlts->clkClock.PiDriftMulI->text() == "NA") ||
        (ntlts->clkClock.PiDriftDivI->text() == "NA"))
    {
        ntlts->clkClock.PiDriftMulP->setText("NA");
        ntlts->clkClock.PiDriftDivP->setText("NA");
        ntlts->clkClock.PiDriftMulI->setText("NA");
        ntlts->clkClock.PiDriftDivI->setText("NA");
        ntlts->clkClock.PiSetCustomParameters = 0;
    }
    else
    {
        int temp_mul;
        int temp_div;

        temp_string = ntlts->clkClock.PiDriftMulP->text();
        temp_mul = temp_string.toInt(nullptr, 10);
        temp_string = ntlts->clkClock.PiDriftDivP->text();
        temp_div = temp_string.toInt(nullptr, 10);

        if (temp_mul > 1024)
        {
            temp_mul = 1024;
        }
        else if (temp_mul < 0)
        {
            temp_mul = 0;
        }

        if (temp_div > 1024)
        {
            temp_div = 1024;
        }
        else if (temp_div <= 0)
        {
            temp_div = 1;
        }

        temp_data = (0x00010000 * temp_mul) / temp_div;

        if (0 == write_reg(temp_addr + Ucm_ClkClock_ServoDriftFactorPReg, &temp_data))
        {
            ntlts->clkClock.PiDriftMulP->setText(QString::number(temp_mul));
            ntlts->clkClock.PiDriftDivP->setText(QString::number(temp_div));
        }
        else
        {
            ntlts->clkClock.PiDriftMulP->setText("NA");
            ntlts->clkClock.PiDriftDivP->setText("NA");
            ntlts->clkClock.PiSetCustomParameters = 0;
        }

        temp_string = ntlts->clkClock.PiDriftMulI->text();
        temp_mul = temp_string.toInt(nullptr, 10);
        temp_string = ntlts->clkClock.PiDriftDivI->text();
        temp_div = temp_string.toInt(nullptr, 10);

        if (temp_mul > 1024)
        {
            temp_mul = 1024;
        }
        else if (temp_mul < 0)
        {
            temp_mul = 0;
        }

        if (temp_div > 1024)
        {
            temp_div = 1024;
        }
        else if (temp_div <= 0)
        {
            temp_div = 1;
        }

        temp_data = (0x00010000 * temp_mul) / temp_div;

        if (0 == write_reg(temp_addr + Ucm_ClkClock_ServoDriftFactorIReg, &temp_data))
        {
            ntlts->clkClock.PiDriftMulI->setText(QString::number(temp_mul));
            ntlts->clkClock.PiDriftDivI->setText(QString::number(temp_div));
        }
        else
        {
            ntlts->clkClock.PiDriftMulI->setText("NA");
            ntlts->clkClock.PiDriftDivI->setText("NA");
            ntlts->clkClock.PiSetCustomParameters = 0;
        }
    }

    temp_data = 0x00000000;
    if (true == ntlts->clkClock.Enable->isChecked())
    {
        temp_data |= 0x00000001; // enable
    }
    if (true == ntlts->clkClock.TimeAdj->isChecked())
    {
        temp_data |= 0x00000002; // set time
    }
    if (true == ntlts->clkClock.OffsetAdj->isChecked())
    {
        temp_data |= 0x00000004; // set offset
    }
    if (true == ntlts->clkClock.DriftAdj->isChecked())
    {
        temp_data |= 0x00000008; // set drift
    }
    if (true == ntlts->clkClock.PiSetCustomParameters->isChecked())
    {
        temp_data |= 0x00000100; // set servo parameters
    }

    if (0 == write_reg(temp_addr + Ucm_ClkClock_ControlReg, &temp_data))
    {
        // nothing
    }
    else
    {
        ntlts->clkClock.Seconds->setText("NA");
        ntlts->clkClock.Nanoseconds->setText("NA");
    }

    ntlts->clkClock.TimeAdj = 0;
    ntlts->clkClock.OffsetAdj = 0;
    ntlts->clkClock.DriftAdj = 0;
    ntlts->clkClock.PiSetCustomParameters = 0;
}

*/

#endif