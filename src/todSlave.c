#include "ntl_uart.h"
// #include "uartCircBuff.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"

int8_t tod_slave_read_values(NTL_TS_T *ntlts)

{
    uint32_t temp_data = 0;
    uint32_t temp_addr = 0;

    temp_addr = ntlts->todRegs.StartAddr;

    if (0 == read_reg(temp_addr + Ucm_TodSlave_ControlReg, &temp_data))
    {

    	ntlts->todRegs.ControlReg = temp_data;
        // enabled
        if ((temp_data & 0x00000001) == 0)
        {
            ntlts->todSlave.Enable = 0;
        }
        else
        {
            // ntlts->TodSlaveEnable = 1;;
            ntlts->todSlave.Enable = 1;
        }

        // protocol
        switch ((temp_data >> 28) & 0x7)
        {
        case 0:
            // ntlts->TodSlaveProtocolValue->setCurrentText("NMEA");
            snprintf(ntlts->todSlave.Protocol, sizeof(ntlts->todSlave.Protocol), "NMEA");
            break;
        case 1:
            // ntlts->TodSlaveProtocolValue->setCurrentText("UBX");
            snprintf(ntlts->todSlave.Protocol, sizeof(ntlts->todSlave.Protocol), "UBX");

            break;
        case 2:
            // ntlts->TodSlaveProtocolValue->setCurrentText("TSIP");
            snprintf(ntlts->todSlave.Protocol, sizeof(ntlts->todSlave.Protocol), "TSIP");

            break;
        case 3:
            // ntlts->TodSlaveProtocolValue->setCurrentText("ESIP");
            snprintf(ntlts->todSlave.Protocol, sizeof(ntlts->todSlave.Protocol), "ESIP");

            break;
        default:
            // ntlts->TodSlaveProtocolValue->setCurrentText("NA");
            snprintf(ntlts->todSlave.Protocol, sizeof(ntlts->todSlave.Protocol), "NA");

            break;
        }

        // gnss
        switch ((temp_data >> 24) & 0xF)
        {
        case 0:
            snprintf(ntlts->todSlave.Gnss, sizeof(ntlts->todSlave.Gnss), "ALL");
            break;
        case 1:
            snprintf(ntlts->todSlave.Gnss, sizeof(ntlts->todSlave.Gnss), "COMBINED");
            break;
        case 2:
            snprintf(ntlts->todSlave.Gnss, sizeof(ntlts->todSlave.Gnss), "GPS");
            break;
        case 3:
            snprintf(ntlts->todSlave.Gnss, sizeof(ntlts->todSlave.Gnss), "GLONASS");
            break;
        case 4:
            snprintf(ntlts->todSlave.Gnss, sizeof(ntlts->todSlave.Gnss), "GALILEO");
            break;
        case 5:
            snprintf(ntlts->todSlave.Gnss, sizeof(ntlts->todSlave.Gnss), "BEIDOU");
            break;
        default:
            snprintf(ntlts->todSlave.Gnss, sizeof(ntlts->todSlave.Gnss), "NA");
            break;
        }

        // disabled msg
        // ntlts->TodSlaveMsgDisableValue->setText(QString("0x%1").arg(((temp_data >> 16) & 0xFF), 2, 16, QLatin1Char('0')));
    }
    else
    {
        // ntlts->TodSlaveProtocolValue->setCurrentText("NA");
        snprintf(ntlts->todSlave.Protocol, sizeof(ntlts->todSlave.Protocol), "NA");

        // ntlts->TodSlaveGnssValue->setCurrentText("NA");
        snprintf(ntlts->todSlave.Gnss, sizeof(ntlts->todSlave.Gnss), "NA");

        // ntlts->TodSlaveMsgDisableValue->setText("NA");
        // ntlts->TodSlaveEnable = 0;
        ntlts->todSlave.Enable = 0;
    }

    // correction
    if (0 == read_reg(temp_addr + Ucm_TodSlave_CorrectionReg, &temp_data))
    {
        // ntlts->TodSlaveCorrectionValue->setText(QString("0x%1").arg(temp_data, 8, 16, QLatin1Char('0')));
    	ntlts->todRegs.CorrectionReg = temp_data;
        ntlts->todSlave.Correction = temp_data;
    }
    else
    {
        // ntlts->TodSlaveCorrectionValue->setText("NA");
    }

    // baud rate
    if (0 == read_reg(temp_addr + Ucm_TodSlave_UartBaudRateReg, &temp_data))
    {

    	ntlts->todRegs.UartBaudRateReg = temp_data;
        switch (temp_data)
        {
        case 0:
            // ntlts->TodSlaveBaudRateValue->setCurrentText("1200");
            ntlts->todSlave.BaudRate = 1200;
            break;
        case 1:
            // ntlts->TodSlaveBaudRateValue->setCurrentText("2400");
            ntlts->todSlave.BaudRate = 2400;
            break;
        case 2:
            // ntlts->TodSlaveBaudRateValue->setCurrentText("4800");
            ntlts->todSlave.BaudRate = 4800;
            break;
        case 3:
            // ntlts->TodSlaveBaudRateValue->setCurrentText("9600");
            ntlts->todSlave.BaudRate = 9600;
            break;
        case 4:
            // ntlts->TodSlaveBaudRateValue->setCurrentText("19200");
            ntlts->todSlave.BaudRate = 19200;
            break;
        case 5:
            // ntlts->TodSlaveBaudRateValue->setCurrentText("38400");
            ntlts->todSlave.BaudRate = 38400;
            break;
        case 6:
            // ntlts->TodSlaveBaudRateValue->setCurrentText("57600");
            ntlts->todSlave.BaudRate = 57600;
            break;
        case 7:
            // ntlts->TodSlaveBaudRateValue->setCurrentText("115200");
            ntlts->todSlave.BaudRate = 115200;
            break;
        case 8:
            // ntlts->TodSlaveBaudRateValue->setCurrentText("230400");
            ntlts->todSlave.BaudRate = 230400;
            break;
        case 9:
            // ntlts->TodSlaveBaudRateValue->setCurrentText("460800");
            ntlts->todSlave.BaudRate = 460800;
            break;
        case 10:
            // ntlts->TodSlaveBaudRateValue->setCurrentText("921600");
            ntlts->todSlave.BaudRate = 921600;
            break;
        case 11:
            // ntlts->TodSlaveBaudRateValue->setCurrentText("1000000");
            ntlts->todSlave.BaudRate = 1000000;
            break;
        case 12:
            // ntlts->TodSlaveBaudRateValue->setCurrentText("2000000");
            ntlts->todSlave.BaudRate = 2000000;
            break;
        default:
            // ntlts->TodSlaveBaudRateValue->setCurrentText("NA");
            ntlts->todSlave.BaudRate = 38400;

            break;
        }
    }
    else
    {
        // ntlts->TodSlaveBaudRateValue->setCurrentText("NA");
        ntlts->todSlave.BaudRate = 38400;
    }

    // polarity
    if (0 == read_reg(temp_addr + Ucm_TodSlave_PolarityReg, &temp_data))
    {
    	ntlts->todRegs.PolarityReg = temp_data;
        if ((temp_data & 0x00000001) == 0)
        {
            // ntlts->TodSlaveInverted = 1;;
            ntlts->todSlave.Inverted = 1;
        }
        else
        {
            // ntlts->TodSlaveInverted = 0;
            ntlts->todSlave.Inverted = 0;
        }
    }
    else
    {
        // ntlts->TodSlaveInverted = 0;
        ntlts->todSlave.Inverted = 0;
    }

    // input ok
    if (0 == read_reg(temp_addr + Ucm_TodSlave_StatusReg, &temp_data))
    {
    	ntlts->todRegs.StatusReg = temp_data;
        if (temp_data == 0)
        {
            ntlts->todSlave.InputOk = 1;

        }
        else
        {
            ntlts->todSlave.InputOk = 0;
        }

        // clear after read
        write_reg(temp_addr + Ucm_TodSlave_StatusReg, &temp_data);
    }
    else
    {
        ntlts->todSlave.InputOk = 0;
    }

    // utc status
    if (0 == read_reg(temp_addr + Ucm_TodSlave_UtcStatusReg, &temp_data))
    {
        // ntlts->todSlave.UtcOffset = ->setText( QString::number(  ((signed char)(temp_data & 0xFF))   )   ) ;
        ntlts->todSlave.UtcOffset = ((signed char)(temp_data & 0xFF));
        if ((temp_data & 0x00000100) == 0)
        {
            ntlts->todSlave.UtcInfoValid = 0;
        }
        else
        {
            ntlts->todSlave.UtcInfoValid = 1;
        }

        // if ((temp_data & 0x00001000) == 0)
        //{
        //     ntlts->todSlave.LeapAnnounce = 0;
        // }
        // else
        //{
        //     ntlts->todSlave.LeapAnnounce = 1;
        // }
        //
        // if ((temp_data & 0x00002000) == 0)
        //{
        //    ntlts->todSlave.Leap59 = 0;
        //}
        // else
        //{
        //    ntlts->todSlave.Leap59 = 1;
        //}
        // if ((temp_data & 0x00004000) == 0)
        //{
        //    ntlts->todSlave.Leap61 = 0;
        //}
        // else
        //{
        //    ntlts->todSlave.Leap61 = 1;
        //}
        //
        // if ((temp_data & 0x00010000) == 0)
        //{
        //    ntlts->todSlave.LeapInfoValid = 0;
        //}
        // else
        //{
        //    ntlts->todSlave.LeapInfoValid = 1;
        //}
    }
    else
    {
        /// ntlts->todSlave.UtcOffset->setText("NA");
        ntlts->todSlave.UtcInfoValid = 0;
        // ntlts->todSlave.LeapAnnounce = 0;
        // ntlts->todSlave.Leap59 = 0;
        // ntlts->todSlave.Leap61 = 0;
        // ntlts->todSlave.LeapInfoValid = 0;
    }

    // time to leap second
    // if (0 == read_reg(temp_addr + Ucm_TodSlave_TimeToLeapSecondReg, &temp_data))
    //{
    //    ntlts->todSlave.TimeToLeapValue->setText(QString::number(((signed int)(temp_data))));
    //}
    // else
    //{
    //    ntlts->todSlave.TimeToLeapValue->setText("NA");
    //}

    if (0 == read_reg(temp_addr + Ucm_TodSlave_GnssStatus_Reg_Con, &temp_data))
    {
        // gnss fix ok
        if ((temp_data & 0x00010000) == 0)
        {
            ntlts->todSlave.GnssFix = 0;
        }
        else
        {
            ntlts->todSlave.GnssFix = 1;
        }

        // gnss fix
        switch ((temp_data >> 17) & 0xFF)
        {
        case 0:
            // ntlts->todSlave.GnssFixValue->setCurrentText("NO FIX");
            snprintf(ntlts->todSlave.GnssFixType, sizeof(ntlts->todSlave.GnssFixType), "NO FIX");
            break;
        case 1:
            // ntlts->todSlave.GnssFixValue->setCurrentText("DEAD RECKONING");
            snprintf(ntlts->todSlave.GnssFixType, sizeof(ntlts->todSlave.GnssFixType), "DEAD RECKONING");
            break;
        case 2:
            // ntlts->todSlave.GnssFixValue->setCurrentText("2D FIX");
            snprintf(ntlts->todSlave.GnssFixType, sizeof(ntlts->todSlave.GnssFixType), "2D FIX");
            break;
        case 3:
            // ntlts->todSlave.GnssFixValue->setCurrentText("3D FIX");
            snprintf(ntlts->todSlave.GnssFixType, sizeof(ntlts->todSlave.GnssFixType), "3D FIX");
            break;
        case 4:
            // ntlts->todSlave.GnssFixValue->setCurrentText("GNSS & DEAD RECKONING");
            snprintf(ntlts->todSlave.GnssFixType, sizeof(ntlts->todSlave.GnssFixType), "GNSS & DEAD RECKONING");
            break;
        default:
            // ntlts->todSlave.GnssFixValue->setCurrentText("NA");
            snprintf(ntlts->todSlave.GnssFixType, sizeof(ntlts->todSlave.GnssFixType), "NA");
            break;
        }

        // spoofing state
        // switch ((temp_data >> 25) & 0x3)
        //{
        // case 0:
        //    ntlts->todSlave.SpoofingStateValue->setCurrentText("UNKNOWN");
        //    break;
        // case 1:
        //    ntlts->todSlave.SpoofingStateValue->setCurrentText("NO SPOOFING");
        //    break;
        // case 2:
        //    ntlts->todSlave.SpoofingStateValue->setCurrentText("SPOOFING");
        //    break;
        // default:
        //    ntlts->todSlave.SpoofingStateValue->setCurrentText("NA");
        //    break;
        //}
        //
        //// fix and spoofing valid
        // if ((temp_data & 0x10000000) == 0)
        //{
        //     ntlts->todSlave.FixAndSpoofingInfoValid = 0;
        // }
        // else
        //{
        //     ntlts->todSlave.FixAndSpoofingInfoValid = 1;
        //     ;
        // }
        //
        //// jamming level
        // ntlts->todSlave.JammingLevelValue->setText(QString::number(((temp_data >> 5) & 0xFF)));
        //
        //// jamming state
        // switch ((temp_data >> 3) & 0x3)
        //{
        // case 0:
        //     ntlts->todSlave.JammingStateValue->setCurrentText("UNKNOWN");
        //     break;
        // case 1:
        //     ntlts->todSlave.JammingStateValue->setCurrentText("NO JAMMING");
        //     break;
        // case 2:
        //     ntlts->todSlave.JammingStateValue->setCurrentText("JAMMING LEVEL WARNING");
        //     break;
        // case 3:
        //     ntlts->todSlave.JammingStateValue->setCurrentText("JAMMING LEVEL CRITICAL");
        //     break;
        // default:
        //     ntlts->todSlave.JammingStateValue->setCurrentText("NA");
        //     break;
        // }
        //
        // antenna state
        //    switch ((temp_data >> 0) & 0x7)
        //    {
        //    case 0:
        //        ntlts->todSlave.AntennaStateValue->setCurrentText("INIT");
        //        break;
        //    case 1:
        //        ntlts->todSlave.AntennaStateValue->setCurrentText("DONT KNOW");
        //        break;
        //    case 2:
        //        ntlts->todSlave.AntennaStateValue->setCurrentText("OK");
        //        break;
        //    case 3:
        //        ntlts->todSlave.AntennaStateValue->setCurrentText("SHORT");
        //        break;
        //    case 4:
        //        ntlts->todSlave.AntennaStateValue->setCurrentText("OPEN");
        //        break;
        //    default:
        //        ntlts->todSlave.AntennaStateValue->setCurrentText("NA");
        //        break;
        //    }
        //
        //    // antenna & jamming valid
        //    if ((temp_data & 0x20000000) == 0)
        //    {
        //        ntlts->todSlave.AntennaAndJammingInfoValid = 0;
        //    }
        //    else
        //    {
        //        ntlts->todSlave.AntennaAndJammingInfoValid = 1;
        //
        //    }
    }
    else
    {
        // ntlts->todSlave.GnssFixValue->setCurrentText("NA");
        snprintf(ntlts->todSlave.GnssFixType, sizeof(ntlts->todSlave.GnssFixType), "NA");
        ntlts->todSlave.GnssFix = 0;
        // ntlts->todSlave.SpoofingStateValue->setCurrentText("NA");
        // ntlts->todSlave.FixAndSpoofingInfoValid = 0;
        // ntlts->todSlave.JammingLevelValue->setText("NA");
        // ntlts->todSlave.JammingStateValue->setCurrentText("NA");
        // ntlts->todSlave.AntennaStateValue->setCurrentText("NA");
        // ntlts->todSlave.AntennaAndJammingInfoValid = 0;
    }

    // nr of satelites
    // if (0 == read_reg(temp_addr + Ucm_TodSlave_SatelliteNumber_Reg_Con, &temp_data))
    //{
    //    ntlts->todSlave.NrOfSatellitesSeenValue->setText(QString::number(((temp_data >> 0) & 0xFF)));
    //    ntlts->todSlave.NrOfSatellitesLockedValue->setText(QString::number(((temp_data >> 8) & 0xFF)));
    //
    //    // valid
    //    if ((temp_data & 0x00010000) == 0)
    //    {
    //        ntlts->todSlave.NrOfSatellitesInfo = 0;
    //    }
    //    else
    //    {
    //        ntlts->todSlave.NrOfSatellitesInfo = 1;
    //        ;
    //    }
    //
    //    nr_of_satellites = (temp_data >> 0) & 0xFF;
    //    nr_of_satellites_locked = (temp_data >> 8) & 0xFF;
    //
    //    if (true == tod_slave_timer->isActive())
    //    {
    //
    //        tod_slave_nr_of_satellites_series->append(tod_slave_satellites_number_of_points, nr_of_satellites);
    //        tod_slave_nr_of_satellites_locked_series->append(tod_slave_satellites_number_of_points, nr_of_satellites_locked);
    //
    //        if (tod_slave_satellites_number_of_points < 50)
    //        {
    //            tod_slave_satellites_number_of_points++;
    //        }
    //        else
    //        {
    //            for (int j = 1; j < tod_slave_nr_of_satellites_series->count(); j++)
    //            {
    //                QPointF temp_point;
    //
    //                temp_point = tod_slave_nr_of_satellites_series->at(j);
    //                tod_slave_nr_of_satellites_series->replace(j, (j - 1), temp_point.y());
    //                temp_point = tod_slave_nr_of_satellites_locked_series->at(j);
    //                tod_slave_nr_of_satellites_locked_series->replace(j, (j - 1), temp_point.y());
    //            }
    //            tod_slave_nr_of_satellites_series->remove(0);
    //            tod_slave_nr_of_satellites_locked_series->remove(0);
    //        }
    //
    //        temp_max = 0;
    //        for (int j = 0; j < tod_slave_nr_of_satellites_series->count(); j++)
    //        {
    //            QPointF temp_point = tod_slave_nr_of_satellites_series->at(j);
    //            if (temp_max < temp_point.y())
    //            {
    //                temp_max = temp_point.y();
    //            }
    //        }
    //        if ((temp_max % 10) <= 5)
    //        {
    //            temp_max += (10 - (temp_max % 10));
    //        }
    //        else
    //        {
    //            temp_max += (20 - (temp_max % 10));
    //        }
    //        if (temp_max > 260)
    //        {
    //            temp_max = 260;
    //        }
    //        tod_slave_satellites_chart->axisY()->setMin(0);
    //        tod_slave_satellites_chart->axisY()->setMax(temp_max);
    //
    //        tod_slave_satellites_chart->show();
    //    }
    //}
    // else
    //{
    //    ntlts->todSlave.NrOfSatellitesSeenValue->setText("NA");
    //    ntlts->todSlave.NrOfSatellitesLockedValue->setText("NA");
    //    ntlts->todSlave.NrOfSatellitesInfo = 0;
    //}

    // version
    if (0 == read_reg(temp_addr + Ucm_TodSlave_VersionReg, &temp_data))
    {
        // ntlts->todSlave.VersionValue->setText(QString("0x%1").arg(temp_data, 8, 16, QLatin1Char('0')));
        ntlts->todSlave.Version = temp_data;
    }
    else
    {
        // ntlts->todSlave.VersionValue->setText("NA");
    }

    return 0;
}

