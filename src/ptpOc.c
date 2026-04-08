#include "ntl_uart.h"
#include "cores.h"
// #include "uartCircBuff.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"

#ifdef NTL_TIME_SERVER

uint8_t ptp_oc_read_values(NTL_TS_T *ntlts)
{

    uint64_t temp_next_jump;
    uint64_t temp_delay;
    int64_t temp_signed_delay;
    uint64_t temp_offset;
    int64_t temp_signed_offset;
    uint8_t temp_length;
    // uint32_t temp_ip;
    uint8_t temp_min = 0;
    uint8_t temp_max = 0;

    uint32_t temp_data = 0;
    uint32_t temp_addr = 0;
    uint32_t temp_ip;
    uint8_t temp_string[STRING_SIZE] = {0};
    uint8_t temp_ip6[16];

    temp_addr = ntlts->ptpRegs.StartAddr;

    // enabled
    if (0 == read_reg(temp_addr + Ucm_PtpOc_ControlReg, &temp_data))
    {
        if ((temp_data & 0x00000001) == 0)
        {
            ntlts->ptpOc.Enable = 0;
        }
        else
        {
            ntlts->ptpOc.Enable = 1;
        }
    }
    else
    {
        ntlts->ptpOc.Enable = 0;
    }

    // vlan
    // if (0 == read_reg(temp_addr + Ucm_PtpOc_ConfigVlanReg, &temp_data))
    //{
    //    if ((temp_data & 0x00010000) == 0)
    //    {
    //        ntlts->ptpOc.VlanEnable = 0;
    //    }
    //    else
    //    {
    //        ntlts->ptpOc.VlanEnable = 1;
    //    }
    //
    //    temp_data &= 0x0000FFFF;
    //
    //    ntlts->ptpOc.VlanValue->setText(QString("0x%1").arg(temp_data, 4, 16, QLatin1Char('0')));
    //}
    // else
    //{
    //    ntlts->ptpOc.VlanEnable = 0;
    //    ntlts->ptpOc.VlanValue->setText("NA");
    //}

    // profile and layer
    if (0 == read_reg(temp_addr + Ucm_PtpOc_ConfigProfileReg, &temp_data))
    {
        switch (temp_data & 0x00000007)
        {
        case 0:
            // ntlts->ptpOc.ProfileValue->setCurrentText("Default");
            snprintf(ntlts->ptpOc.Profile, sizeof(ntlts->ptpOc.Profile), "Default");
            break;
        case 1:
            // ntlts->ptpOc.ProfileValue->setCurrentText("Power");
            snprintf(ntlts->ptpOc.Profile, sizeof(ntlts->ptpOc.Profile), "Power");
            break;
        case 2:
            // ntlts->ptpOc.ProfileValue->setCurrentText("Utility");
            snprintf(ntlts->ptpOc.Profile, sizeof(ntlts->ptpOc.Profile), "Utility");
            break;
        case 3:
            // ntlts->ptpOc.ProfileValue->setCurrentText("TSN");
            snprintf(ntlts->ptpOc.Profile, sizeof(ntlts->ptpOc.Profile), "TSN");
            break;
        case 4:
            // ntlts->ptpOc.ProfileValue->setCurrentText("ITUG8265.1");
            snprintf(ntlts->ptpOc.Profile, sizeof(ntlts->ptpOc.Profile), "ITUG8265.1");
            break;
        case 5:
            // ntlts->ptpOc.ProfileValue->setCurrentText("ITUG8275.1");
            snprintf(ntlts->ptpOc.Profile, sizeof(ntlts->ptpOc.Profile), "ITUG8275.1");
            break;
        case 6:
            // ntlts->ptpOc.ProfileValue->setCurrentText("ITUG8275.2");
            snprintf(ntlts->ptpOc.Profile, sizeof(ntlts->ptpOc.Profile), "ITUG8275.2");
            break;
        default:
            // ntlts->ptpOc.ProfileValue->setCurrentText("NA");
            snprintf(ntlts->ptpOc.Profile, sizeof(ntlts->ptpOc.Profile), "NA");
            break;
        }

        switch ((temp_data >> 8) & 0x00000001)
        {
        case 0:
            ntlts->ptpOc.DefaultDsTwoStep = 0;
            break;
        case 1:
            ntlts->ptpOc.DefaultDsTwoStep = 1;
            break;
        default:
            ntlts->ptpOc.DefaultDsTwoStep = 0;
            break;
        }

        switch ((temp_data >> 9) & 0x00000001)
        {
        case 0:
            ntlts->ptpOc.DefaultDsSignaling = 0;
            break;
        case 1:
            ntlts->ptpOc.DefaultDsSignaling = 1;
            break;
        default:
            ntlts->ptpOc.DefaultDsSignaling = 0;
            break;
        }

        switch ((temp_data >> 16) & 0x00000003)
        {
        case 0:
            // ntlts->ptpOc.LayerValue->setCurrentText("Layer 2");
            snprintf(ntlts->ptpOc.Layer, sizeof(ntlts->ptpOc.Layer), "Layer 2");

            break;
        case 1:
            // ntlts->ptpOc.LayerValue->setCurrentText("Layer 3v4");
            snprintf(ntlts->ptpOc.Layer, sizeof(ntlts->ptpOc.Layer), "Layer 3v4");

            break;
        case 2:
            // ntlts->ptpOc.LayerValue->setCurrentText("Layer 3v6");
            snprintf(ntlts->ptpOc.Layer, sizeof(ntlts->ptpOc.Layer), "Layer 3v6");

            break;
        default:
            // ntlts->ptpOc.LayerValue->setCurrentText("NA");
            snprintf(ntlts->ptpOc.Layer, sizeof(ntlts->ptpOc.Layer), "NA");

            break;
        }

        switch ((temp_data >> 20) & 0x00000003)
        {
        case 0:
            ntlts->ptpOc.DefaultDsSlaveOnly = 0;
            ntlts->ptpOc.DefaultDsMasterOnly = 0;
            break;
        case 1:
            ntlts->ptpOc.DefaultDsSlaveOnly = 1;
            ntlts->ptpOc.DefaultDsMasterOnly = 0;
            break;
        case 2:
            ntlts->ptpOc.DefaultDsSlaveOnly = 0;
            ntlts->ptpOc.DefaultDsMasterOnly = 1;
            break;
        default:
            ntlts->ptpOc.DefaultDsSlaveOnly = 0;
            ntlts->ptpOc.DefaultDsMasterOnly = 0;
            break;
        }

        // switch ((temp_data >> 22) & 0x00000001)
        //{
        // case 0:
        //     ntlts->ptpOc.DefaultDsDisableOffsetCor = 0;
        //     break;
        // case 1:
        //     ntlts->ptpOc.DefaultDsDisableOffsetCor = 1;
        //     break;
        // default:
        //     ntlts->ptpOc.DefaultDsDisableOffsetCor = 0;
        //     break;
        // }

        // switch ((temp_data >> 23) & 0x00000001)
        //{
        // case 0:
        //     ntlts->ptpOc.DefaultDsListedUnicastSlavesOnly = 0;
        //     break;
        // case 1:
        //     ntlts->ptpOc.DefaultDsListedUnicastSlavesOnly = 1;
        //     break;
        // default:
        //     ntlts->ptpOc.DefaultDsListedUnicastSlavesOnly = 0;
        //     break;
        // }

        switch ((temp_data >> 24) & 0x00000001)
        {
        case 0:
            // ntlts->ptpOc.DelayMechanismValue->setCurrentText("P2P");
            snprintf(ntlts->ptpOc.DelayMechanism, sizeof(ntlts->ptpOc.DelayMechanism), "P2P");
            break;
        case 1:
            if ((temp_data & 0x02000000) == 0)
            {
                // ntlts->ptpOc.DelayMechanismValue->setCurrentText("E2E");
                snprintf(ntlts->ptpOc.DelayMechanism, sizeof(ntlts->ptpOc.DelayMechanism), "E2E");
            }
            else
            {
                // ntlts->ptpOc.DelayMechanismValue->setCurrentText("E2E Unicast");
                snprintf(ntlts->ptpOc.DelayMechanism, sizeof(ntlts->ptpOc.DelayMechanism), "E2E Unicast");
            }
            break;
        default:
            // ntlts->ptpOc.DelayMechanismValue->setCurrentText("NA");
            snprintf(ntlts->ptpOc.DelayMechanism, sizeof(ntlts->ptpOc.DelayMechanism), "NA");

            break;
        }
    }
    else
    {
        // ntlts->ptpOc.ProfileValue->setCurrentText("NA");
        snprintf(ntlts->ptpOc.Profile, sizeof(ntlts->ptpOc.Profile), "NA");

        ntlts->ptpOc.DefaultDsTwoStep = 0;
        ntlts->ptpOc.DefaultDsSignaling = 0;
        // ntlts->ptpOc.LayerValue->setCurrentText("NA");
        snprintf(ntlts->ptpOc.Layer, sizeof(ntlts->ptpOc.Layer), "NA");

        // ntlts->ptpOc.DelayMechanismValue->setCurrentText("NA");
        snprintf(ntlts->ptpOc.DelayMechanism, sizeof(ntlts->ptpOc.DelayMechanism), "NA");

        ntlts->ptpOc.DefaultDsSlaveOnly = 0;
        ntlts->ptpOc.DefaultDsMasterOnly = 0;
        // ntlts->ptpOc.DefaultDsListedUnicastSlavesOnly = 0;
        // ntlts->ptpOc.DefaultDsDisableOffsetCor = 0;
    }

    // lucky packet filter
    // if (0 == read_reg(temp_addr + Ucm_PtpOc_ConfigLuckyPacketFilterReg, &temp_data))
    //{
    //    if ((temp_data & 0x00000001) == 0)
    //    {
    //        ntlts->ptpOc.DefaultDsLpFilter = 0;
    //    }
    //    else
    //    {
    //        ntlts->ptpOc.DefaultDsLpFilter = 1;
    //    }
    //
    //    ntlts->ptpOc.DefaultDsLpFilterWindowValue->setText(QString::number(((temp_data >> 16) & 0x00001FFF)));
    //}
    // else
    //{
    //    ntlts->ptpOc.DefaultDsLpFilterWindowValue->setText("NA");
    //    ntlts->ptpOc.DefaultDsLpFilter = 0;
    //}

    // ip
    // temp_string = ntlts->ptpOc.LayerValue->currentText();
    // if (temp_string == "Layer 3v4")
    if (strncmp(ntlts->ptpOc.Layer, "Layer 3v4", strlen("Layer 3v4")) == 0)

    {
        // temp_string.clear();
        memset(temp_string, 0, sizeof(temp_string));

        if (0 == read_reg(temp_addr + Ucm_PtpOc_ConfigIpReg, &temp_data))
        {
            temp_ip = 0x00000000;
            temp_ip |= (temp_data >> 0) & 0x000000FF;
            temp_ip = temp_ip << 8;
            temp_ip |= (temp_data >> 8) & 0x000000FF;
            temp_ip = temp_ip << 8;
            temp_ip |= (temp_data >> 16) & 0x000000FF;
            temp_ip = temp_ip << 8;
            temp_ip |= (temp_data >> 24) & 0x000000FF;

            unsigned char ip_bytes[4];
            ip_bytes[0] = temp_ip & 0xFF;
            ip_bytes[1] = (temp_ip >> 8) & 0xFF;
            ip_bytes[2] = (temp_ip >> 16) & 0xFF;
            ip_bytes[3] = (temp_ip >> 24) & 0xFF;

            snprintf(ntlts->ptpOc.ipAddr, sizeof(ntlts->ptpOc.ipAddr), "%d.%d.%d.%d", ip_bytes[3], ip_bytes[2], ip_bytes[1], ip_bytes[0]);

            // temp_string = QHostAddress(temp_ip).toString();

            // ntlts->ptpOc.IpValue->setText(temp_string);
        }
        else
        {
            // ntlts->ptpOc.IpValue->setText("NA");
            snprintf(ntlts->ptpOc.ipAddr, sizeof(ntlts->ptpOc.ipAddr), "%s", "NA");
        }
    }
    // else if (temp_string == "Layer 3v6")
    else if (strncmp(ntlts->ptpOc.Layer, "Layer 3v6", strlen("Layer 3v6")) == 0)

    {
        // temp_string.clear();
        if (0 == read_reg(temp_addr + Ucm_PtpOc_ConfigIpReg, &temp_data))
        {
            temp_ip6[0] = (temp_data >> 0) & 0x000000FF;
            temp_ip6[1] = (temp_data >> 8) & 0x000000FF;
            temp_ip6[2] = (temp_data >> 16) & 0x000000FF;
            temp_ip6[3] = (temp_data >> 24) & 0x000000FF;

            if (0 == read_reg(temp_addr + Ucm_PtpOc_ConfigIpv61Reg, &temp_data))
            {
                temp_ip6[4] = (temp_data >> 0) & 0x000000FF;
                temp_ip6[5] = (temp_data >> 8) & 0x000000FF;
                temp_ip6[6] = (temp_data >> 16) & 0x000000FF;
                temp_ip6[7] = (temp_data >> 24) & 0x000000FF;

                if (0 == read_reg(temp_addr + Ucm_PtpOc_ConfigIpv62Reg, &temp_data))
                {
                    temp_ip6[8] = (temp_data >> 0) & 0x000000FF;
                    temp_ip6[9] = (temp_data >> 8) & 0x000000FF;
                    temp_ip6[10] = (temp_data >> 16) & 0x000000FF;
                    temp_ip6[11] = (temp_data >> 24) & 0x000000FF;

                    if (0 == read_reg(temp_addr + Ucm_PtpOc_ConfigIpv63Reg, &temp_data))
                    {
                        temp_ip6[12] = (temp_data >> 0) & 0x000000FF;
                        temp_ip6[13] = (temp_data >> 8) & 0x000000FF;
                        temp_ip6[14] = (temp_data >> 16) & 0x000000FF;
                        temp_ip6[15] = (temp_data >> 24) & 0x000000FF;

                        // temp_string = QHostAddress(temp_ip6).toString();

                        snprintf(ntlts->ptpOc.ipAddr, sizeof(ntlts->ptpOc.ipAddr), "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
                                 temp_ip6[0],
                                 temp_ip6[1],
                                 temp_ip6[2],
                                 temp_ip6[3],
                                 temp_ip6[4],
                                 temp_ip6[5],
                                 temp_ip6[6],
                                 temp_ip6[7],
                                 temp_ip6[8],
                                 temp_ip6[9],
                                 temp_ip6[10],
                                 temp_ip6[11],
                                 temp_ip6[12],
                                 temp_ip6[13],
                                 temp_ip6[14],
                                 temp_ip6[15]);

                        // ntlts->ptpOc.IpValue->setText(temp_string);
                    }
                    else
                    {
                        // ntlts->ptpOc.IpValue->setText("NA");
                        snprintf(ntlts->ptpOc.ipAddr, sizeof(ntlts->ptpOc.ipAddr), "%s", "NA");
                    }
                }
                else
                {
                    //                    ntlts->ptpOc.IpValue->setText("NA");
                    snprintf(ntlts->ptpOc.ipAddr, sizeof(ntlts->ptpOc.ipAddr), "%s", "NA");
                }
            }
            else
            {
                //                ntlts->ptpOc.IpValue->setText("NA");
                snprintf(ntlts->ptpOc.ipAddr, sizeof(ntlts->ptpOc.ipAddr), "%s", "NA");
            }
        }
        else
        {
            //            ntlts->ptpOc.IpValue->setText("NA");
            snprintf(ntlts->ptpOc.ipAddr, sizeof(ntlts->ptpOc.ipAddr), "%s", "NA");
        }
    }
    else
    {
        //        ntlts->ptpOc.IpValue->setText("NA");
        snprintf(ntlts->ptpOc.ipAddr, sizeof(ntlts->ptpOc.ipAddr), "%s", "NA");
    }

    //********************************
    // default dataset
    //********************************
    temp_data = 0x40000000;
    if (0 == write_reg(temp_addr + Ucm_PtpOc_DefaultDsControlReg, &temp_data))
    {
        for (int i = 0; i < 10; i++)
        {
            if (0 == read_reg(temp_addr + Ucm_PtpOc_DefaultDsControlReg, &temp_data))
            {
                if ((temp_data & 0x80000000) != 0)
                {
                    // clock id
                    // temp_string.clear();
                    uint8_t temp_clock_id[64];
                    if (0 == read_reg(temp_addr + Ucm_PtpOc_DefaultDs1Reg, &temp_data))
                    {
                        // temp_string.append(QString("%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        // temp_string.append(":");
                        // temp_string.append(QString("%1").arg(((temp_data >> 8) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        // temp_string.append(":");
                        // temp_string.append(QString("%1").arg(((temp_data >> 16) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        // temp_string.append(":");
                        // temp_string.append(QString("%1").arg(((temp_data >> 24) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        // temp_string.append(":");

                        temp_clock_id[0] = ((temp_data >> 0) & 0x000000FF);
                        temp_clock_id[1] = ((temp_data >> 8) & 0x000000FF);
                        temp_clock_id[2] = ((temp_data >> 16) & 0x000000FF);
                        temp_clock_id[3] = ((temp_data >> 24) & 0x000000FF);

                        if (0 == read_reg(temp_addr + Ucm_PtpOc_DefaultDs2Reg, &temp_data))
                        {
                            // temp_string.append(QString("%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));
                            // temp_string.append(":");
                            // temp_string.append(QString("%1").arg(((temp_data >> 8) & 0x000000FF), 2, 16, QLatin1Char('0')));
                            // temp_string.append(":");
                            // temp_string.append(QString("%1").arg(((temp_data >> 16) & 0x000000FF), 2, 16, QLatin1Char('0')));
                            // temp_string.append(":");
                            // temp_string.append(QString("%1").arg(((temp_data >> 24) & 0x000000FF), 2, 16, QLatin1Char('0')));

                            temp_clock_id[4] = ((temp_data >> 0) & 0x000000FF);
                            temp_clock_id[5] = ((temp_data >> 8) & 0x000000FF);
                            temp_clock_id[6] = ((temp_data >> 16) & 0x000000FF);
                            temp_clock_id[7] = ((temp_data >> 24) & 0x000000FF);

                            // ntlts->ptpOc.DefaultDsClockIdValue->setText(temp_string);

                            snprintf(ntlts->ptpOc.DefaultDsClockId, sizeof(ntlts->ptpOc.DefaultDsClockId),
                                     "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",
                                     temp_clock_id[0],
                                     temp_clock_id[1],
                                     temp_clock_id[2],
                                     temp_clock_id[3],
                                     temp_clock_id[4],
                                     temp_clock_id[5],
                                     temp_clock_id[6],
                                     temp_clock_id[7]);
                        }
                        else
                        {
                            // ntlts->ptpOc.DefaultDsClockIdValue->setText("NA");

                            snprintf(ntlts->ptpOc.DefaultDsClockId, sizeof(ntlts->ptpOc.DefaultDsClockId), "NA");
                        }
                    }
                    else
                    {
                        // ntlts->ptpOc.DefaultDsClockIdValue->setText("NA");
                        snprintf(ntlts->ptpOc.DefaultDsClockId, sizeof(ntlts->ptpOc.DefaultDsClockId), "NA");
                    }

                    // domain, priority 1 & 2
                    if (0 == read_reg(temp_addr + Ucm_PtpOc_DefaultDs3Reg, &temp_data))
                    {
                        // ntlts->ptpOc.DefaultDsDomainValue->setText(QString("0x%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        // ntlts->ptpOc.DefaultDsPriority2Value->setText(QString("0x%1").arg(((temp_data >> 16) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        // ntlts->ptpOc.DefaultDsPriority1Value->setText(QString("0x%1").arg(((temp_data >> 24) & 0x000000FF), 2, 16, QLatin1Char('0')));

                        ntlts->ptpOc.DefaultDsDomain = ((temp_data >> 0) & 0x000000FF);
                        ntlts->ptpOc.DefaultDsPriority2 = ((temp_data >> 16) & 0x000000FF);
                        ntlts->ptpOc.DefaultDsPriority1 = ((temp_data >> 24) & 0x000000FF);
                    }
                    else
                    {
                        // ntlts->ptpOc.DefaultDsDomainValue->setText("NA");
                        // ntlts->ptpOc.DefaultDsPriority1Value->setText("NA");
                        // ntlts->ptpOc.DefaultDsPriority2Value->setText("NA");

                        ntlts->ptpOc.DefaultDsDomain = 0;
                        ntlts->ptpOc.DefaultDsPriority2 = 0;
                        ntlts->ptpOc.DefaultDsPriority1 = 0;
                    }

                    // variance, accuracy ,class
                    if (0 == read_reg(temp_addr + Ucm_PtpOc_DefaultDs4Reg, &temp_data))
                    {
                        // ntlts->ptpOc.DefaultDsVarianceValue->setText(QString("0x%1").arg(((temp_data >> 0) & 0x0000FFFF), 4, 16, QLatin1Char('0')));
                        // ntlts->ptpOc.DefaultDsAccuracyValue->setText(QString::number( ((temp_data >> 16) & 0x000000FF) ));
                        // ntlts->ptpOc.DefaultDsClassValue->setText(QString("0x%1").arg( ((temp_data >> 24) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        ntlts->ptpOc.DefaultDsVariance = ((temp_data >> 0) & 0x0000FFFF);
                        ntlts->ptpOc.DefaultDsAccuracy = ((temp_data >> 16) & 0x000000FF);
                        ntlts->ptpOc.DefaultDsClass = ((temp_data >> 24) & 0x000000FF);
                    }
                    else
                    {
                        // ntlts->ptpOc.DefaultDsVarianceValue->setText("NA");
                        // ntlts->ptpOc.DefaultDsAccuracyValue->setText("NA");
                        // ntlts->ptpOc.DefaultDsClassValue->setText("NA");

                        ntlts->ptpOc.DefaultDsVariance = 0;
                        ntlts->ptpOc.DefaultDsAccuracy = 0;
                        ntlts->ptpOc.DefaultDsClass = 0;
                    }

                    // short id
                    if (0 == read_reg(temp_addr + Ucm_PtpOc_DefaultDs5Reg, &temp_data))
                    {
                        ntlts->ptpOc.DefaultDsShortId = temp_data;
                        // ntlts->ptpOc.DefaultDsShortIdValue->setText(QString("0x%1").arg(temp_data, 4, 16, QLatin1Char('0')));
                    }
                    else
                    {
                        ntlts->ptpOc.DefaultDsShortId = -1;
                        // ntlts->ptpOc.DefaultDsShortIdValue->setText("NA");
                    }

                    // inaccuracy
                    if (0 == read_reg(temp_addr + Ucm_PtpOc_DefaultDs6Reg, &temp_data))
                    {
                        ntlts->ptpOc.DefaultDsInaccuracy = temp_data;
                        // ntlts->ptpOc.DefaultDsInaccuracyValue->setText(QString::number(temp_data));
                    }
                    else
                    {
                        ntlts->ptpOc.DefaultDsInaccuracy = -1;
                        // ntlts->ptpOc.DefaultDsInaccuracyValue->setText("NA");
                    }

                    // nr of ports
                    if (0 == read_reg(temp_addr + Ucm_PtpOc_DefaultDs7Reg, &temp_data))
                    {
                        ntlts->ptpOc.DefaultDsNrOfPorts = temp_data;
                        // ntlts->ptpOc.DefaultDsNrOfPortsValue->setText(QString::number(temp_data));
                    }
                    else
                    {
                        ntlts->ptpOc.DefaultDsNrOfPorts = -1;
                        // ntlts->ptpOc.DefaultDsNrOfPortsValue->setText("NA");
                    }

                    break;
                }
                else if (i == 9)
                {
                    // cout << "ERROR: " << "read did not complete" << endl;
                    // ntlts->ptpOc.DefaultDsClockIdValue->setText("NA");
                    // ntlts->ptpOc.DefaultDsDomainValue->setText("NA");
                    // ntlts->ptpOc.DefaultDsPriority1Value->setText("NA");
                    // ntlts->ptpOc.DefaultDsPriority2Value->setText("NA");
                    // ntlts->ptpOc.DefaultDsVarianceValue->setText("NA");
                    // ntlts->ptpOc.DefaultDsAccuracyValue->setText("NA");
                    // ntlts->ptpOc.DefaultDsClassValue->setText("NA");
                    // ntlts->ptpOc.DefaultDsShortIdValue->setText("NA");
                    // ntlts->ptpOc.DefaultDsInaccuracyValue->setText("NA");
                    // ntlts->ptpOc.DefaultDsNrOfPortsValue->setText("NA");
                }
            }
            else
            {
                // ntlts->ptpOc.DefaultDsClockIdValue->setText("NA");
                // ntlts->ptpOc.DefaultDsDomainValue->setText("NA");
                // ntlts->ptpOc.DefaultDsPriority1Value->setText("NA");
                // ntlts->ptpOc.DefaultDsPriority2Value->setText("NA");
                // ntlts->ptpOc.DefaultDsVarianceValue->setText("NA");
                // ntlts->ptpOc.DefaultDsAccuracyValue->setText("NA");
                // ntlts->ptpOc.DefaultDsClassValue->setText("NA");
                // ntlts->ptpOc.DefaultDsShortIdValue->setText("NA");
                // ntlts->ptpOc.DefaultDsInaccuracyValue->setText("NA");
                // ntlts->ptpOc.DefaultDsNrOfPortsValue->setText("NA");
            }
        }
    }
    else
    {
        // ntlts->ptpOc.DefaultDsClockIdValue->setText("NA");
        // ntlts->ptpOc.DefaultDsDomainValue->setText("NA");
        // ntlts->ptpOc.DefaultDsPriority1Value->setText("NA");
        // ntlts->ptpOc.DefaultDsPriority2Value->setText("NA");
        // ntlts->ptpOc.DefaultDsVarianceValue->setText("NA");
        // ntlts->ptpOc.DefaultDsAccuracyValue->setText("NA");
        // ntlts->ptpOc.DefaultDsClassValue->setText("NA");
        // ntlts->ptpOc.DefaultDsShortIdValue->setText("NA");
        // ntlts->ptpOc.DefaultDsInaccuracyValue->setText("NA");
        // ntlts->ptpOc.DefaultDsNrOfPortsValue->setText("NA");
    }

    //********************************
    // port dataset
    //********************************

    /*
    temp_data = 0x40000000;
    if (0 == write_reg(temp_addr + Ucm_PtpOc_PortDsControlReg, &temp_data))
    {
        for (int i = 0; i < 10; i++)
        {
            if (0 == read_reg(temp_addr + Ucm_PtpOc_PortDsControlReg, &temp_data))
            {
                if ((temp_data & 0x80000000) != 0)
                {
                    temp_string = ntlts->ptpOc.DelayMechanismValue->currentText();
                    if ((temp_string == "E2E") or(temp_string == "E2E Unicast"))
                    {
                        // end to end delay
                        ntlts->ptpOc.PortDsPeerDelayValue->setText("NA");
                    }
                    else if (0 == read_reg(temp_addr + Ucm_PtpOc_PortDs1Reg, &temp_data))
                    {
                        // peer delay
                        temp_delay = temp_data;
                        temp_delay = temp_delay << 32;
                        if (0 == read_reg(temp_addr + Ucm_PtpOc_PortDs2Reg, &temp_data))
                        {
                            temp_delay |= temp_data;
                            temp_signed_delay = (long long)temp_delay;
                            temp_signed_delay = temp_signed_delay >> 16;
                            ntlts->ptpOc.PortDsPeerDelayValue->setText(QString::number(temp_signed_delay));

                            if (true == ptp_oc_timer->isActive())
                            {
                                ptp_oc_delay_series->append(ptp_oc_delay_number_of_points, temp_signed_delay);

                                if (ptp_oc_delay_number_of_points < 20)
                                {
                                    ptp_oc_delay_number_of_points++;
                                }
                                else
                                {
                                    for (int j = 1; j < ptp_oc_delay_series->count(); j++)
                                    {
                                        QPointF temp_point = ptp_oc_delay_series->at(j);
                                        ptp_oc_delay_series->replace(j, (j - 1), temp_point.y());
                                    }
                                    ptp_oc_delay_series->remove(0);
                                }

                                temp_min = 0;
                                temp_max = 0;
                                for (int j = 0; j < ptp_oc_delay_series->count(); j++)
                                {
                                    QPointF temp_point = ptp_oc_delay_series->at(j);
                                    if (j == 0)
                                    {
                                        temp_min = temp_point.y();
                                        temp_max = temp_point.y();
                                    }
                                    if (temp_min > temp_point.y())
                                    {
                                        temp_min = temp_point.y();
                                    }
                                    if (temp_max < temp_point.y())
                                    {
                                        temp_max = temp_point.y();
                                    }
                                }
                                temp_max = ((temp_max / 100) + 1) * 100;
                                temp_min = ((temp_min / 100) - 1) * 100;
                                // if (temp_min < 0)
                                //{
                                //     temp_min = 0;
                                // }
                                ptp_oc_delay_chart->axisY()->setMin(temp_min);
                                ptp_oc_delay_chart->axisY()->setMax(temp_max);

                                ptp_oc_delay_chart->show();
                            }
                        }
                        else
                        {
                            ntlts->ptpOc.PortDsPeerDelayValue->setText("NA");
                        }
                    }
                    else
                    {
                        ntlts->ptpOc.PortDsPeerDelayValue->setText("NA");
                    }

                    // state
                    if (0 == read_reg(temp_addr + Ucm_PtpOc_PortDs3Reg, &temp_data))
                    {
                        switch (temp_data)
                        {
                        case 0x0000001:
                            ntlts->ptpOc.PortDsStateValue->setText("INITIALIZING");
                            break;
                        case 0x00000002:
                            ntlts->ptpOc.PortDsStateValue->setText("FAULTY");
                            break;
                        case 0x00000003:
                            ntlts->ptpOc.PortDsStateValue->setText("DISABLED");
                            break;
                        case 0x00000004:
                            ntlts->ptpOc.PortDsStateValue->setText("LISTENING");
                            break;
                        case 0x00000005:
                            ntlts->ptpOc.PortDsStateValue->setText("PREMASTER");
                            break;
                        case 0x00000006:
                            ntlts->ptpOc.PortDsStateValue->setText("MASTER");
                            break;
                        case 0x00000007:
                            ntlts->ptpOc.PortDsStateValue->setText("PASSIVE");
                            break;
                        case 0x00000008:
                            ntlts->ptpOc.PortDsStateValue->setText("UNCALIBRATED");
                            break;
                        case 0x00000009:
                            ntlts->ptpOc.PortDsStateValue->setText("SLAVE");
                            break;
                        default:
                            ntlts->ptpOc.PortDsStateValue->setText("NA");
                            break;
                        }
                    }
                    else
                    {
                        ntlts->ptpOc.PortDsStateValue->setText("NA");
                    }

                    // pdelay and delay req log msg interval and max delay
                    if (0 == read_reg(temp_addr + Ucm_PtpOc_PortDs4Reg, &temp_data))
                    {
                        ntlts->ptpOc.PortDsPDelayReqLogMsgIntervalValue->setText(QString::number((signed char)(temp_data & 0x000000FF)));
                        ntlts->ptpOc.PortDsDelayReqLogMsgIntervalValue->setText(QString::number((signed char)((temp_data >> 8) & 0x000000FF)));
                        ntlts->ptpOc.PortDsDelayReceiptTimeoutValue->setText(QString::number(((temp_data >> 16) & 0x000000FF)));
                    }
                    else
                    {
                        ntlts->ptpOc.PortDsPDelayReqLogMsgIntervalValue->setText("NA");
                        ntlts->ptpOc.PortDsDelayReqLogMsgIntervalValue->setText("NA");
                        ntlts->ptpOc.PortDsDelayReceiptTimeoutValue->setText("NA");
                    }

                    // announce log msg interval and announce receipt timeout
                    if (0 == read_reg(temp_addr + Ucm_PtpOc_PortDs5Reg, &temp_data))
                    {
                        ntlts->ptpOc.PortDsAnnounceLogMsgIntervalValue->setText(QString::number((signed char)(temp_data & 0x000000FF)));
                        ntlts->ptpOc.PortDsAnnounceReceiptTimeoutValue->setText(QString::number(((temp_data >> 8) & 0x000000FF)));
                    }
                    else
                    {
                        ntlts->ptpOc.PortDsAnnounceLogMsgIntervalValue->setText("NA");
                        ntlts->ptpOc.PortDsAnnounceReceiptTimeoutValue->setText("NA");
                    }

                    // sync log msg interval and sync receipt timeout
                    if (0 == read_reg(temp_addr + Ucm_PtpOc_PortDs6Reg, &temp_data))
                    {
                        ntlts->ptpOc.PortDsSyncLogMsgIntervalValue->setText(QString::number((signed char)(temp_data & 0x000000FF)));
                        ntlts->ptpOc.PortDsSyncReceiptTimeoutValue->setText(QString::number(((temp_data >> 8) & 0x000000FF)));
                    }
                    else
                    {
                        ntlts->ptpOc.PortDsSyncLogMsgIntervalValue->setText("NA");
                        ntlts->ptpOc.PortDsSyncReceiptTimeoutValue->setText("NA");
                    }

                    // asymmetry
                    if (0 == read_reg(temp_addr + Ucm_PtpOc_PortDs7Reg, &temp_data))
                    {
                        ntlts->ptpOc.PortDsAsymmetryValue->setText(QString::number((signed int)temp_data));
                    }
                    else
                    {
                        ntlts->ptpOc.PortDsAsymmetryValue->setText("NA");
                    }

                    // max pdelay
                    if (0 == read_reg(temp_addr + Ucm_PtpOc_PortDs8Reg, &temp_data))
                    {
                        ntlts->ptpOc.PortDsMaxPeerDelayValue->setText(QString::number(temp_data));
                    }
                    else
                    {
                        ntlts->ptpOc.PortDsMaxPeerDelayValue->setText("NA");
                    }

                    break;
                }
                else if (i == 9)
                {
                    cout << "ERROR: " << "read did not complete" << endl;
                    ntlts->ptpOc.PortDsPeerDelayValue->setText("NA");
                    ntlts->ptpOc.PortDsStateValue->setText("NA");
                    ntlts->ptpOc.PortDsAsymmetryValue->setText("NA");
                    ntlts->ptpOc.PortDsMaxPeerDelayValue->setText("NA");
                    ntlts->ptpOc.PortDsPDelayReqLogMsgIntervalValue->setText("NA");
                    ntlts->ptpOc.PortDsDelayReqLogMsgIntervalValue->setText("NA");
                    ntlts->ptpOc.PortDsDelayReceiptTimeoutValue->setText("NA");
                    ntlts->ptpOc.PortDsAnnounceLogMsgIntervalValue->setText("NA");
                    ntlts->ptpOc.PortDsAnnounceReceiptTimeoutValue->setText("NA");
                    ntlts->ptpOc.PortDsSyncLogMsgIntervalValue->setText("NA");
                    ntlts->ptpOc.PortDsSyncReceiptTimeoutValue->setText("NA");
                }
            }
            else
            {
                ntlts->ptpOc.PortDsPeerDelayValue->setText("NA");
                ntlts->ptpOc.PortDsStateValue->setText("NA");
                ntlts->ptpOc.PortDsAsymmetryValue->setText("NA");
                ntlts->ptpOc.PortDsMaxPeerDelayValue->setText("NA");
                ntlts->ptpOc.PortDsPDelayReqLogMsgIntervalValue->setText("NA");
                ntlts->ptpOc.PortDsDelayReqLogMsgIntervalValue->setText("NA");
                ntlts->ptpOc.PortDsDelayReceiptTimeoutValue->setText("NA");
                ntlts->ptpOc.PortDsAnnounceLogMsgIntervalValue->setText("NA");
                ntlts->ptpOc.PortDsAnnounceReceiptTimeoutValue->setText("NA");
                ntlts->ptpOc.PortDsSyncLogMsgIntervalValue->setText("NA");
                ntlts->ptpOc.PortDsSyncReceiptTimeoutValue->setText("NA");
            }
        }
    }
    else
    {
        ntlts->ptpOc.PortDsPeerDelayValue->setText("NA");
        ntlts->ptpOc.PortDsStateValue->setText("NA");
        ntlts->ptpOc.PortDsAsymmetryValue->setText("NA");
        ntlts->ptpOc.PortDsMaxPeerDelayValue->setText("NA");
        ntlts->ptpOc.PortDsPDelayReqLogMsgIntervalValue->setText("NA");
        ntlts->ptpOc.PortDsDelayReqLogMsgIntervalValue->setText("NA");
        ntlts->ptpOc.PortDsDelayReceiptTimeoutValue->setText("NA");
        ntlts->ptpOc.PortDsAnnounceLogMsgIntervalValue->setText("NA");
        ntlts->ptpOc.PortDsAnnounceReceiptTimeoutValue->setText("NA");
        ntlts->ptpOc.PortDsSyncLogMsgIntervalValue->setText("NA");
        ntlts->ptpOc.PortDsSyncReceiptTimeoutValue->setText("NA");
    }

    //********************************
    // current dataset
    //********************************
    temp_data = 0x40000000;
    if (0 == write_reg(temp_addr + Ucm_PtpOc_CurrentDsControlReg, &temp_data))
    {
        for (int i = 0; i < 10; i++)
        {
            if (0 == read_reg(temp_addr + Ucm_PtpOc_CurrentDsControlReg, &temp_data))
            {
                if ((temp_data & 0x80000000) != 0)
                {

                    // steps removed
                    if (0 == read_reg(temp_addr + Ucm_PtpOc_CurrentDs1Reg, &temp_data))
                    {
                        ntlts->ptpOc.CurrentDsStepsRemovedValue->setText(QString::number(temp_data & 0xFFFF));
                    }
                    else
                    {
                        ntlts->ptpOc.CurrentDsStepsRemovedValue->setText("NA");
                    }

                    // offset
                    if (0 == read_reg(temp_addr + Ucm_PtpOc_CurrentDs2Reg, &temp_data))
                    {
                        temp_offset = temp_data;
                        temp_offset = temp_offset << 32;
                        if (0 == read_reg(temp_addr + Ucm_PtpOc_CurrentDs3Reg, &temp_data))
                        {
                            temp_offset |= temp_data;

                            if ((temp_offset & 0x8000000000000000) != 0)
                            {
                                temp_offset = (0xFFFF000000000000 | (temp_offset >> 16));
                                temp_signed_offset = (long long)temp_offset;
                            }
                            else
                            {
                                temp_offset = (0x0000FFFFFFFFFFFF & (temp_offset >> 16));
                                temp_signed_offset = (long long)temp_offset;
                            }

                            if (temp_signed_offset == -4294967296) // negativ 0
                            {
                                temp_signed_offset = 0;
                            }

                            // limit to one second in display
                            if (temp_signed_offset >= 100000)
                            {
                                temp_signed_offset = 100000;
                            }
                            else if (temp_signed_offset <= -100000)
                            {
                                temp_signed_offset = -100000;
                            }

                            ntlts->ptpOc.CurrentDsOffsetValue->setText(QString::number(temp_signed_offset));

                            if (true == ptp_oc_timer->isActive())
                            {

                                ptp_oc_offset_series->append(ptp_oc_offset_number_of_points, temp_signed_offset);

                                if (ptp_oc_offset_number_of_points < 20)
                                {
                                    ptp_oc_offset_number_of_points++;
                                }
                                else
                                {
                                    for (int j = 1; j < ptp_oc_offset_series->count(); j++)
                                    {
                                        QPointF temp_point = ptp_oc_offset_series->at(j);
                                        ptp_oc_offset_series->replace(j, (j - 1), temp_point.y());
                                    }
                                    ptp_oc_offset_series->remove(0);
                                }

                                temp_min = 0;
                                temp_max = 0;
                                for (int j = 0; j < ptp_oc_offset_series->count(); j++)
                                {
                                    QPointF temp_point = ptp_oc_offset_series->at(j);
                                    if (j == 0)
                                    {
                                        temp_min = temp_point.y();
                                        temp_max = temp_point.y();
                                    }
                                    if (temp_min > temp_point.y())
                                    {
                                        temp_min = temp_point.y();
                                    }
                                    if (temp_max < temp_point.y())
                                    {
                                        temp_max = temp_point.y();
                                    }
                                }
                                temp_max = ((temp_max / 100) + 1) * 100;
                                temp_min = ((temp_min / 100) - 1) * 100;
                                if (temp_max > 100000)
                                {
                                    temp_max = 100000;
                                }
                                if (temp_min < -100000)
                                {
                                    temp_min = -100000;
                                }
                                ptp_oc_offset_chart->axisY()->setMin(temp_min);
                                ptp_oc_offset_chart->axisY()->setMax(temp_max);

                                ptp_oc_offset_chart->show();
                            }
                        }
                        else
                        {
                            ntlts->ptpOc.CurrentDsOffsetValue->setText("NA");
                        }
                    }
                    else
                    {
                        ntlts->ptpOc.CurrentDsOffsetValue->setText("NA");
                    }

                    temp_string = ntlts->ptpOc.DelayMechanismValue->currentText();
                    if (temp_string == "P2P")
                    {
                        // peer delay
                        ntlts->ptpOc.CurrentDsDelayValue->setText("NA");
                    }
                    else if (0 == read_reg(temp_addr + Ucm_PtpOc_CurrentDs4Reg, &temp_data))
                    {
                        // end to end delay
                        temp_delay = temp_data;
                        temp_delay = temp_delay << 32;
                        if (0 == read_reg(temp_addr + Ucm_PtpOc_CurrentDs5Reg, &temp_data))
                        {
                            temp_delay |= temp_data;
                            temp_signed_delay = (long long)temp_delay;
                            temp_signed_delay = temp_signed_delay >> 16;
                            ntlts->ptpOc.CurrentDsDelayValue->setText(QString::number(temp_signed_delay));

                            if (true == ptp_oc_timer->isActive())
                            {
                                ptp_oc_delay_series->append(ptp_oc_delay_number_of_points, temp_signed_delay);

                                if (ptp_oc_delay_number_of_points < 20)
                                {
                                    ptp_oc_delay_number_of_points++;
                                }
                                else
                                {
                                    for (int j = 1; j < ptp_oc_delay_series->count(); j++)
                                    {
                                        QPointF temp_point = ptp_oc_delay_series->at(j);
                                        ptp_oc_delay_series->replace(j, (j - 1), temp_point.y());
                                    }
                                    ptp_oc_delay_series->remove(0);
                                }

                                temp_min = 0;
                                temp_max = 0;
                                for (int j = 0; j < ptp_oc_delay_series->count(); j++)
                                {
                                    QPointF temp_point = ptp_oc_delay_series->at(j);
                                    if (j == 0)
                                    {
                                        temp_min = temp_point.y();
                                        temp_max = temp_point.y();
                                    }
                                    if (temp_min > temp_point.y())
                                    {
                                        temp_min = temp_point.y();
                                    }
                                    if (temp_max < temp_point.y())
                                    {
                                        temp_max = temp_point.y();
                                    }
                                }
                                temp_max = ((temp_max / 100) + 1) * 100;
                                temp_min = ((temp_min / 100) - 1) * 100;
                                // if (temp_min < 0)
                                //{
                                //     temp_min = 0;
                                // }
                                ptp_oc_delay_chart->axisY()->setMin(temp_min);
                                ptp_oc_delay_chart->axisY()->setMax(temp_max);

                                ptp_oc_delay_chart->show();
                            }
                        }
                        else
                        {
                            ntlts->ptpOc.CurrentDsDelayValue->setText("NA");
                        }
                    }
                    else
                    {
                        ntlts->ptpOc.CurrentDsDelayValue->setText("NA");
                    }

                    break;
                }
                else if (i == 9)
                {
                    cout << "ERROR: " << "read did not complete" << endl;
                    ntlts->ptpOc.CurrentDsStepsRemovedValue->setText("NA");
                    ntlts->ptpOc.CurrentDsOffsetValue->setText("NA");
                }
            }
            else
            {
                ntlts->ptpOc.CurrentDsStepsRemovedValue->setText("NA");
                ntlts->ptpOc.CurrentDsOffsetValue->setText("NA");
            }
        }
    }
    else
    {
        ntlts->ptpOc.CurrentDsStepsRemovedValue->setText("NA");
        ntlts->ptpOc.CurrentDsOffsetValue->setText("NA");
    }

    //********************************
    // parent dataset
    //********************************
    temp_data = 0x40000000;
    if (0 == write_reg(temp_addr + Ucm_PtpOc_ParentDsControlReg, &temp_data))
    {
        for (int i = 0; i < 10; i++)
        {
            if (0 == read_reg(temp_addr + Ucm_PtpOc_ParentDsControlReg, &temp_data))
            {
                if ((temp_data & 0x80000000) != 0)
                {

                    // parent clock id and port id
                    temp_string.clear();
                    if (0 == read_reg(temp_addr + Ucm_PtpOc_ParentDs1Reg, &temp_data))
                    {
                        temp_string.append(QString("%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        temp_string.append(":");
                        temp_string.append(QString("%1").arg(((temp_data >> 8) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        temp_string.append(":");
                        temp_string.append(QString("%1").arg(((temp_data >> 16) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        temp_string.append(":");
                        temp_string.append(QString("%1").arg(((temp_data >> 24) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        temp_string.append(":");
                        if (0 == read_reg(temp_addr + Ucm_PtpOc_ParentDs2Reg, &temp_data))
                        {
                            temp_string.append(QString("%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));
                            temp_string.append(":");
                            temp_string.append(QString("%1").arg(((temp_data >> 8) & 0x000000FF), 2, 16, QLatin1Char('0')));
                            temp_string.append(":");
                            temp_string.append(QString("%1").arg(((temp_data >> 16) & 0x000000FF), 2, 16, QLatin1Char('0')));
                            temp_string.append(":");
                            temp_string.append(QString("%1").arg(((temp_data >> 24) & 0x000000FF), 2, 16, QLatin1Char('0')));
                            temp_string.append(".");
                            if (0 == read_reg(temp_addr + Ucm_PtpOc_ParentDs3Reg, &temp_data))
                            {
                                temp_string.append(QString("%1").arg(((temp_data >> 0) & 0x0000FFFF), 4, 16, QLatin1Char('0')));
                                ntlts->ptpOc.ParentDsParentClockIdValue->setText(temp_string);
                            }
                            else
                            {
                                ntlts->ptpOc.ParentDsParentClockIdValue->setText("NA");
                            }
                        }
                        else
                        {
                            ntlts->ptpOc.ParentDsParentClockIdValue->setText("NA");
                        }
                    }
                    else
                    {
                        ntlts->ptpOc.ParentDsParentClockIdValue->setText("NA");
                    }

                    // gm clock id
                    temp_string.clear();
                    if (0 == read_reg(temp_addr + Ucm_PtpOc_ParentDs4Reg, &temp_data))
                    {
                        temp_string.append(QString("%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        temp_string.append(":");
                        temp_string.append(QString("%1").arg(((temp_data >> 8) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        temp_string.append(":");
                        temp_string.append(QString("%1").arg(((temp_data >> 16) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        temp_string.append(":");
                        temp_string.append(QString("%1").arg(((temp_data >> 24) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        temp_string.append(":");
                        if (0 == read_reg(temp_addr + Ucm_PtpOc_ParentDs5Reg, &temp_data))
                        {
                            temp_string.append(QString("%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));
                            temp_string.append(":");
                            temp_string.append(QString("%1").arg(((temp_data >> 8) & 0x000000FF), 2, 16, QLatin1Char('0')));
                            temp_string.append(":");
                            temp_string.append(QString("%1").arg(((temp_data >> 16) & 0x000000FF), 2, 16, QLatin1Char('0')));
                            temp_string.append(":");
                            temp_string.append(QString("%1").arg(((temp_data >> 24) & 0x000000FF), 2, 16, QLatin1Char('0')));
                            ntlts->ptpOc.ParentDsGmClockIdValue->setText(temp_string);
                        }
                        else
                        {
                            ntlts->ptpOc.ParentDsGmClockIdValue->setText("NA");
                        }
                    }
                    else
                    {
                        ntlts->ptpOc.ParentDsGmClockIdValue->setText("NA");
                    }

                    // gm priority 1 & 2
                    if (0 == read_reg(temp_addr + Ucm_PtpOc_ParentDs3Reg, &temp_data))
                    {
                        ntlts->ptpOc.ParentDsGmPriority2Value->setText(QString("0x%1").arg(((temp_data >> 16) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        ntlts->ptpOc.ParentDsGmPriority1Value->setText(QString("0x%1").arg(((temp_data >> 24) & 0x000000FF), 2, 16, QLatin1Char('0')));
                    }
                    else
                    {
                        ntlts->ptpOc.ParentDsGmPriority1Value->setText("NA");
                        ntlts->ptpOc.ParentDsGmPriority2Value->setText("NA");
                    }

                    // variance, accuracy ,class
                    if (0 == read_reg(temp_addr + Ucm_PtpOc_ParentDs6Reg, &temp_data))
                    {
                        ntlts->ptpOc.ParentDsGmVarianceValue->setText(QString("0x%1").arg(((temp_data >> 0) & 0x0000FFFF), 4, 16, QLatin1Char('0')));
                        ntlts->ptpOc.ParentDsGmAccuracyValue->setText(QString::number(((temp_data >> 16) & 0x000000FF)));
                        ntlts->ptpOc.ParentDsGmClassValue->setText(QString("0x%1").arg(((temp_data >> 24) & 0x000000FF), 2, 16, QLatin1Char('0')));
                    }
                    else
                    {
                        ntlts->ptpOc.ParentDsGmVarianceValue->setText("NA");
                        ntlts->ptpOc.ParentDsGmAccuracyValue->setText("NA");
                        ntlts->ptpOc.ParentDsGmClassValue->setText("NA");
                    }

                    // gm short id
                    if (0 == read_reg(temp_addr + Ucm_PtpOc_ParentDs7Reg, &temp_data))
                    {
                        ntlts->ptpOc.ParentDsGmShortIdValue->setText(QString("0x%1").arg(temp_data, 4, 16, QLatin1Char('0')));
                    }
                    else
                    {
                        ntlts->ptpOc.ParentDsGmShortIdValue->setText("NA");
                    }

                    // gm inaccuracy
                    if (0 == read_reg(temp_addr + Ucm_PtpOc_ParentDs8Reg, &temp_data))
                    {
                        ntlts->ptpOc.ParentDsGmInaccuracyValue->setText(QString::number(temp_data));
                    }
                    else
                    {
                        ntlts->ptpOc.ParentDsGmInaccuracyValue->setText("NA");
                    }

                    // nw inaccuracy
                    if (0 == read_reg(temp_addr + Ucm_PtpOc_ParentDs9Reg, &temp_data))
                    {
                        ntlts->ptpOc.ParentDsNwInaccuracyValue->setText(QString::number(temp_data));
                    }
                    else
                    {
                        ntlts->ptpOc.ParentDsNwInaccuracyValue->setText("NA");
                    }

                    break;
                }
                else if (i == 9)
                {
                    cout << "ERROR: " << "read did not complete" << endl;
                    ntlts->ptpOc.ParentDsParentClockIdValue->setText("NA");
                    ntlts->ptpOc.ParentDsGmClockIdValue->setText("NA");
                    ntlts->ptpOc.ParentDsGmPriority1Value->setText("NA");
                    ntlts->ptpOc.ParentDsGmPriority2Value->setText("NA");
                    ntlts->ptpOc.ParentDsGmAccuracyValue->setText("NA");
                    ntlts->ptpOc.ParentDsGmClassValue->setText("NA");
                    ntlts->ptpOc.ParentDsGmShortIdValue->setText("NA");
                    ntlts->ptpOc.ParentDsGmInaccuracyValue->setText("NA");
                    ntlts->ptpOc.ParentDsNwInaccuracyValue->setText("NA");
                }
            }
            else
            {
                ntlts->ptpOc.ParentDsParentClockIdValue->setText("NA");
                ntlts->ptpOc.ParentDsGmClockIdValue->setText("NA");
                ntlts->ptpOc.ParentDsGmPriority1Value->setText("NA");
                ntlts->ptpOc.ParentDsGmPriority2Value->setText("NA");
                ntlts->ptpOc.ParentDsGmAccuracyValue->setText("NA");
                ntlts->ptpOc.ParentDsGmClassValue->setText("NA");
                ntlts->ptpOc.ParentDsGmShortIdValue->setText("NA");
                ntlts->ptpOc.ParentDsGmInaccuracyValue->setText("NA");
                ntlts->ptpOc.ParentDsNwInaccuracyValue->setText("NA");
            }
        }
    }
    else
    {
        ntlts->ptpOc.ParentDsParentClockIdValue->setText("NA");
        ntlts->ptpOc.ParentDsGmClockIdValue->setText("NA");
        ntlts->ptpOc.ParentDsGmPriority1Value->setText("NA");
        ntlts->ptpOc.ParentDsGmPriority2Value->setText("NA");
        ntlts->ptpOc.ParentDsGmAccuracyValue->setText("NA");
        ntlts->ptpOc.ParentDsGmClassValue->setText("NA");
        ntlts->ptpOc.ParentDsGmShortIdValue->setText("NA");
        ntlts->ptpOc.ParentDsGmInaccuracyValue->setText("NA");
        ntlts->ptpOc.ParentDsNwInaccuracyValue->setText("NA");
    }

    //********************************
    // time properties dataset
    //********************************
    ntlts->ptpOc.TimePropertiesDsSetLocalProperties = 0;
    temp_data = 0x40000000;
    if (0 == write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDsControlReg, &temp_data))
    {
        for (int i = 0; i < 10; i++)
        {
            if (0 == read_reg(temp_addr + Ucm_PtpOc_TimePropertiesDsControlReg, &temp_data))
            {
                if ((temp_data & 0x80000000) != 0)
                {

                    // time source, ptp timescale, freq traceable, time traceable, lep61, leap 59, ut offset val, utc offset
                    if (0 == read_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs1Reg, &temp_data))
                    {
                        ntlts->ptpOc.TimePropertiesDsTimeSourceValue->setText(QString("0x%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        if ((temp_data & 0x00000100) != 0)
                        {
                            ntlts->ptpOc.TimePropertiesDsPtpTimescale = 1;
                        }
                        else
                        {
                            ntlts->ptpOc.TimePropertiesDsPtpTimescale = 0;
                        }
                        if ((temp_data & 0x00000200) != 0)
                        {
                            ntlts->ptpOc.TimePropertiesDsFreqTraceable = 1;
                        }
                        else
                        {
                            ntlts->ptpOc.TimePropertiesDsFreqTraceable = 0;
                        }
                        if ((temp_data & 0x00000400) != 0)
                        {
                            ntlts->ptpOc.TimePropertiesDsTimeTraceable = 1;
                        }
                        else
                        {
                            ntlts->ptpOc.TimePropertiesDsTimeTraceable = 0;
                        }
                        if ((temp_data & 0x00000800) != 0)
                        {
                            ntlts->ptpOc.TimePropertiesDsLeap61 = 1;
                        }
                        else
                        {
                            ntlts->ptpOc.TimePropertiesDsLeap61 = 0;
                        }
                        if ((temp_data & 0x00001000) != 0)
                        {
                            ntlts->ptpOc.TimePropertiesDsLeap59 = 1;
                        }
                        else
                        {
                            ntlts->ptpOc.TimePropertiesDsLeap59 = 0;
                        }
                        if ((temp_data & 0x00002000) != 0)
                        {
                            ntlts->ptpOc.TimePropertiesDsUtcOffsetVal = 1;
                        }
                        else
                        {
                            ntlts->ptpOc.TimePropertiesDsUtcOffsetVal = 0;
                        }

                        ntlts->ptpOc.TimePropertiesDsUtcOffsetValue->setText(QString::number((signed short)((temp_data >> 16) & 0x0000FFFF)));
                    }
                    else
                    {
                        ntlts->ptpOc.TimePropertiesDsTimeSourceValue->setText("NA");
                        ntlts->ptpOc.TimePropertiesDsPtpTimescale = 0;
                        ntlts->ptpOc.TimePropertiesDsFreqTraceable = 0;
                        ntlts->ptpOc.TimePropertiesDsTimeTraceable = 0;
                        ntlts->ptpOc.TimePropertiesDsLeap59 = 0;
                        ntlts->ptpOc.TimePropertiesDsLeap61 = 0;
                        ntlts->ptpOc.TimePropertiesDsUtcOffsetVal = 0;
                        ntlts->ptpOc.TimePropertiesDsUtcOffsetValue->setText("NA");
                    }

                    // current offset
                    if (0 == read_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs2Reg, &temp_data))
                    {
                        ntlts->ptpOc.TimePropertiesDsCurrentOffsetValue->setText(QString::number((int)temp_data));
                    }
                    else
                    {
                        ntlts->ptpOc.TimePropertiesDsCurrentOffsetValue->setText("NA");
                    }

                    // jump seconds
                    if (0 == read_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs3Reg, &temp_data))
                    {
                        ntlts->ptpOc.TimePropertiesDsJumpSecondsValue->setText(QString::number((int)temp_data));
                    }
                    else
                    {
                        ntlts->ptpOc.TimePropertiesDsJumpSecondsValue->setText("NA");
                    }

                    // next jump
                    if (0 == read_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs4Reg, &temp_data))
                    {
                        temp_next_jump = temp_data;
                        temp_next_jump = temp_next_jump << 32;
                        if (0 == read_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs5Reg, &temp_data))
                        {
                            temp_next_jump |= temp_data;
                            ntlts->ptpOc.TimePropertiesDsNextJumpValue->setText(QString::number(temp_next_jump));
                        }
                        else
                        {
                            ntlts->ptpOc.TimePropertiesDsNextJumpValue->setText("NA");
                        }
                    }
                    else
                    {
                        ntlts->ptpOc.TimePropertiesDsNextJumpValue->setText("NA");
                    }

                    // display name
                    temp_string.clear();
                    if (0 == read_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs6Reg, temp_length))
                    {
                        for (int j = 0; j < 3; j++)
                        {
                            if (0 == read_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs7Reg + (j * 4), &temp_data))
                            {
                                temp_string.append((QChar)((temp_data >> 0) & 0x000000FF));
                                temp_string.append((QChar)((temp_data >> 8) & 0x000000FF));
                                temp_string.append((QChar)((temp_data >> 16) & 0x000000FF));
                                temp_string.append((QChar)((temp_data >> 24) & 0x000000FF));
                            }
                            else
                            {
                                temp_string.clear();
                                temp_string.append("NA");
                                break;
                            }
                        }
                        temp_string.truncate(temp_length);
                        ntlts->ptpOc.TimePropertiesDsDisplayNameValue->setText(temp_string);
                    }
                    else
                    {
                        ntlts->ptpOc.TimePropertiesDsDisplayNameValue->setText("NA");
                    }
                    break;
                }
                else if (i == 9)
                {
                    cout << "ERROR: " << "read did not complete" << endl;
                    ntlts->ptpOc.TimePropertiesDsTimeSourceValue->setText("NA");
                    ntlts->ptpOc.TimePropertiesDsPtpTimescale = 0;
                    ntlts->ptpOc.TimePropertiesDsFreqTraceable = 0;
                    ntlts->ptpOc.TimePropertiesDsTimeTraceable = 0;
                    ntlts->ptpOc.TimePropertiesDsLeap59 = 0;
                    ntlts->ptpOc.TimePropertiesDsLeap61 = 0;
                    ntlts->ptpOc.TimePropertiesDsUtcOffsetVal = 0;
                    ntlts->ptpOc.TimePropertiesDsUtcOffsetValue->setText("NA");
                    ntlts->ptpOc.TimePropertiesDsCurrentOffsetValue->setText("NA");
                    ntlts->ptpOc.TimePropertiesDsJumpSecondsValue->setText("NA");
                    ntlts->ptpOc.TimePropertiesDsNextJumpValue->setText("NA");
                    ntlts->ptpOc.TimePropertiesDsDisplayNameValue->setText("NA");
                }
            }
            else
            {
                ntlts->ptpOc.TimePropertiesDsTimeSourceValue->setText("NA");
                ntlts->ptpOc.TimePropertiesDsPtpTimescale = 0;
                ntlts->ptpOc.TimePropertiesDsFreqTraceable = 0;
                ntlts->ptpOc.TimePropertiesDsTimeTraceable = 0;
                ntlts->ptpOc.TimePropertiesDsLeap59 = 0;
                ntlts->ptpOc.TimePropertiesDsLeap61 = 0;
                ntlts->ptpOc.TimePropertiesDsUtcOffsetVal = 0;
                ntlts->ptpOc.TimePropertiesDsUtcOffsetValue->setText("NA");
                ntlts->ptpOc.TimePropertiesDsCurrentOffsetValue->setText("NA");
                ntlts->ptpOc.TimePropertiesDsJumpSecondsValue->setText("NA");
                ntlts->ptpOc.TimePropertiesDsNextJumpValue->setText("NA");
                ntlts->ptpOc.TimePropertiesDsDisplayNameValue->setText("NA");
            }
        }
    }
    else
    {
        ntlts->ptpOc.TimePropertiesDsTimeSourceValue->setText("NA");
        ntlts->ptpOc.TimePropertiesDsPtpTimescale = 0;
        ntlts->ptpOc.TimePropertiesDsFreqTraceable = 0;
        ntlts->ptpOc.TimePropertiesDsTimeTraceable = 0;
        ntlts->ptpOc.TimePropertiesDsLeap59 = 0;
        ntlts->ptpOc.TimePropertiesDsLeap61 = 0;
        ntlts->ptpOc.TimePropertiesDsUtcOffsetVal = 0;
        ntlts->ptpOc.TimePropertiesDsUtcOffsetValue->setText("NA");
        ntlts->ptpOc.TimePropertiesDsCurrentOffsetValue->setText("NA");
        ntlts->ptpOc.TimePropertiesDsJumpSecondsValue->setText("NA");
        ntlts->ptpOc.TimePropertiesDsNextJumpValue->setText("NA");
        ntlts->ptpOc.TimePropertiesDsDisplayNameValue->setText("NA");
    }
        */

    // version
    if (0 == read_reg(temp_addr + Ucm_PtpOc_VersionReg, &temp_data))
    {
        ntlts->ptpOc.Version = temp_data;
    }
    else
    {
        ntlts->ptpOc.Version = temp_data;
    }

    return 0;
}
uint8_t ptp_oc_write_values(NTL_TS_T *ntlts, uint8_t fromRegisters) { return 0; }

#endif