int8_t tod_slave_write_values(NTL_TS_T *ntlts, uint8_t fromRegisters)

// void Ucm_TodSlaveTab::tod_slave_write_values(void)
{
    uint32_t temp_data = 0;
    uint32_t temp_addr = 0;
    uint32_t temp_ctrl = 0;
    // uint8_t temp_string[STRING_SIZE] = {0};

    temp_addr = ntlts->todRegs.StartAddr;

    // correction
    // temp_string = ui->TodSlaveCorrectionValue->text();
    // temp_data = temp_string.toUInt(nullptr, 16);

    temp_data = ntlts->todSlave.Correction;

    if (fromRegisters)
    {
        temp_data = ntlts->todRegs.CorrectionReg;
    }

    // if (temp_string == "NA")
    if (false)
    {
        // nothing
    }
    else if (0 == write_reg(temp_addr + Ucm_TodSlave_CorrectionReg, &temp_data))
    {
        /// ui->TodSlaveCorrectionValue->setText(QString("0x%1").arg(temp_data, 8, 16, QLatin1Char('0')));
    }
    else
    {
        // ui->TodSlaveCorrectionValue->setText("NA");
        return -1;
    }

    // baud rate
    // temp_string = ui->TodSlaveBaudRateValue->currentText();

    if (ntlts->todSlave.BaudRate == 1200)
    {
        temp_data = 0x00000000;
    }
    else if (ntlts->todSlave.BaudRate == 2400)
    {
        temp_data = 0x0000001;
    }
    else if (ntlts->todSlave.BaudRate == 4800)
    {
        temp_data = 0x00000002;
    }
    else if (ntlts->todSlave.BaudRate == 9600)
    {
        temp_data = 0x00000003;
    }
    else if (ntlts->todSlave.BaudRate == 19200)
    {
        temp_data = 0x00000004;
    }
    else if (ntlts->todSlave.BaudRate == 38400)
    {
        temp_data = 0x00000005;
    }
    else if (ntlts->todSlave.BaudRate == 57600)
    {
        temp_data = 0x00000006;
    }
    else if (ntlts->todSlave.BaudRate == 115200)
    {
        temp_data = 0x00000007;
    }
    else if (ntlts->todSlave.BaudRate == 230400)
    {
        temp_data = 0x00000008;
    }
    else if (ntlts->todSlave.BaudRate == 460800)
    {
        temp_data = 0x00000009;
    }
    else if (ntlts->todSlave.BaudRate == 921600)
    {
        temp_data = 0x000000A;
    }
    else if (ntlts->todSlave.BaudRate == 1000000)
    {
        temp_data = 0x0000000B;
    }
    else if (ntlts->todSlave.BaudRate == 2000000)
    {
        temp_data = 0x0000000C;
    }
    else
    {
        temp_data = 0x00000000;
    }

    if (fromRegisters)
    {
        temp_data = ntlts->todRegs.UartBaudRateReg;
    }

    if (false)
    {
        // nothing
    }
    else if (0 == write_reg(temp_addr + Ucm_TodSlave_UartBaudRateReg, &temp_data))
    {
        // switch (temp_data)
        //{
        // case 0:
        //     ui->TodSlaveBaudRateValue->setCurrentText("1200");
        //     break;
        // case 1:
        //     ui->TodSlaveBaudRateValue->setCurrentText("2400");
        //     break;
        // case 2:
        //     ui->TodSlaveBaudRateValue->setCurrentText("4800");
        //     break;
        // case 3:
        //     ui->TodSlaveBaudRateValue->setCurrentText("9600");
        //     break;
        // case 4:
        //     ui->TodSlaveBaudRateValue->setCurrentText("19200");
        //     break;
        // case 5:
        //     ui->TodSlaveBaudRateValue->setCurrentText("38400");
        //     break;
        // case 6:
        //     ui->TodSlaveBaudRateValue->setCurrentText("57600");
        //     break;
        // case 7:
        //     ui->TodSlaveBaudRateValue->setCurrentText("115200");
        //     break;
        // case 8:
        //     ui->TodSlaveBaudRateValue->setCurrentText("230400");
        //     break;
        // case 9:
        //     ui->TodSlaveBaudRateValue->setCurrentText("460800");
        //     break;
        // case 10:
        //     ui->TodSlaveBaudRateValue->setCurrentText("921600");
        //     break;
        // case 11:
        //     ui->TodSlaveBaudRateValue->setCurrentText("1000000");
        //     break;
        // case 12:
        //     ui->TodSlaveBaudRateValue->setCurrentText("2000000");
        //     break;
        // default:
        //     ui->TodSlaveBaudRateValue->setCurrentText("NA");
        //     break;
        // }
    }
    else
    {
        // ui->TodSlaveBaudRateValue->setCurrentText("NA");
        return -2;
    }

    // polarity
    temp_data = 0x00000000; // nothing
    // if (false == ui->TodSlaveInvertedCheckBox->isChecked())
    if (ntlts->todSlave.Inverted == 0)
    {
        temp_data |= 0x00000001; // no inversion
    }

    if (fromRegisters)
    {
        temp_data = ntlts->todRegs.PolarityReg;
    }

    if (0 == write_reg(temp_addr + Ucm_TodSlave_PolarityReg, &temp_data))
    {
        // nothing
    }
    else
    {
        // ui->TodSlaveInvertedCheckBox->setChecked(false);
        return -3;
    }

    // control
    if (0 == read_reg(temp_addr + Ucm_TodSlave_ControlReg, &temp_ctrl))
    {
        // nothing
    }

    // nothing
    temp_data = 0x00000000;
    // protocol
    // temp_string = ui->TodSlaveProtocolValue->currentText();

    // if (temp_string == "NMEA")
    if (strncmp(ntlts->todSlave.Protocol, "NMEA", strlen("NMEA")) == 0)
    {
        temp_data |= 0x00000000;
    }
    // else if (temp_string == "UBX")
    else if (strncmp(ntlts->todSlave.Protocol, "UBX", strlen("UBX")) == 0)
    {
        temp_data |= 0x10000000;
    }
    // else if (temp_string == "TSIP")
    else if (strncmp(ntlts->todSlave.Protocol, "TSIP", strlen("TSIP")) == 0)

    {
        temp_data |= 0x20000000;
    }
    // else if (temp_string == "ESIP")
    else if (strncmp(ntlts->todSlave.Protocol, "ESIP", strlen("ESIP")) == 0)

    {
        temp_data |= 0x30000000;
    }
    else
    {
        temp_data |= temp_ctrl & 0x00000000;
    }

    // gnss
    // temp_string = ui->TodSlaveGnssValue->currentText();

    if (strncmp(ntlts->todSlave.Gnss, "ALL", strlen("ALL")) == 0)
    {
        temp_data |= 0x00000000;
    }
    else if (strncmp(ntlts->todSlave.Gnss, "COMBINED", strlen("COMBINED")) == 0)
    {
        temp_data |= 0x01000000;
    }
    else if (strncmp(ntlts->todSlave.Gnss, "GPS", strlen("GPS")) == 0)
    {
        temp_data |= 0x02000000;
    }
    else if (strncmp(ntlts->todSlave.Gnss, "GLONASS", strlen("GLONASS")) == 0)
    {
        temp_data |= 0x03000000;
    }
    else if (strncmp(ntlts->todSlave.Gnss, "GALILEO", strlen("GALILEO")) == 0)
    {
        temp_data |= 0x04000000;
    }
    else if (strncmp(ntlts->todSlave.Gnss, "BEIDOU", strlen("BEIDOU")) == 0)
    {
        temp_data |= 0x05000000;
    }
    else
    {
        temp_data |= temp_ctrl & 0x0F000000;
    }

    // msg disable
    // temp_string = ui->TodSlaveMsgDisableValue->text();
    // if (temp_string == "NA")
    // {
    //     temp_data |= temp_ctrl & 0x00FF0000;
    // }
    // else
    // {
    //     temp_data |= ((temp_string.toUInt(nullptr, 16) & 0xFF) << 16);
    // }

    if (ntlts->todSlave.Enable)
    {
        temp_data |= 0x00000001; // enable
    }

    if (fromRegisters)
    {
        temp_data = ntlts->todRegs.ControlReg;
    }

    if (0 == write_reg(temp_addr + Ucm_TodSlave_ControlReg, &temp_data))
    {
        // nothing
    }
    else
    {
        return -4;
        // ui->TodSlaveProtocolValue->setCurrentText("NA");
        // ui->TodSlaveGnssValue->setCurrentText("NA");
        // ui->TodSlaveMsgDisableValue->setText("NA");
        // ui->TodSlaveEnableCheckBox->setChecked(false);
    }

    return 0;
}



void tod_handler(char *temp_rsp, int rsp_size, const char *prop, char *val)
{
    int write = 0;
    int err = 0;

    err = tod_slave_read_values(&ntlts);

    if (err != 0)
    {
        snprintf(temp_rsp, rsp_size, "TOD_READ_ERR: %d", err);
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
            ntlts.todSlave.Enable = strtoul(val, NULL, 10);
        }

        // return ram value
        snprintf(temp_rsp, rsp_size, "$TOD,ENB,%d", ntlts.todSlave.Enable);
    }

    // GET / SET PROTOCOL
    else if (strncmp(prop, "PRO", 3) == 0)
    {
        if (write)
        {
            memcpy(ntlts.todSlave.Protocol, val, sizeof(ntlts.todSlave.Protocol));
        }

        // return ram value
        snprintf(temp_rsp, rsp_size, "$TOD,PRO,%s", ntlts.todSlave.Protocol);
    }

    // GET / SET GNSS
    else if (strncmp(prop, "GNS", 3) == 0)
    {
        if (write)
        {
            memcpy(ntlts.todSlave.Gnss, val, sizeof(ntlts.todSlave.Gnss));
        }

        // return ram value
        snprintf(temp_rsp, rsp_size, "$TOD,GNS,%s", ntlts.todSlave.Gnss);
    }

    // GET INPUT OK

    else if (strncmp(prop, "IOK", 3) == 0)
    {
        snprintf(temp_rsp, rsp_size, "$TOD,IOK,%d", ntlts.todSlave.InputOk);
    }

    // GET / SET BAUDRATE
    else if (strncmp(prop, "BAU", 3) == 0)
    {
        if (write)
        {
            ntlts.todSlave.BaudRate = strtoul(val, NULL, 10);
        }

        // return ram value
        snprintf(temp_rsp, rsp_size, "$TOD,BAU,%d", ntlts.todSlave.BaudRate);
    }

    // WRITE FPGA REGS WITH UPDATED RAM
    if (write)
    {
        err = tod_slave_write_values(&ntlts, 0);
        if (err != 0)
        {
            snprintf(temp_rsp, rsp_size, "TOD_WRITE_ERR: %d", err);
        }
        write = 0;

        err = tod_slave_read_values(&ntlts);

        if (err != 0)
        {
            snprintf(temp_rsp, rsp_size, "TOD_READ_ERR: %d", err);
        }
    }
}
