#include "ntl_uart.h"
// #include "uartCircBuff.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"

int8_t ptp_oc_read_values(NTL_TS_T *ntlts)
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
        // return 88;
        //  temp_string.clear();
        if (0 == read_reg(temp_addr + Ucm_PtpOc_ConfigIpReg, &temp_data))
        {
            printf("tempdata from r0: %d\n", temp_data);
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

                        // printf(ntlts->ptpOc.ipAddr);

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

    temp_data = 0x40000000;
    if (0 == write_reg(temp_addr + Ucm_PtpOc_PortDsControlReg, &temp_data))
    {
        for (int i = 0; i < 10; i++)
        {
            if (0 == read_reg(temp_addr + Ucm_PtpOc_PortDsControlReg, &temp_data))
            {
                if ((temp_data & 0x80000000) != 0)
                {
                    // temp_string = ntlts->ptpOc.DelayMechanismValue->currentText();
                    // if ((temp_string == "E2E") or(temp_string == "E2E Unicast"))

                    if (strncmp(ntlts->ptpOc.DelayMechanism, "E2E", strlen("E2E")) == 0 || strncmp(ntlts->ptpOc.DelayMechanism, "E2E Unicast", strlen("E2E Unicast")) == 0)
                    {
                        // end to end delay
                        // ntlts->ptpOc.PortDsPeerDelayValue->setText("NA");
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
                            // ntlts->ptpOc.PortDsPeerDelayValue->setText(QString::number(temp_signed_delay));

                            ntlts->ptpOc.PortDsPeerDelay = temp_signed_delay;

                            // if (true == ptp_oc_timer->isActive())
                            //{
                            //     ptp_oc_delay_series->append(ptp_oc_delay_number_of_points, temp_signed_delay);
                            //
                            //    if (ptp_oc_delay_number_of_points < 20)
                            //    {
                            //        ptp_oc_delay_number_of_points++;
                            //    }
                            //    else
                            //    {
                            //        for (int j = 1; j < ptp_oc_delay_series->count(); j++)
                            //        {
                            //            QPointF temp_point = ptp_oc_delay_series->at(j);
                            //            ptp_oc_delay_series->replace(j, (j - 1), temp_point.y());
                            //        }
                            //        ptp_oc_delay_series->remove(0);
                            //    }
                            //
                            //    temp_min = 0;
                            //    temp_max = 0;
                            //    for (int j = 0; j < ptp_oc_delay_series->count(); j++)
                            //    {
                            //        QPointF temp_point = ptp_oc_delay_series->at(j);
                            //        if (j == 0)
                            //        {
                            //            temp_min = temp_point.y();
                            //            temp_max = temp_point.y();
                            //        }
                            //        if (temp_min > temp_point.y())
                            //        {
                            //            temp_min = temp_point.y();
                            //        }
                            //        if (temp_max < temp_point.y())
                            //        {
                            //            temp_max = temp_point.y();
                            //        }
                            //    }
                            //    temp_max = ((temp_max / 100) + 1) * 100;
                            //    temp_min = ((temp_min / 100) - 1) * 100;
                            //    // if (temp_min < 0)
                            //    //{
                            //    //     temp_min = 0;
                            //    // }
                            //    ptp_oc_delay_chart->axisY()->setMin(temp_min);
                            //    ptp_oc_delay_chart->axisY()->setMax(temp_max);
                            //
                            //    ptp_oc_delay_chart->show();
                            //}
                        }
                        else
                        {
                            // ntlts->ptpOc.PortDsPeerDelayValue->setText("NA");
                        }
                    }
                    else
                    {
                        // ntlts->ptpOc.PortDsPeerDelayValue->setText("NA");
                    }

                    // state
                    if (0 == read_reg(temp_addr + Ucm_PtpOc_PortDs3Reg, &temp_data))
                    {
                        switch (temp_data)
                        {
                        case 0x0000001:
                            // ntlts->ptpOc.PortDsStateValue->setText("INITIALIZING");
                            snprintf(ntlts->ptpOc.PortDsState, sizeof(ntlts->ptpOc.PortDsState), "INITIALIZING");
                            // snprintf(ntlts->ptpOc.Layer, sizeof(ntlts->ptpOc.Layer), "NA");

                            break;
                        case 0x00000002:
                            // ntlts->ptpOc.PortDsStateValue->setText("FAULTY");
                            snprintf(ntlts->ptpOc.PortDsState, sizeof(ntlts->ptpOc.PortDsState), "FAULTY");
                            break;
                        case 0x00000003:
                            // ntlts->ptpOc.PortDsStateValue->setText("DISABLED");
                            snprintf(ntlts->ptpOc.PortDsState, sizeof(ntlts->ptpOc.PortDsState), "DISABLED");
                            break;
                        case 0x00000004:
                            // ntlts->ptpOc.PortDsStateValue->setText("LISTENING");
                            snprintf(ntlts->ptpOc.PortDsState, sizeof(ntlts->ptpOc.PortDsState), "LISTENING");
                            break;
                        case 0x00000005:
                            // ntlts->ptpOc.PortDsStateValue->setText("PREMASTER");
                            snprintf(ntlts->ptpOc.PortDsState, sizeof(ntlts->ptpOc.PortDsState), "PREMASTER");
                            break;
                        case 0x00000006:
                            // ntlts->ptpOc.PortDsStateValue->setText("MASTER");
                            snprintf(ntlts->ptpOc.PortDsState, sizeof(ntlts->ptpOc.PortDsState), "MASTER");
                            break;
                        case 0x00000007:
                            // ntlts->ptpOc.PortDsStateValue->setText("PASSIVE");
                            snprintf(ntlts->ptpOc.PortDsState, sizeof(ntlts->ptpOc.PortDsState), "PASSIVE");
                            break;
                        case 0x00000008:
                            // ntlts->ptpOc.PortDsStateValue->setText("UNCALIBRATED");
                            snprintf(ntlts->ptpOc.PortDsState, sizeof(ntlts->ptpOc.PortDsState), "UNCALIBRATED");
                            break;
                        case 0x00000009:
                            // ntlts->ptpOc.PortDsStateValue->setText("SLAVE");
                            snprintf(ntlts->ptpOc.PortDsState, sizeof(ntlts->ptpOc.PortDsState), "SLAVE");
                            break;
                        default:
                            // ntlts->ptpOc.PortDsStateValue->setText("NA");
                            snprintf(ntlts->ptpOc.PortDsState, sizeof(ntlts->ptpOc.PortDsState), "NA");
                            break;
                        }
                    }
                    else
                    {
                        // ntlts->ptpOc.PortDsStateValue->setText("NA");
                        snprintf(ntlts->ptpOc.PortDsState, sizeof(ntlts->ptpOc.PortDsState), "NA");
                    }

                    // pdelay and delay req log msg interval and max delay
                    if (0 == read_reg(temp_addr + Ucm_PtpOc_PortDs4Reg, &temp_data))
                    {
                        // ntlts->ptpOc.PortDsPDelayReqLogMsgIntervalValue->setText(QString::number((signed char)(temp_data & 0x000000FF)));
                        // ntlts->ptpOc.PortDsDelayReqLogMsgIntervalValue->setText(QString::number((signed char)((temp_data >> 8) & 0x000000FF)));
                        // ntlts->ptpOc.PortDsDelayReceiptTimeoutValue->setText(QString::number(((temp_data >> 16) & 0x000000FF)));

                        ntlts->ptpOc.PortDsPDelayReqLogMsgInterval = ((signed char)(temp_data & 0x000000FF));
                        ntlts->ptpOc.PortDsDelayReqLogMsgInterval = ((signed char)((temp_data >> 8) & 0x000000FF));
                        ntlts->ptpOc.PortDsDelayReceiptTimeout = (((temp_data >> 16) & 0x000000FF));
                    }
                    else
                    {
                        // ntlts->ptpOc.PortDsPDelayReqLogMsgIntervalValue->setText("NA");
                        // ntlts->ptpOc.PortDsDelayReqLogMsgIntervalValue->setText("NA");
                        // ntlts->ptpOc.PortDsDelayReceiptTimeoutValue->setText("NA");
                        return -1;
                    }

                    // announce log msg interval and announce receipt timeout
                    if (0 == read_reg(temp_addr + Ucm_PtpOc_PortDs5Reg, &temp_data))
                    {
                        // ntlts->ptpOc.PortDsAnnounceLogMsgIntervalValue->setText(QString::number((signed char)(temp_data & 0x000000FF)));
                        // ntlts->ptpOc.PortDsAnnounceReceiptTimeoutValue->setText(QString::number(((temp_data >> 8) & 0x000000FF)));

                        ntlts->ptpOc.PortDsAnnounceLogMsgInterval = (signed char)(temp_data & 0x000000FF);
                        ntlts->ptpOc.PortDsAnnounceReceiptTimeout = ((temp_data >> 8) & 0x000000FF);
                    }
                    else
                    {
                        // ntlts->ptpOc.PortDsAnnounceLogMsgIntervalValue->setText("NA");
                        // ntlts->ptpOc.PortDsAnnounceReceiptTimeoutValue->setText("NA");
                    }

                    // sync log msg interval and sync receipt timeout
                    if (0 == read_reg(temp_addr + Ucm_PtpOc_PortDs6Reg, &temp_data))
                    {
                        // ntlts->ptpOc.PortDsSyncLogMsgIntervalValue->setText(QString::number((signed char)(temp_data & 0x000000FF)));
                        // ntlts->ptpOc.PortDsSyncReceiptTimeoutValue->setText(QString::number(((temp_data >> 8) & 0x000000FF)));

                        ntlts->ptpOc.PortDsSyncLogMsgInterval = ((signed char)(temp_data & 0x000000FF));
                        ntlts->ptpOc.PortDsSyncReceiptTimeout = (((temp_data >> 8) & 0x000000FF));
                    }
                    else
                    {
                        // ntlts->ptpOc.PortDsSyncLogMsgIntervalValue->setText("NA");
                        // ntlts->ptpOc.PortDsSyncReceiptTimeoutValue->setText("NA");
                    }

                    // asymmetry
                    if (0 == read_reg(temp_addr + Ucm_PtpOc_PortDs7Reg, &temp_data))
                    {
                        // ntlts->ptpOc.PortDsAsymmetry->setText(QString::number((signed int)temp_data));
                        ntlts->ptpOc.PortDsAsymmetry = temp_data;
                    }
                    else
                    {
                        // ntlts->ptpOc.PortDsAsymmetryValue->setText("NA");
                    }

                    // max pdelay
                    if (0 == read_reg(temp_addr + Ucm_PtpOc_PortDs8Reg, &temp_data))
                    {
                        // ntlts->ptpOc.PortDsMaxPeerDelayValue->setText(QString::number(temp_data));
                        ntlts->ptpOc.PortDsMaxPeerDelay = temp_data;
                    }
                    else
                    {
                        // ntlts->ptpOc.PortDsMaxPeerDelayValue->setText("NA");
                    }

                    break;
                }
                else if (i == 9)
                {
                    //  cout << "ERROR: " << "read did not complete" << endl;
                    //  ntlts->ptpOc.PortDsPeerDelayValue->setText("NA");
                    //  ntlts->ptpOc.PortDsStateValue->setText("NA");
                    //  ntlts->ptpOc.PortDsAsymmetryValue->setText("NA");
                    //  ntlts->ptpOc.PortDsMaxPeerDelayValue->setText("NA");
                    //  ntlts->ptpOc.PortDsPDelayReqLogMsgIntervalValue->setText("NA");
                    //  ntlts->ptpOc.PortDsDelayReqLogMsgIntervalValue->setText("NA");
                    //  ntlts->ptpOc.PortDsDelayReceiptTimeoutValue->setText("NA");
                    //  ntlts->ptpOc.PortDsAnnounceLogMsgIntervalValue->setText("NA");
                    //  ntlts->ptpOc.PortDsAnnounceReceiptTimeoutValue->setText("NA");
                    //  ntlts->ptpOc.PortDsSyncLogMsgIntervalValue->setText("NA");
                    //  ntlts->ptpOc.PortDsSyncReceiptTimeoutValue->setText("NA");
                }
            }
            else
            {
                // ntlts->ptpOc.PortDsPeerDelayValue->setText("NA");
                // ntlts->ptpOc.PortDsStateValue->setText("NA");
                // ntlts->ptpOc.PortDsAsymmetryValue->setText("NA");
                // ntlts->ptpOc.PortDsMaxPeerDelayValue->setText("NA");
                // ntlts->ptpOc.PortDsPDelayReqLogMsgIntervalValue->setText("NA");
                // ntlts->ptpOc.PortDsDelayReqLogMsgIntervalValue->setText("NA");
                // ntlts->ptpOc.PortDsDelayReceiptTimeoutValue->setText("NA");
                // ntlts->ptpOc.PortDsAnnounceLogMsgIntervalValue->setText("NA");
                // ntlts->ptpOc.PortDsAnnounceReceiptTimeoutValue->setText("NA");
                // ntlts->ptpOc.PortDsSyncLogMsgIntervalValue->setText("NA");
                // ntlts->ptpOc.PortDsSyncReceiptTimeoutValue->setText("NA");
            }
        }
    }
    else
    {
        // ntlts->ptpOc.PortDsPeerDelayValue->setText("NA");
        // ntlts->ptpOc.PortDsStateValue->setText("NA");
        // ntlts->ptpOc.PortDsAsymmetryValue->setText("NA");
        // ntlts->ptpOc.PortDsMaxPeerDelayValue->setText("NA");
        // ntlts->ptpOc.PortDsPDelayReqLogMsgIntervalValue->setText("NA");
        // ntlts->ptpOc.PortDsDelayReqLogMsgIntervalValue->setText("NA");
        // ntlts->ptpOc.PortDsDelayReceiptTimeoutValue->setText("NA");
        // ntlts->ptpOc.PortDsAnnounceLogMsgIntervalValue->setText("NA");
        // ntlts->ptpOc.PortDsAnnounceReceiptTimeoutValue->setText("NA");
        // ntlts->ptpOc.PortDsSyncLogMsgIntervalValue->setText("NA");
        // ntlts->ptpOc.PortDsSyncReceiptTimeoutValue->setText("NA");
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
                        // ntlts->ptpOc.CurrentDsStepsRemovedValue->setText(QString::number(temp_data & 0xFFFF));
                        ntlts->ptpOc.CurrentDsStepsRemoved = temp_data & 0xFFFF;
                    }
                    else
                    {
                        // ntlts->ptpOc.CurrentDsStepsRemovedValue->setText("NA");
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

                            // ntlts->ptpOc.CurrentDsOffsetValue->setText(QString::number(temp_signed_offset));

                            ntlts->ptpOc.CurrentDsOffset = temp_signed_offset;

                            // if (true == ptp_oc_timer->isActive())
                            //{
                            //
                            //    ptp_oc_offset_series->append(ptp_oc_offset_number_of_points, temp_signed_offset);
                            //
                            //    if (ptp_oc_offset_number_of_points < 20)
                            //    {
                            //        ptp_oc_offset_number_of_points++;
                            //    }
                            //    else
                            //    {
                            //        for (int j = 1; j < ptp_oc_offset_series->count(); j++)
                            //        {
                            //            QPointF temp_point = ptp_oc_offset_series->at(j);
                            //            ptp_oc_offset_series->replace(j, (j - 1), temp_point.y());
                            //        }
                            //        ptp_oc_offset_series->remove(0);
                            //    }
                            //
                            //    temp_min = 0;
                            //    temp_max = 0;
                            //    for (int j = 0; j < ptp_oc_offset_series->count(); j++)
                            //    {
                            //        QPointF temp_point = ptp_oc_offset_series->at(j);
                            //        if (j == 0)
                            //        {
                            //            temp_min = temp_point.y();
                            //            temp_max = temp_point.y();
                            //        }
                            //        if (temp_min > temp_point.y())
                            //        {
                            //            temp_min = temp_point.y();
                            //        }
                            //        if (temp_max < temp_point.y())
                            //        {
                            //            temp_max = temp_point.y();
                            //        }
                            //    }
                            //    temp_max = ((temp_max / 100) + 1) * 100;
                            //    temp_min = ((temp_min / 100) - 1) * 100;
                            //    if (temp_max > 100000)
                            //    {
                            //        temp_max = 100000;
                            //    }
                            //    if (temp_min < -100000)
                            //    {
                            //        temp_min = -100000;
                            //    }
                            //    ptp_oc_offset_chart->axisY()->setMin(temp_min);
                            //    ptp_oc_offset_chart->axisY()->setMax(temp_max);
                            //
                            //    ptp_oc_offset_chart->show();
                            //}
                        }
                        else
                        {
                            // ntlts->ptpOc.CurrentDsOffsetValue->setText("NA");
                        }
                    }
                    else
                    {
                        // ntlts->ptpOc.CurrentDsOffsetValue->setText("NA");
                        return -4;
                    }

                    // temp_string = ntlts->ptpOc.DelayMechanismValue->currentText();
                    if (strncmp(ntlts->ptpOc.DelayMechanism, "P2P", strlen("P2P")) == 0)
                    // if (temp_string == "P2P")
                    {
                        // peer delay
                        // ntlts->ptpOc.CurrentDsDelayValue->setText("NA");
                        // ntlts->ptpOc.CurrentDsDelay =
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
                            // ntlts->ptpOc.CurrentDsDelayValue->setText(QString::number(temp_signed_delay));

                            ntlts->ptpOc.CurrentDsDelay = temp_signed_delay;

                            // if (true == ptp_oc_timer->isActive())
                            //{
                            //     ptp_oc_delay_series->append(ptp_oc_delay_number_of_points, temp_signed_delay);
                            //
                            //    if (ptp_oc_delay_number_of_points < 20)
                            //    {
                            //        ptp_oc_delay_number_of_points++;
                            //    }
                            //    else
                            //    {
                            //        for (int j = 1; j < ptp_oc_delay_series->count(); j++)
                            //        {
                            //            QPointF temp_point = ptp_oc_delay_series->at(j);
                            //            ptp_oc_delay_series->replace(j, (j - 1), temp_point.y());
                            //        }
                            //        ptp_oc_delay_series->remove(0);
                            //    }
                            //
                            //    temp_min = 0;
                            //    temp_max = 0;
                            //    for (int j = 0; j < ptp_oc_delay_series->count(); j++)
                            //    {
                            //        QPointF temp_point = ptp_oc_delay_series->at(j);
                            //        if (j == 0)
                            //        {
                            //            temp_min = temp_point.y();
                            //            temp_max = temp_point.y();
                            //        }
                            //        if (temp_min > temp_point.y())
                            //        {
                            //            temp_min = temp_point.y();
                            //        }
                            //        if (temp_max < temp_point.y())
                            //        {
                            //            temp_max = temp_point.y();
                            //        }
                            //    }
                            //    temp_max = ((temp_max / 100) + 1) * 100;
                            //    temp_min = ((temp_min / 100) - 1) * 100;
                            //    // if (temp_min < 0)
                            //    //{
                            //    //     temp_min = 0;
                            //    // }
                            //    ptp_oc_delay_chart->axisY()->setMin(temp_min);
                            //    ptp_oc_delay_chart->axisY()->setMax(temp_max);
                            //
                            //    ptp_oc_delay_chart->show();
                            //}
                        }
                        else
                        {
                            // ntlts->ptpOc.CurrentDsDelayValue->setText("NA");
                        }
                    }
                    else
                    {
                        // ntlts->ptpOc.CurrentDsDelayValue->setText("NA");
                    }

                    break;
                }
                else if (i == 9)
                {
                    // cout << "ERROR: " << "read did not complete" << endl;
                    // ntlts->ptpOc.CurrentDsStepsRemovedValue->setText("NA");
                    // ntlts->ptpOc.CurrentDsOffsetValue->setText("NA");
                }
            }
            else
            {
                // ntlts->ptpOc.CurrentDsStepsRemovedValue->setText("NA");
                // ntlts->ptpOc.CurrentDsOffsetValue->setText("NA");
            }
        }
    }
    else
    {
        // ntlts->ptpOc.CurrentDsStepsRemovedValue->setText("NA");
        // ntlts->ptpOc.CurrentDsOffsetValue->setText("NA");
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
                    uint8_t temp_clock_id[8];
                    memset(temp_clock_id, 0, sizeof(temp_clock_id));

                    if (0 == read_reg(temp_addr + Ucm_PtpOc_ParentDs1Reg, &temp_data))
                    {
                        temp_clock_id[0] = ((temp_data >> 0) & 0x000000FF);
                        temp_clock_id[1] = ((temp_data >> 8) & 0x000000FF);
                        temp_clock_id[2] = ((temp_data >> 16) & 0x000000FF);
                        temp_clock_id[3] = ((temp_data >> 24) & 0x000000FF);

                        if (0 == read_reg(temp_addr + Ucm_PtpOc_ParentDs2Reg, &temp_data))
                        {
                            temp_clock_id[4] = ((temp_data >> 0) & 0x000000FF);
                            temp_clock_id[5] = ((temp_data >> 8) & 0x000000FF);
                            temp_clock_id[6] = ((temp_data >> 16) & 0x000000FF);
                            temp_clock_id[7] = ((temp_data >> 24) & 0x000000FF);

                            snprintf(ntlts->ptpOc.ParentDsParentClockId, sizeof(ntlts->ptpOc.ParentDsParentClockId),
                                     "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x.%04x",
                                     temp_clock_id[0], temp_clock_id[1], temp_clock_id[2], temp_clock_id[3],
                                     temp_clock_id[4], temp_clock_id[5], temp_clock_id[6], temp_clock_id[7],
                                     (temp_data >> 16) & 0x0000FFFF);
                        }
                        else
                        {
                            snprintf(ntlts->ptpOc.ParentDsParentClockId, sizeof(ntlts->ptpOc.ParentDsParentClockId), "NA");
                        }
                    }
                    else
                    {
                        snprintf(ntlts->ptpOc.ParentDsParentClockId, sizeof(ntlts->ptpOc.ParentDsParentClockId), "NA");
                    }

                    memset(temp_clock_id, 0, sizeof(temp_clock_id));
                    if (0 == read_reg(temp_addr + Ucm_PtpOc_ParentDs4Reg, &temp_data))
                    {
                        temp_clock_id[0] = ((temp_data >> 0) & 0x000000FF);
                        temp_clock_id[1] = ((temp_data >> 8) & 0x000000FF);
                        temp_clock_id[2] = ((temp_data >> 16) & 0x000000FF);
                        temp_clock_id[3] = ((temp_data >> 24) & 0x000000FF);
                        if (0 == read_reg(temp_addr + Ucm_PtpOc_ParentDs5Reg, &temp_data))
                        {
                            temp_clock_id[4] = ((temp_data >> 0) & 0x000000FF);
                            temp_clock_id[5] = ((temp_data >> 8) & 0x000000FF);
                            temp_clock_id[6] = ((temp_data >> 16) & 0x000000FF);
                            temp_clock_id[7] = ((temp_data >> 24) & 0x000000FF);

                            snprintf(ntlts->ptpOc.ParentDsGmClockId, sizeof(ntlts->ptpOc.ParentDsGmClockId),
                                     "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",
                                     temp_clock_id[0], temp_clock_id[1], temp_clock_id[2], temp_clock_id[3],
                                     temp_clock_id[4], temp_clock_id[5], temp_clock_id[6], temp_clock_id[7]);
                        }
                        else
                        {
                            snprintf(ntlts->ptpOc.ParentDsGmClockId, sizeof(ntlts->ptpOc.ParentDsGmClockId), "NA");
                        }
                    }
                    else
                    {
                        snprintf(ntlts->ptpOc.ParentDsGmClockId, sizeof(ntlts->ptpOc.ParentDsGmClockId), "NA");
                    }

                    if (0 == read_reg(temp_addr + Ucm_PtpOc_ParentDs3Reg, &temp_data))
                    {
                        ntlts->ptpOc.ParentDsGmPriority2 = ((temp_data >> 16) & 0x000000FF);
                        ntlts->ptpOc.ParentDsGmPriority1 = ((temp_data >> 24) & 0x000000FF);
                    }
                    else
                    {
                        ntlts->ptpOc.ParentDsGmPriority1 = 0;
                        ntlts->ptpOc.ParentDsGmPriority2 = 0;
                    }

                    if (0 == read_reg(temp_addr + Ucm_PtpOc_ParentDs6Reg, &temp_data))
                    {
                        ntlts->ptpOc.ParentDsGmVariance = ((temp_data >> 0) & 0x0000FFFF);
                        ntlts->ptpOc.ParentDsGmAccuracy = ((temp_data >> 16) & 0x000000FF);
                        ntlts->ptpOc.ParentDsGmClass = ((temp_data >> 24) & 0x000000FF);
                    }
                    else
                    {
                        ntlts->ptpOc.ParentDsGmVariance = 0;
                        ntlts->ptpOc.ParentDsGmAccuracy = 0;
                        ntlts->ptpOc.ParentDsGmClass = 0;
                    }

                    if (0 == read_reg(temp_addr + Ucm_PtpOc_ParentDs7Reg, &temp_data))
                    {
                        ntlts->ptpOc.ParentDsGmShortId = temp_data;
                    }
                    else
                    {
                        ntlts->ptpOc.ParentDsGmShortId = 0;
                    }

                    if (0 == read_reg(temp_addr + Ucm_PtpOc_ParentDs8Reg, &temp_data))
                    {
                        ntlts->ptpOc.ParentDsGmInaccuracy = temp_data;
                    }
                    else
                    {
                        ntlts->ptpOc.ParentDsGmInaccuracy = 0;
                    }

                    if (0 == read_reg(temp_addr + Ucm_PtpOc_ParentDs9Reg, &temp_data))
                    {
                        ntlts->ptpOc.ParentDsNwInaccuracy = temp_data;
                    }
                    else
                    {
                        ntlts->ptpOc.ParentDsNwInaccuracy = 0;
                    }

                    break;
                }
                else if (i == 9)
                {
                    // cout << "ERROR: " << "read did not complete" << endl;
                    //        ntlts->ptpOc.ParentDsParentClockIdValue->setText("NA");
                    //        ntlts->ptpOc.ParentDsGmClockIdValue->setText("NA");
                    //        ntlts->ptpOc.ParentDsGmPriority1Value->setText("NA");
                    //        ntlts->ptpOc.ParentDsGmPriority2Value->setText("NA");
                    //        ntlts->ptpOc.ParentDsGmAccuracyValue->setText("NA");
                    //        ntlts->ptpOc.ParentDsGmClassValue->setText("NA");
                    //        ntlts->ptpOc.ParentDsGmShortIdValue->setText("NA");
                    //        ntlts->ptpOc.ParentDsGmInaccuracyValue->setText("NA");
                    //        ntlts->ptpOc.ParentDsNwInaccuracyValue->setText("NA");
                }
            }
            else
            {
                //      ntlts->ptpOc.ParentDsParentClockIdValue->setText("NA");
                //      ntlts->ptpOc.ParentDsGmClockIdValue->setText("NA");
                //      ntlts->ptpOc.ParentDsGmPriority1Value->setText("NA");
                //      ntlts->ptpOc.ParentDsGmPriority2Value->setText("NA");
                //      ntlts->ptpOc.ParentDsGmAccuracyValue->setText("NA");
                //      ntlts->ptpOc.ParentDsGmClassValue->setText("NA");
                //      ntlts->ptpOc.ParentDsGmShortIdValue->setText("NA");
                //      ntlts->ptpOc.ParentDsGmInaccuracyValue->setText("NA");
                //      ntlts->ptpOc.ParentDsNwInaccuracyValue->setText("NA");
            }
        }
    }
    else
    {
        // ntlts->ptpOc.ParentDsParentClockIdValue->setText("NA");
        // ntlts->ptpOc.ParentDsGmClockIdValue->setText("NA");
        // ntlts->ptpOc.ParentDsGmPriority1Value->setText("NA");
        // ntlts->ptpOc.ParentDsGmPriority2Value->setText("NA");
        // ntlts->ptpOc.ParentDsGmAccuracyValue->setText("NA");
        // ntlts->ptpOc.ParentDsGmClassValue->setText("NA");
        // ntlts->ptpOc.ParentDsGmShortIdValue->setText("NA");
        // ntlts->ptpOc.ParentDsGmInaccuracyValue->setText("NA");
        // ntlts->ptpOc.ParentDsNwInaccuracyValue->setText("NA");
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

                    if (0 == read_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs1Reg, &temp_data))
                    {
                        ntlts->ptpOc.TimePropertiesDsTimeSource = ((temp_data >> 0) & 0x000000FF);
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

                        ntlts->ptpOc.TimePropertiesDsUtcOffset = (int16_t)((temp_data >> 16) & 0x0000FFFF);
                    }
                    else
                    {
                        ntlts->ptpOc.TimePropertiesDsTimeSource = 0;
                        ntlts->ptpOc.TimePropertiesDsPtpTimescale = 0;
                        ntlts->ptpOc.TimePropertiesDsFreqTraceable = 0;
                        ntlts->ptpOc.TimePropertiesDsTimeTraceable = 0;
                        ntlts->ptpOc.TimePropertiesDsLeap59 = 0;
                        ntlts->ptpOc.TimePropertiesDsLeap61 = 0;
                        ntlts->ptpOc.TimePropertiesDsUtcOffsetVal = 0;
                        ntlts->ptpOc.TimePropertiesDsUtcOffset = 0;
                    }

                    if (0 == read_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs2Reg, &temp_data))
                    {
                        ntlts->ptpOc.TimePropertiesDsCurrentOffset = (int32_t)temp_data;
                    }
                    else
                    {
                        ntlts->ptpOc.TimePropertiesDsCurrentOffset = 0;
                    }

                    if (0 == read_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs3Reg, &temp_data))
                    {
                        ntlts->ptpOc.TimePropertiesDsJumpSeconds = (int32_t)temp_data;
                    }
                    else
                    {
                        ntlts->ptpOc.TimePropertiesDsJumpSeconds = 0;
                    }

                    if (0 == read_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs4Reg, &temp_data))
                    {
                        temp_next_jump = temp_data;
                        temp_next_jump = temp_next_jump << 32;
                        if (0 == read_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs5Reg, &temp_data))
                        {
                            temp_next_jump |= temp_data;
                            ntlts->ptpOc.TimePropertiesDsNextJump = temp_next_jump;
                        }
                        else
                        {
                            ntlts->ptpOc.TimePropertiesDsNextJump = 0;
                        }
                    }
                    else
                    {
                        ntlts->ptpOc.TimePropertiesDsNextJump = 0;
                    }

                    memset(temp_string, 0, STRING_SIZE);
                    temp_length = 0;
                    if (0 == read_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs6Reg, &temp_data))
                    {
                        temp_length = (uint8_t)(temp_data & 0x000000FF);
                        if (temp_length > 16)
                            temp_length = 16;

                        for (int j = 0; j < 3; j++)
                        {
                            if (0 == read_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs7Reg + (j * 4), &temp_data))
                            {
                                temp_string[j * 4 + 0] = (uint8_t)((temp_data >> 0) & 0x000000FF);
                                temp_string[j * 4 + 1] = (uint8_t)((temp_data >> 8) & 0x000000FF);
                                temp_string[j * 4 + 2] = (uint8_t)((temp_data >> 16) & 0x000000FF);
                                temp_string[j * 4 + 3] = (uint8_t)((temp_data >> 24) & 0x000000FF);
                            }
                            else
                            {
                                memset(temp_string, 0, STRING_SIZE);
                                snprintf(ntlts->ptpOc.TimePropertiesDsDisplayName, sizeof(ntlts->ptpOc.TimePropertiesDsDisplayName), "NA");
                                break;
                            }
                        }
                        temp_string[temp_length] = '\0';
                        snprintf(ntlts->ptpOc.TimePropertiesDsDisplayName, sizeof(ntlts->ptpOc.TimePropertiesDsDisplayName), "%s", temp_string);
                    }
                    else
                    {
                        snprintf(ntlts->ptpOc.TimePropertiesDsDisplayName, sizeof(ntlts->ptpOc.TimePropertiesDsDisplayName), "NA");
                    }
                    break;
                }
                else if (i == 9)
                {
                }
            }
            else
            {
            }
        }
    }
    else
    {
    }

    // version
    if (0 == read_reg(temp_addr + Ucm_PtpOc_VersionReg, &temp_data))
    {
        ntlts->ptpOc.Version = temp_data;
    }
    else
    {
        ntlts->ptpOc.Version = 0;
    }

    return 0;
}

int8_t ptp_oc_write_values(NTL_TS_T *ntlts, uint8_t fromRegisters)
{
    uint32_t temp_data = 0;
    uint32_t temp_addr = 0;

    // uint8_t temp_string[STRING_SIZE] = {0};

    temp_addr = ntlts->ptpRegs.StartAddr;

    // profile and layer
    // temp_string = ui->PtpOcProfileValue->currentText();

    if (strncmp(ntlts->ptpOc.Profile, "Default", strlen("Default")) == 0)
    {
        temp_data = 0x00000000;
    }
    else if (strncmp(ntlts->ptpOc.Profile, "Power", strlen("Power")) == 0)
    {
        temp_data = 0x00000001;
    }
    else if (strncmp(ntlts->ptpOc.Profile, "Utility", strlen("Utility")) == 0)
    {
        temp_data = 0x00000002;
    }
    else if (strncmp(ntlts->ptpOc.Profile, "TSN", strlen("TSN")) == 0)
    {
        temp_data = 0x00000003;
    }
    else if (strncmp(ntlts->ptpOc.Profile, "ITUG8265.1", strlen("ITUG8265.1")) == 0)
    {
        temp_data = 0x00000004;
    }
    else if (strncmp(ntlts->ptpOc.Profile, "ITUG8275.1", strlen("ITUG8275.1")) == 0)
    {
        temp_data = 0x00000005;
    }
    else if (strncmp(ntlts->ptpOc.Profile, "ITUG8275.2", strlen("ITUG8275.2")) == 0)
    {
        temp_data = 0x00000006;
    }
    else
    {
        temp_data = 0x00000000;
    }

    if (false)
    // if (temp_string == "NA")
    {
        // nothing
    }
    else
    {
        if (ntlts->ptpOc.DefaultDsTwoStep)
        {
            temp_data |= 0x00000100;
        }

        if (ntlts->ptpOc.DefaultDsSignaling)
        {
            temp_data |= 0x00000200;
        }

        if (ntlts->ptpOc.DefaultDsSlaveOnly)
        {
            temp_data |= 0x00100000;
        }
        if (ntlts->ptpOc.DefaultDsMasterOnly)
        {
            temp_data |= 0x00200000;
        }

        // if (ntlts->ptpOc.DefaultDsDisableOffsetCor)
        // {
        //     temp_data |= 0x00400000;
        // }
        //
        // if (ntlts->ptpOc.DefaultDsListedUnicastSlavesOnly)
        // {
        //     temp_data |= 0x00800000;
        // }

        // temp_string = ui->PtpOcLayerValue->currentText();
        if (strncmp(ntlts->ptpOc.Layer, "Layer 2", strlen("Layer 2")) == 0)
        {
            temp_data |= 0x00000000;
        }
        else if (strncmp(ntlts->ptpOc.Layer, "Layer 3v4", strlen("Layer 3v4")) == 0)
        {
            temp_data |= 0x00010000;
        }
        else if (strncmp(ntlts->ptpOc.Layer, "Layer 3v6", strlen("Layer 3v6")) == 0)
        {
            temp_data |= 0x00020000;
        }
        else

        {
            temp_data |= 0x00000000;

            // return -8; return not needed?
        }

        if (false)
        {
            // nothing
        }
        else
        {
            // temp_string = ui->PtpOcDelayMechanismValue->currentText();
            // if (temp_string == "P2P")
            if (strncmp(ntlts->ptpOc.Layer, "P2P", strlen("P2P")) == 0)

            {
                temp_data |= 0x00000000;
            }
            // else if (temp_string == "E2E")
            else if (strncmp(ntlts->ptpOc.Layer, "E2E", strlen("E2E")) == 0)

            {
                temp_data |= 0x01000000;
            }
            // else if (temp_string == "E2E Unicast")
            else if (strncmp(ntlts->ptpOc.Layer, "E2E Unicast", strlen("E2E Unicast")) == 0)

            {
                temp_data |= 0x03000000;
            }
            else
            {
                temp_data |= 0x00000000;
            }

            ///

            if (fromRegisters)
            {
                temp_data = ntlts->ptpRegs.ConfigProfileReg;
            }

            // if (temp_string == "NA")
            if (false)
            {
                // nothing
            }
            else if (0 == write_reg(temp_addr + Ucm_PtpOc_ConfigProfileReg, &temp_data))
            {
                //    switch (temp_data & 0x00000007)
                //    {
                //    case 0:
                //        ui->PtpOcProfileValue->setCurrentText("Default");
                //        break;
                //    case 1:
                //        ui->PtpOcProfileValue->setCurrentText("Power");
                //        break;
                //    case 2:
                //        ui->PtpOcProfileValue->setCurrentText("Utility");
                //        break;
                //    case 3:
                //        ui->PtpOcProfileValue->setCurrentText("TSN");
                //        break;
                //    case 4:
                //        ui->PtpOcProfileValue->setCurrentText("ITUG8265.1");
                //        break;
                //    case 5:
                //        ui->PtpOcProfileValue->setCurrentText("ITUG8275.1");
                //        break;
                //    case 6:
                //        ui->PtpOcProfileValue->setCurrentText("ITUG8275.2");
                //        break;
                //    default:
                //        ui->PtpOcProfileValue->setCurrentText("NA");
                //        break;
                //    }
                //
                //    switch ((temp_data >> 8) & 0x00000001)
                //    {
                //    case 0:
                //        ui->PtpOcDefaultDsTwoStepCheckBox->setChecked(false);
                //        break;
                //    case 1:
                //        ui->PtpOcDefaultDsTwoStepCheckBox->setChecked(true);
                //        break;
                //    default:
                //        ui->PtpOcDefaultDsTwoStepCheckBox->setChecked(false);
                //        break;
                //    }
                //
                //    switch ((temp_data >> 9) & 0x00000001)
                //    {
                //    case 0:
                //        ui->PtpOcDefaultDsSignalingCheckBox->setChecked(false);
                //        break;
                //    case 1:
                //        ui->PtpOcDefaultDsSignalingCheckBox->setChecked(true);
                //        break;
                //    default:
                //        ui->PtpOcDefaultDsSignalingCheckBox->setChecked(false);
                //        break;
                //    }
                //
                switch ((temp_data >> 16) & 0x00000003)
                {
                case 0:
                    // ui->PtpOcLayerValue->setCurrentText("Layer 2");
                    snprintf(ntlts->ptpOc.Layer, sizeof(ntlts->ptpOc.Layer), "Layer 2");
                    break;
                case 1:
                    // ui->PtpOcLayerValue->setCurrentText("Layer 3v4");
                    snprintf(ntlts->ptpOc.Layer, sizeof(ntlts->ptpOc.Layer), "Layer 3v4");

                    break;
                case 2:
                    // ui->PtpOcLayerValue->setCurrentText("Layer 3v6");
                    snprintf(ntlts->ptpOc.Layer, sizeof(ntlts->ptpOc.Layer), "Layer 3v6");

                    break;
                default:
                    // ui->PtpOcLayerValue->setCurrentText("NA");
                    snprintf(ntlts->ptpOc.Layer, sizeof(ntlts->ptpOc.Layer), "NA");

                    break;
                }
                //
                //    switch ((temp_data >> 20) & 0x00000003)
                //    {
                //    case 0:
                //        ui->PtpOcDefaultDsSlaveOnlyCheckBox->setChecked(false);
                //        ui->PtpOcDefaultDsMasterOnlyCheckBox->setChecked(false);
                //        break;
                //    case 1:
                //        ui->PtpOcDefaultDsSlaveOnlyCheckBox->setChecked(true);
                //        ui->PtpOcDefaultDsMasterOnlyCheckBox->setChecked(false);
                //        break;
                //    case 2:
                //        ui->PtpOcDefaultDsSlaveOnlyCheckBox->setChecked(false);
                //        ui->PtpOcDefaultDsMasterOnlyCheckBox->setChecked(true);
                //        break;
                //    default:
                //        ui->PtpOcDefaultDsSlaveOnlyCheckBox->setChecked(false);
                //        ui->PtpOcDefaultDsMasterOnlyCheckBox->setChecked(false);
                //        break;
                //    }
                //
                //    switch ((temp_data >> 22) & 0x00000001)
                //    {
                //    case 0:
                //        ui->PtpOcDefaultDsDisableOffsetCorCheckBox->setChecked(false);
                //        break;
                //    case 1:
                //        ui->PtpOcDefaultDsDisableOffsetCorCheckBox->setChecked(true);
                //        break;
                //    default:
                //        ui->PtpOcDefaultDsDisableOffsetCorCheckBox->setChecked(false);
                //        break;
                //    }
                //
                //    switch ((temp_data >> 23) & 0x00000001)
                //    {
                //    case 0:
                //        ui->PtpOcDefaultDsListedUnicastSlavesOnlyCheckBox->setChecked(false);
                //        break;
                //    case 1:
                //        ui->PtpOcDefaultDsListedUnicastSlavesOnlyCheckBox->setChecked(true);
                //        break;
                //    default:
                //        ui->PtpOcDefaultDsListedUnicastSlavesOnlyCheckBox->setChecked(false);
                //        break;
                //    }
                //
                //    switch ((temp_data >> 24) & 0x00000001)
                //    {
                //    case 0:
                //        ui->PtpOcDelayMechanismValue->setCurrentText("P2P");
                //        break;
                //    case 1:
                //        if ((temp_data & 0x02000000) == 0)
                //        {
                //            ui->PtpOcDelayMechanismValue->setCurrentText("E2E");
                //        }
                //        else
                //        {
                //            ui->PtpOcDelayMechanismValue->setCurrentText("E2E Unicast");
                //        }
                //        break;
                //    default:
                //        ui->PtpOcDelayMechanismValue->setCurrentText("NA");
                //        break;
                //    }
                //
                temp_data = 0x00000001; // write
                if (0 == write_reg(temp_addr + Ucm_PtpOc_ConfigControlReg, &temp_data))
                {
                    // nothing
                }
                else
                {
                    return -1;
                    // ui->PtpOcProfileValue->setCurrentText("NA");
                    // ui->PtpOcDefaultDsTwoStepCheckBox->setChecked(false);
                    // ui->PtpOcDefaultDsSignalingCheckBox->setChecked(false);
                    // ui->PtpOcLayerValue->setCurrentText("NA");
                    // ui->PtpOcDelayMechanismValue->setCurrentText("NA");
                    // ui->PtpOcDefaultDsSlaveOnlyCheckBox->setChecked(false);
                    // ui->PtpOcDefaultDsMasterOnlyCheckBox->setChecked(false);
                    // ui->PtpOcDefaultDsListedUnicastSlavesOnlyCheckBox->setChecked(false);
                    // ui->PtpOcDefaultDsDisableOffsetCorCheckBox->setChecked(false);
                }
            }
            else
            {
                return -2;
                // ui->PtpOcProfileValue->setCurrentText("NA");
                // ui->PtpOcDefaultDsTwoStepCheckBox->setChecked(false);
                // ui->PtpOcDefaultDsSignalingCheckBox->setChecked(false);
                // ui->PtpOcLayerValue->setCurrentText("NA");
                // ui->PtpOcDelayMechanismValue->setCurrentText("NA");
                // ui->PtpOcDefaultDsSlaveOnlyCheckBox->setChecked(false);
                // ui->PtpOcDefaultDsMasterOnlyCheckBox->setChecked(false);
                // ui->PtpOcDefaultDsListedUnicastSlavesOnlyCheckBox->setChecked(false);
                // ui->PtpOcDefaultDsDisableOffsetCorCheckBox->setChecked(false);
            }
        }
    }

    //// lucky packet filter
    // temp_string = ui->PtpOcDefaultDsLpFilterWindowValue->text();
    // temp_data = temp_string.toUInt(nullptr, 10);
    // temp_data &= 0x00001FFF;
    // temp_data = temp_data << 16;
    // if (ntlts->ptpOc.DefaultDsLpFilter)
    //{
    //     temp_data |= 0x00000003;
    // }
    // else
    //{
    //     temp_data |= 0x00000002;
    // }
    // if (temp_string == "NA")
    //{
    //     // nothing
    // }
    // else if (0 == write_reg(temp_addr + Ucm_PtpOc_ConfigLuckyPacketFilterReg, &temp_data))
    //{
    //     temp_data = 0x00000010; // write
    //     if (0 == write_reg(temp_addr + Ucm_PtpOc_ConfigControlReg, &temp_data))
    //     {
    //         // nothing
    //     }
    //     else
    //     {
    //         ui->PtpOcDefaultDsLpFilterWindowValue->setText("NA");
    //         ui->PtpOcDefaultDsLpFilterCheckBox->setChecked(false);
    //     }
    // }
    // else
    //{
    //     ui->PtpOcDefaultDsLpFilterWindowValue->setText("NA");
    //     ui->PtpOcDefaultDsLpFilterCheckBox->setChecked(false);
    // }
    //
    //// vlan
    // temp_string = ui->PtpOcVlanValue->text();
    // temp_data = temp_string.toUInt(nullptr, 16);
    // temp_data &= 0x0000FFFF;
    // if (ntlts->ptpOc.VlanEnable)
    //{
    //     temp_data |= 0x00010000; // enable
    // }
    // if (temp_string == "NA")
    //{
    //     // nothing
    // }
    // else if (0 == write_reg(temp_addr + Ucm_PtpOc_ConfigVlanReg, &temp_data))
    //{
    //     temp_data &= 0x0000FFFF;
    //     ui->PtpOcVlanValue->setText(QString("0x%1").arg(temp_data, 4, 16, QLatin1Char('0')));
    //
    //    temp_data = 0x00000002; // write
    //    if (0 == write_reg(temp_addr + Ucm_PtpOc_ConfigControlReg, &temp_data))
    //    {
    //        // nothing
    //    }
    //    else
    //    {
    //        ui->PtpOcVlanEnableCheckBox->setChecked(false);
    //        ui->PtpOcVlanValue->setText("NA");
    //    }
    //}
    // else
    //{
    //    ui->PtpOcVlanEnableCheckBox->setChecked(false);
    //    ui->PtpOcVlanValue->setText("NA");
    //}

    // ip
    // temp_string = ui->PtpOcLayerValue->currentText();
    if (strncmp(ntlts->ptpOc.Layer, "Layer 3v4", strlen("Layer 3v4")) == 0)
    // if (temp_string == "Layer 3v4")
    {
        int32_t temp_ip[4] = {0};

        char temp_string[64] = {0};

        memcpy(temp_string, ntlts->ptpOc.ipAddr, sizeof(temp_string));

        char *token;
        for (int i = 0; i < 4; i++)
        {
            token = (i == 0) ? strtok(temp_string, ".") : strtok(NULL, ".");
            if (token == NULL)
                break;
            temp_ip[i] = strtol(token, NULL, 10);
        }

        temp_data = 0x00000000;
        temp_data |= (temp_ip[3]) & 0x000000FF;
        temp_data = temp_data << 8;
        temp_data |= (temp_ip[2]) & 0x000000FF;
        temp_data = temp_data << 8;
        temp_data |= (temp_ip[1]) & 0x000000FF;
        temp_data = temp_data << 8;
        temp_data |= (temp_ip[0]) & 0x000000FF;

        if (fromRegisters)
        {
            temp_data = ntlts->ptpRegs.ConfigIpReg;
        }

        if (false)
        {
            // nothing
        }
        else if (0 == write_reg(temp_addr + Ucm_PtpOc_ConfigIpReg, &temp_data))
        {
            temp_data = 0x00000004; // write
            if (0 == write_reg(temp_addr + Ucm_PtpOc_ConfigControlReg, &temp_data))
            {
                // ui->PtpOcIpValue->setText(temp_string);
            }
            else
            {
                // ui->PtpOcIpValue->setText("NA");
                return -4;
            }
        }
        else
        {
            // ui->PtpOcIpValue->setText("NA");
            return -5;
        }
    }
    else if (strncmp(ntlts->ptpOc.Layer, "Layer 3v6", strlen("Layer 3v6")) == 0)
    {
        // temp_string = ui->PtpOcIpValue->text();
        // temp_ip6 = QHostAddress(temp_string).toIPv6Address();

        long temp_ip6[16] = {0};
        char temp_string[64] = {0};

        memcpy(temp_string, ntlts->ptpOc.ipAddr, sizeof(temp_string));
        char *token;
        char *err;
        char byte[3];
        for (int i = 0; i < 32; i += 2)
        {

            token = (i == 0) ? strtok(temp_string, ":") : strtok(NULL, ":");
            if (token == NULL)
                break;

            // Extract first two characters
            strncpy(byte, token, 2);
            byte[2] = '\0';
            temp_ip6[i] = strtol(byte, &err, 16);
            if (err == token || *err != '\0')
            {
                return -1;
            }

            // Extract next two characters
            strncpy(byte, token + 2, 2);
            byte[2] = '\0';
            temp_ip6[i + 1] = strtol(byte, &err, 16);
            if (err == token || *err != '\0')
            {
                return -1;
            }
        }

        temp_data = 0x00000000;
        temp_data |= temp_ip6[3] & 0x000000FF;
        temp_data = temp_data << 8;
        temp_data |= temp_ip6[2] & 0x000000FF;
        temp_data = temp_data << 8;
        temp_data |= temp_ip6[1] & 0x000000FF;
        temp_data = temp_data << 8;
        temp_data |= temp_ip6[0] & 0x000000FF;

        if (fromRegisters)
        {
            temp_data = ntlts->ptpRegs.ConfigIpReg;
        }

        if (strncmp(ntlts->ptpOc.ipAddr, "NA", strlen("NA")) == 0)
        {
            // nothing
        }
        else if (0 == write_reg(temp_addr + Ucm_PtpOc_ConfigIpReg, &temp_data))
        {
            temp_data = 0x00000000;
            temp_data |= temp_ip6[7] & 0x000000FF;
            temp_data = temp_data << 8;
            temp_data |= temp_ip6[6] & 0x000000FF;
            temp_data = temp_data << 8;
            temp_data |= temp_ip6[5] & 0x000000FF;
            temp_data = temp_data << 8;
            temp_data |= temp_ip6[4] & 0x000000FF;

            if (fromRegisters)
            {
                temp_data = ntlts->ptpRegs.ConfigIpv61Reg;
            }

            if (strncmp(ntlts->ptpOc.ipAddr, "NA", strlen("NA")) == 0)
            {
                // nothing
            }
            else if (0 == write_reg(temp_addr + Ucm_PtpOc_ConfigIpv61Reg, &temp_data))
            {
                temp_data = 0x00000000;
                temp_data |= temp_ip6[11] & 0x000000FF;
                temp_data = temp_data << 8;
                temp_data |= temp_ip6[10] & 0x000000FF;
                temp_data = temp_data << 8;
                temp_data |= temp_ip6[9] & 0x000000FF;
                temp_data = temp_data << 8;
                temp_data |= temp_ip6[8] & 0x000000FF;

                if (fromRegisters)
                {
                    temp_data = ntlts->ptpRegs.ConfigIpv62Reg;
                }

                if (strncmp(ntlts->ptpOc.ipAddr, "NA", strlen("NA")) == 0)
                {
                    // nothing
                }
                else if (0 == write_reg(temp_addr + Ucm_PtpOc_ConfigIpv62Reg, &temp_data))
                {
                    temp_data = 0x00000000;
                    temp_data |= temp_ip6[15] & 0x000000FF;
                    temp_data = temp_data << 8;
                    temp_data |= temp_ip6[14] & 0x000000FF;
                    temp_data = temp_data << 8;
                    temp_data |= temp_ip6[13] & 0x000000FF;
                    temp_data = temp_data << 8;
                    temp_data |= temp_ip6[12] & 0x000000FF;

                    if (fromRegisters)
                    {
                        temp_data = ntlts->ptpRegs.ConfigIpv63Reg;
                    }

                    if (strncmp(ntlts->ptpOc.ipAddr, "NA", strlen("NA")) == 0)
                    {
                        // nothing
                    }
                    else if (0 == write_reg(temp_addr + Ucm_PtpOc_ConfigIpv63Reg, &temp_data))
                    {
                        temp_data = 0x00000004; // write
                        if (0 == write_reg(temp_addr + Ucm_PtpOc_ConfigControlReg, &temp_data))
                        {
                            // ui->PtpOcIpValue->setText(temp_string);
                        }
                        else
                        {
                            // ui->PtpOcIpValue->setText("NA");
                        }
                    }
                    else
                    {
                        // ui->PtpOcIpValue->setText("NA");
                    }
                }
                else
                {
                    // ui->PtpOcIpValue->setText("NA");
                }
            }
            else
            {
                // ui->PtpOcIpValue->setText("NA");
            }
        }
        else
        {
            // ui->PtpOcIpValue->setText("NA");
        }
    }
    else
    {
        // ui->PtpOcIpValue->setText("NA");
    }
    /*
        //********************************
        // default dataset
        //********************************
        // clock id
        temp_string = ui->PtpOcDefaultDsClockIdValue->text();
        temp_string.remove(QChar(':'), Qt::CaseInsensitive);
        temp_mac = temp_string.toULongLong(nullptr, 16);
        temp_data = 0x00000000;
        temp_data |= (temp_mac >> 32) & 0x000000FF;
        temp_data = temp_data << 8;
        temp_data |= (temp_mac >> 40) & 0x000000FF;
        temp_data = temp_data << 8;
        temp_data |= (temp_mac >> 48) & 0x000000FF;
        temp_data = temp_data << 8;
        temp_data |= (temp_mac >> 56) & 0x000000FF;

        if (temp_string == "NA")
        {
            // nothing
        }
        else if (0 == write_reg(temp_addr + Ucm_PtpOc_DefaultDs1Reg, &temp_data))
        {
            temp_string.clear();
            temp_string.append(QString("%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));
            temp_string.append(":");
            temp_string.append(QString("%1").arg(((temp_data >> 8) & 0x000000FF), 2, 16, QLatin1Char('0')));
            temp_string.append(":");
            temp_string.append(QString("%1").arg(((temp_data >> 16) & 0x000000FF), 2, 16, QLatin1Char('0')));
            temp_string.append(":");
            temp_string.append(QString("%1").arg(((temp_data >> 24) & 0x000000FF), 2, 16, QLatin1Char('0')));
            temp_string.append(":");

            temp_data = 0x00000000;
            temp_data |= (temp_mac >> 0) & 0x000000FF;
            temp_data = temp_data << 8;
            temp_data |= (temp_mac >> 8) & 0x000000FF;
            temp_data = temp_data << 8;
            temp_data |= (temp_mac >> 16) & 0x000000FF;
            temp_data = temp_data << 8;
            temp_data |= (temp_mac >> 24) & 0x000000FF;
            if (0 == write_reg(temp_addr + Ucm_PtpOc_DefaultDs2Reg, &temp_data))
            {
                temp_string.append(QString("%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));
                temp_string.append(":");
                temp_string.append(QString("%1").arg(((temp_data >> 8) & 0x000000FF), 2, 16, QLatin1Char('0')));
                temp_string.append(":");
                temp_string.append(QString("%1").arg(((temp_data >> 16) & 0x000000FF), 2, 16, QLatin1Char('0')));
                temp_string.append(":");
                temp_string.append(QString("%1").arg(((temp_data >> 24) & 0x000000FF), 2, 16, QLatin1Char('0')));

                temp_data = 0x00000001; // write
                if (0 == write_reg(temp_addr + Ucm_PtpOc_DefaultDsControlReg, &temp_data))
                {
                    ui->PtpOcDefaultDsClockIdValue->setText(temp_string);
                }
                else
                {
                    ui->PtpOcDefaultDsClockIdValue->setText("NA");
                }
            }
            else
            {
                ui->PtpOcDefaultDsClockIdValue->setText("NA");
            }
        }
        else
        {
            ui->PtpOcDefaultDsClockIdValue->setText("NA");
        }

        // domain
        temp_string = ui->PtpOcDefaultDsDomainValue->text();
        temp_data = temp_string.toUInt(nullptr, 16);
        if (temp_string == "NA")
        {
            // nothing
        }
        else if (0 == write_reg(temp_addr + Ucm_PtpOc_DefaultDs3Reg, &temp_data))
        {
            temp_data &= 0x000000FF;
            ui->PtpOcDefaultDsDomainValue->setText(QString("0x%1").arg(temp_data, 2, 16, QLatin1Char('0')));

            temp_data = 0x00000002; // write
            if (0 == write_reg(temp_addr + Ucm_PtpOc_DefaultDsControlReg, &temp_data))
            {
                // nothing
            }
            else
            {
                ui->PtpOcDefaultDsDomainValue->setText("NA");
            }
        }
        else
        {
            ui->PtpOcDefaultDsDomainValue->setText("NA");
        }

        // priority 1
        temp_string = ui->PtpOcDefaultDsPriority1Value->text();
        temp_data = temp_string.toUInt(nullptr, 16);
        temp_data = temp_data << 24;
        if (temp_string == "NA")
        {
            // nothing
        }
        else if (0 == write_reg(temp_addr + Ucm_PtpOc_DefaultDs3Reg, &temp_data))
        {
            ui->PtpOcDefaultDsPriority1Value->setText(QString("0x%1").arg(((temp_data >> 24) & 0x000000FF), 2, 16, QLatin1Char('0')));

            temp_data = 0x00000008; // write
            if (0 == write_reg(temp_addr + Ucm_PtpOc_DefaultDsControlReg, &temp_data))
            {
                // nothing
            }
            else
            {
                ui->PtpOcDefaultDsPriority1Value->setText("NA");
            }
        }
        else
        {
            ui->PtpOcDefaultDsPriority1Value->setText("NA");
        }

        // priority 2
        temp_string = ui->PtpOcDefaultDsPriority2Value->text();
        temp_data = temp_string.toUInt(nullptr, 16);
        temp_data = temp_data << 16;
        if (temp_string == "NA")
        {
            // nothing
        }
        else if (0 == write_reg(temp_addr + Ucm_PtpOc_DefaultDs3Reg, &temp_data))
        {
            ui->PtpOcDefaultDsPriority2Value->setText(QString("0x%1").arg(((temp_data >> 16) & 0x000000FF), 2, 16, QLatin1Char('0')));

            temp_data = 0x00000010; // write
            if (0 == write_reg(temp_addr + Ucm_PtpOc_DefaultDsControlReg, &temp_data))
            {
                // nothing
            }
            else
            {
                ui->PtpOcDefaultDsPriority2Value->setText("NA");
            }
        }
        else
        {
            ui->PtpOcDefaultDsPriority2Value->setText("NA");
        }

        // accuracy, class, variance
        temp_string = ui->PtpOcDefaultDsClassValue->text();
        temp_data = temp_string.toUInt(nullptr, 16);
        temp_data = temp_data << 8;
        if (temp_string == "NA")
        {
            // nothing
        }
        else
        {
            temp_string = ui->PtpOcDefaultDsAccuracyValue->text();
            temp_data |= temp_string.toUInt(nullptr, 10);
            temp_data = temp_data << 16;
            if (temp_string == "NA")
            {
                // nothing
            }
            else
            {
                temp_string = ui->PtpOcDefaultDsVarianceValue->text();
                temp_data |= temp_string.toUInt(nullptr, 16);
                if (temp_string == "NA")
                {
                    // nothing
                }
                else if (0 == write_reg(temp_addr + Ucm_PtpOc_DefaultDs4Reg, &temp_data))
                {
                    temp_data = 0x00000004; // write
                    if (0 == write_reg(temp_addr + Ucm_PtpOc_DefaultDsControlReg, &temp_data))
                    {
                        // nothing
                    }
                    else
                    {
                        ui->PtpOcDefaultDsVarianceValue->setText("NA");
                        ui->PtpOcDefaultDsAccuracyValue->setText("NA");
                        ui->PtpOcDefaultDsClassValue->setText("NA");
                    }
                }
                else
                {
                    ui->PtpOcDefaultDsVarianceValue->setText("NA");
                    ui->PtpOcDefaultDsAccuracyValue->setText("NA");
                    ui->PtpOcDefaultDsClassValue->setText("NA");
                }
            }
        }

        // short id
        temp_string = ui->PtpOcDefaultDsShortIdValue->text();
        temp_data = temp_string.toUInt(nullptr, 16);
        if (temp_string == "NA")
        {
            // nothing
        }
        else if (0 == write_reg(temp_addr + Ucm_PtpOc_DefaultDs5Reg, &temp_data))
        {
            ui->PtpOcDefaultDsShortIdValue->setText(QString("0x%1").arg(temp_data, 4, 16, QLatin1Char('0')));

            temp_data = 0x00000020; // write
            if (0 == write_reg(temp_addr + Ucm_PtpOc_DefaultDsControlReg, &temp_data))
            {
                // nothing
            }
            else
            {
                ui->PtpOcDefaultDsShortIdValue->setText("NA");
            }
        }
        else
        {
            ui->PtpOcDefaultDsShortIdValue->setText("NA");
        }

        // inaccuracy
        temp_string = ui->PtpOcDefaultDsInaccuracyValue->text();
        temp_data = temp_string.toUInt(nullptr, 10);
        if (temp_string == "NA")
        {
            // nothing
        }
        else if (0 == write_reg(temp_addr + Ucm_PtpOc_DefaultDs6Reg, &temp_data))
        {
            ui->PtpOcDefaultDsInaccuracyValue->setText(QString::number(temp_data));

            temp_data = 0x00000040; // write
            if (0 == write_reg(temp_addr + Ucm_PtpOc_DefaultDsControlReg, &temp_data))
            {
                // nothing
            }
            else
            {
                ui->PtpOcDefaultDsInaccuracyValue->setText("NA");
            }
        }
        else
        {
            ui->PtpOcDefaultDsInaccuracyValue->setText("NA");
        }

        //********************************
        // port dataset
        //********************************
        // delay mechanism
        temp_data = 0x00000001;
        if (0 == write_reg(temp_addr + Ucm_PtpOc_PortDsControlReg, &temp_data))
        {
            // nothing
        }
        else
        {
            ui->PtpOcDelayMechanismValue->setCurrentText("NA");
        }

        if (ntlts->ptpOc.PortDsSetCustomIntervals)
        {
            // delay message intervals
            temp_string = ui->PtpOcPortDsDelayReceiptTimeoutValue->text();
            temp_data = (temp_string.toUInt(nullptr, 10) & 0x000000FF);
            temp_data = temp_data << 8;
            temp_string = ui->PtpOcPortDsDelayReqLogMsgIntervalValue->text();
            temp_data |= (temp_string.toInt(nullptr, 10) & 0x000000FF);
            temp_data = temp_data << 8;
            temp_string = ui->PtpOcPortDsPDelayReqLogMsgIntervalValue->text();
            temp_data |= (temp_string.toInt(nullptr, 10) & 0x000000FF);
            if (temp_string == "NA")
            {
                // nothing
            }
            else if (0 == write_reg(temp_addr + Ucm_PtpOc_PortDs4Reg, &temp_data))
            {
                // announce message intervals
                temp_string = ui->PtpOcPortDsAnnounceReceiptTimeoutValue->text();
                temp_data = (temp_string.toUInt(nullptr, 10) & 0x000000FF);
                temp_data = temp_data << 8;
                temp_string = ui->PtpOcPortDsAnnounceLogMsgIntervalValue->text();
                temp_data |= (temp_string.toInt(nullptr, 10) & 0x000000FF);
                if (temp_string == "NA")
                {
                    // nothing
                }
                else if (0 == write_reg(temp_addr + Ucm_PtpOc_PortDs5Reg, &temp_data))
                {
                    // sync message interval
                    temp_string = ui->PtpOcPortDsSyncReceiptTimeoutValue->text();
                    temp_data = (temp_string.toUInt(nullptr, 10) & 0x000000FF);
                    temp_data = temp_data << 8;
                    temp_string = ui->PtpOcPortDsSyncLogMsgIntervalValue->text();
                    temp_data |= (temp_string.toInt(nullptr, 10) & 0x000000FF);
                    if (temp_string == "NA")
                    {
                        // nothing
                    }
                    else if (0 == write_reg(temp_addr + Ucm_PtpOc_PortDs6Reg, &temp_data))
                    {
                        // set intervals
                        temp_data = 0x00000002;
                        if (0 == write_reg(temp_addr + Ucm_PtpOc_PortDsControlReg, &temp_data))
                        {
                            // nothing
                        }
                        else
                        {
                            ui->PtpOcPortDsPDelayReqLogMsgIntervalValue->setText("NA");
                            ui->PtpOcPortDsDelayReqLogMsgIntervalValue->setText("NA");
                            ui->PtpOcPortDsDelayReceiptTimeoutValue->setText("NA");
                            ui->PtpOcPortDsAnnounceReceiptTimeoutValue->setText("NA");
                            ui->PtpOcPortDsAnnounceLogMsgIntervalValue->setText("NA");
                            ui->PtpOcPortDsSyncLogMsgIntervalValue->setText("NA");
                            ui->PtpOcPortDsSyncReceiptTimeoutValue->setText("NA");
                        }
                    }
                    else
                    {
                        ui->PtpOcPortDsPDelayReqLogMsgIntervalValue->setText("NA");
                        ui->PtpOcPortDsDelayReqLogMsgIntervalValue->setText("NA");
                        ui->PtpOcPortDsDelayReceiptTimeoutValue->setText("NA");
                        ui->PtpOcPortDsAnnounceReceiptTimeoutValue->setText("NA");
                        ui->PtpOcPortDsAnnounceLogMsgIntervalValue->setText("NA");
                        ui->PtpOcPortDsSyncLogMsgIntervalValue->setText("NA");
                        ui->PtpOcPortDsSyncReceiptTimeoutValue->setText("NA");
                    }
                }
                else
                {
                    ui->PtpOcPortDsPDelayReqLogMsgIntervalValue->setText("NA");
                    ui->PtpOcPortDsDelayReqLogMsgIntervalValue->setText("NA");
                    ui->PtpOcPortDsDelayReceiptTimeoutValue->setText("NA");
                    ui->PtpOcPortDsAnnounceReceiptTimeoutValue->setText("NA");
                    ui->PtpOcPortDsAnnounceLogMsgIntervalValue->setText("NA");
                    ui->PtpOcPortDsSyncLogMsgIntervalValue->setText("NA");
                    ui->PtpOcPortDsSyncReceiptTimeoutValue->setText("NA");
                }
            }
            else
            {
                ui->PtpOcPortDsPDelayReqLogMsgIntervalValue->setText("NA");
                ui->PtpOcPortDsDelayReqLogMsgIntervalValue->setText("NA");
                ui->PtpOcPortDsDelayReceiptTimeoutValue->setText("NA");
                ui->PtpOcPortDsAnnounceReceiptTimeoutValue->setText("NA");
                ui->PtpOcPortDsAnnounceLogMsgIntervalValue->setText("NA");
                ui->PtpOcPortDsSyncLogMsgIntervalValue->setText("NA");
                ui->PtpOcPortDsSyncReceiptTimeoutValue->setText("NA");
            }
        }

        // asymmetry
        temp_string = ui->PtpOcPortDsAsymmetryValue->text();
        temp_data = temp_string.toInt(nullptr, 10);
        if (temp_string == "NA")
        {
            // nothing
        }
        else if (0 == write_reg(temp_addr + Ucm_PtpOc_PortDs7Reg, &temp_data))
        {
            // set asymmetry
            temp_data = 0x00000004;
            if (0 == write_reg(temp_addr + Ucm_PtpOc_PortDsControlReg, &temp_data))
            {
                // nothing
            }
            else
            {
                ui->PtpOcPortDsAsymmetryValue->setText("NA");
            }
        }
        else
        {
            ui->PtpOcPortDsAsymmetryValue->setText("NA");
        }

        // max pdelay
        temp_string = ui->PtpOcPortDsMaxPeerDelayValue->text();
        temp_data = temp_string.toUInt(nullptr, 10);
        if (temp_string == "NA")
        {
            // nothing
        }
        else if (0 == write_reg(temp_addr + Ucm_PtpOc_PortDs8Reg, &temp_data))
        {
            // set max pdelay
            temp_data = 0x00000008;
            if (0 == write_reg(temp_addr + Ucm_PtpOc_PortDsControlReg, &temp_data))
            {
                // nothing
            }
            else
            {
                ui->PtpOcPortDsMaxPeerDelayValue->setText("NA");
            }
        }
        else
        {
            ui->PtpOcPortDsMaxPeerDelayValue->setText("NA");
        }

        //********************************
        // current dataset
        //********************************
        // all RO

        //********************************
        // parent dataset
        //********************************
        // all RO

        //********************************
        // time properties dataset
        //********************************
        if (ntlts->ptpOc.TimePropertiesDsSetLocalProperties)
        {
            // single write
            ui->PtpOcTimePropertiesDsSetLocalPropertiesCheckBox->setChecked(false);

            // time source
            temp_string = ui->PtpOcTimePropertiesDsTimeSourceValue->text();
            temp_data = temp_string.toUInt(nullptr, 16);
            if (temp_string == "NA")
            {
                // nothing
            }
            else if (0 == write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs1Reg, &temp_data))
            {
                temp_data &= 0x000000FF;
                ui->PtpOcTimePropertiesDsTimeSourceValue->setText(QString("0x%1").arg(temp_data, 2, 16, QLatin1Char('0')));

                temp_data = 0x00000080; // write
                if (0 == write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDsControlReg, &temp_data))
                {
                    // nothing
                }
                else
                {
                    ui->PtpOcTimePropertiesDsTimeSourceValue->setText("NA");
                }
            }
            else
            {
                ui->PtpOcTimePropertiesDsTimeSourceValue->setText("NA");
            }

            // ptp timescale
            if (ntlts->ptpOc.TimePropertiesDsPtpTimescale)
            {
                temp_data = 0x00000100; // set
            }
            else
            {
                temp_data = 0x00000000; // not set
            }

            if (0 == write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs1Reg, &temp_data))
            {
                if ((temp_data & 0x00000100) != 0)
                {
                    ui->PtpOcTimePropertiesDsPtpTimescaleCheckBox->setChecked(true);
                }
                else
                {
                    ui->PtpOcTimePropertiesDsPtpTimescaleCheckBox->setChecked(false);
                }

                temp_data = 0x00000040; // write
                if (0 == write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDsControlReg, &temp_data))
                {
                    // nothing
                }
                else
                {
                    ui->PtpOcTimePropertiesDsPtpTimescaleCheckBox->setChecked(false);
                }
            }
            else
            {
                ui->PtpOcTimePropertiesDsPtpTimescaleCheckBox->setChecked(false);
            }

            // freq traceable
            if (ntlts->ptpOc.TimePropertiesDsFreqTraceable)
            {
                temp_data = 0x00000200; // set
            }
            else
            {
                temp_data = 0x00000000; // not set
            }

            if (0 == write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs1Reg, &temp_data))
            {
                if ((temp_data & 0x00000200) != 0)
                {
                    ui->PtpOcTimePropertiesDsFreqTraceableCheckBox->setChecked(true);
                }
                else
                {
                    ui->PtpOcTimePropertiesDsFreqTraceableCheckBox->setChecked(false);
                }

                temp_data = 0x00000020; // write
                if (0 == write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDsControlReg, &temp_data))
                {
                    // nothing
                }
                else
                {
                    ui->PtpOcTimePropertiesDsFreqTraceableCheckBox->setChecked(false);
                }
            }
            else
            {
                ui->PtpOcTimePropertiesDsFreqTraceableCheckBox->setChecked(false);
            }

            // time traceable
            if (ntlts->ptpOc.TimePropertiesDsTimeTraceable)
            {
                temp_data = 0x00000400; // set
            }
            else
            {
                temp_data = 0x00000000; // not set
            }

            if (0 == write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs1Reg, &temp_data))
            {
                if ((temp_data & 0x00000400) != 0)
                {
                    ui->PtpOcTimePropertiesDsTimeTraceableCheckBox->setChecked(true);
                }
                else
                {
                    ui->PtpOcTimePropertiesDsTimeTraceableCheckBox->setChecked(false);
                }

                temp_data = 0x00000010; // write
                if (0 == write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDsControlReg, &temp_data))
                {
                    // nothing
                }
                else
                {
                    ui->PtpOcTimePropertiesDsTimeTraceableCheckBox->setChecked(false);
                }
            }
            else
            {
                ui->PtpOcTimePropertiesDsTimeTraceableCheckBox->setChecked(false);
            }

            // leap 61
            if (ntlts->ptpOc.TimePropertiesDsLeap61)
            {
                temp_data = 0x00000800; // set
            }
            else
            {
                temp_data = 0x00000000; // not set
            }

            if (0 == write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs1Reg, &temp_data))
            {
                if ((temp_data & 0x00000800) != 0)
                {
                    ui->PtpOcTimePropertiesDsLeap61CheckBox->setChecked(true);
                }
                else
                {
                    ui->PtpOcTimePropertiesDsLeap61CheckBox->setChecked(false);
                }

                temp_data = 0x00000008; // write
                if (0 == write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDsControlReg, &temp_data))
                {
                    // nothing
                }
                else
                {
                    ui->PtpOcTimePropertiesDsLeap61CheckBox->setChecked(false);
                }
            }
            else
            {
                ui->PtpOcTimePropertiesDsLeap61CheckBox->setChecked(false);
            }

            // leap 59
            if (ntlts->ptpOc.TimePropertiesDsLeap59)
            {
                temp_data = 0x00001000; // set
            }
            else
            {
                temp_data = 0x00000000; // not set
            }

            if (0 == write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs1Reg, &temp_data))
            {
                if ((temp_data & 0x00001000) != 0)
                {
                    ui->PtpOcTimePropertiesDsLeap59CheckBox->setChecked(true);
                }
                else
                {
                    ui->PtpOcTimePropertiesDsLeap59CheckBox->setChecked(false);
                }

                temp_data = 0x00000004; // write
                if (0 == write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDsControlReg, &temp_data))
                {
                    // nothing
                }
                else
                {
                    ui->PtpOcTimePropertiesDsLeap59CheckBox->setChecked(false);
                }
            }
            else
            {
                ui->PtpOcTimePropertiesDsLeap59CheckBox->setChecked(false);
            }

            // utc offset val
            if (ntlts->ptpOc.TimePropertiesDsUtcOffsetVal)
            {
                temp_data = 0x00002000; // set
            }
            else
            {
                temp_data = 0x00000000; // not set
            }

            if (0 == write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs1Reg, &temp_data))
            {
                if ((temp_data & 0x00002000) != 0)
                {
                    ui->PtpOcTimePropertiesDsUtcOffsetValCheckBox->setChecked(true);
                }
                else
                {
                    ui->PtpOcTimePropertiesDsUtcOffsetValCheckBox->setChecked(false);
                }

                temp_data = 0x00000002; // write
                if (0 == write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDsControlReg, &temp_data))
                {
                    // nothing
                }
                else
                {
                    ui->PtpOcTimePropertiesDsUtcOffsetValCheckBox->setChecked(false);
                }
            }
            else
            {
                ui->PtpOcTimePropertiesDsUtcOffsetValCheckBox->setChecked(false);
            }

            // utc offset
            temp_string = ui->PtpOcTimePropertiesDsUtcOffsetValue->text();
            temp_data = temp_string.toInt(nullptr, 10);
            temp_data = temp_data << 16;
            if (temp_string == "NA")
            {
                // nothing
            }
            else if (0 == write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs1Reg, &temp_data))
            {
                ui->PtpOcTimePropertiesDsUtcOffsetValue->setText(QString::number((temp_data >> 16) & 0x0000FFFF));

                temp_data = 0x00000001; // write
                if (0 == write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDsControlReg, &temp_data))
                {
                    // nothing
                }
                else
                {
                    ui->PtpOcTimePropertiesDsUtcOffsetValue->setText("NA");
                }
            }
            else
            {
                ui->PtpOcTimePropertiesDsUtcOffsetValue->setText("NA");
            }

            // current offset
            temp_string = ui->PtpOcTimePropertiesDsCurrentOffsetValue->text();
            temp_data = (unsigned int)temp_string.toInt(nullptr, 10);
            if (temp_string == "NA")
            {
                // nothing
            }
            else if (0 == write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs2Reg, &temp_data))
            {
                ui->PtpOcTimePropertiesDsCurrentOffsetValue->setText(QString::number((int)temp_data));
                temp_data = 0x00000100; // write
                if (0 == write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDsControlReg, &temp_data))
                {
                    // nothing
                }
                else
                {
                    ui->PtpOcTimePropertiesDsCurrentOffsetValue->setText("NA");
                }
            }
            else
            {
                ui->PtpOcTimePropertiesDsCurrentOffsetValue->setText("NA");
            }

            // jump seconds
            temp_string = ui->PtpOcTimePropertiesDsJumpSecondsValue->text();
            temp_data = (unsigned int)temp_string.toInt(nullptr, 10);
            if (temp_string == "NA")
            {
                // nothing
            }
            else if (0 == write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs3Reg, &temp_data))
            {
                ui->PtpOcTimePropertiesDsJumpSecondsValue->setText(QString::number((int)temp_data));
                temp_data = 0x00000200; // write
                if (0 == write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDsControlReg, &temp_data))
                {
                    // nothing
                }
                else
                {
                    ui->PtpOcTimePropertiesDsJumpSecondsValue->setText("NA");
                }
            }
            else
            {
                ui->PtpOcTimePropertiesDsJumpSecondsValue->setText("NA");
            }

            // next jump
            temp_string = ui->PtpOcTimePropertiesDsNextJumpValue->text();
            temp_next_jump = temp_string.toULongLong(nullptr, 10);
            temp_data = temp_next_jump >> 32;
            if (temp_string == "NA")
            {
                // nothing
            }
            else if (0 == write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs4Reg, &temp_data))
            {
                temp_data = temp_next_jump & 0x00000000FFFFFFFF;
                if (0 == write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs5Reg, &temp_data))
                {
                    ui->PtpOcTimePropertiesDsNextJumpValue->setText(QString::number(temp_next_jump));
                    temp_data = 0x00000400; // write
                    if (0 == write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDsControlReg, &temp_data))
                    {
                        // nothing
                    }
                    else
                    {
                        ui->PtpOcTimePropertiesDsNextJumpValue->setText("NA");
                    }
                }
                else
                {
                    ui->PtpOcTimePropertiesDsNextJumpValue->setText("NA");
                }
            }
            else
            {
                ui->PtpOcTimePropertiesDsNextJumpValue->setText("NA");
            }

            // display name
            temp_string = ui->PtpOcTimePropertiesDsDisplayNameValue->text();
            if (temp_string == "NA")
            {
                // nothing
            }
            else
            {
                for (int j = 0; j < 3; j++)
                {
                    temp_data = 0x00000000;
                    if ((j * 4) + 0 < temp_string.size())
                    {
                        temp_data |= ((temp_string.at((j * 4) + 0).toLatin1()) & 0x000000FF) << 0;
                    }

                    if ((j * 4) + 1 < temp_string.size())
                    {
                        temp_data |= ((temp_string.at((j * 4) + 1).toLatin1()) & 0x000000FF) << 8;
                    }

                    if ((j * 4) + 2 < temp_string.size())
                    {
                        temp_data |= ((temp_string.at((j * 4) + 2).toLatin1()) & 0x000000FF) << 16;
                    }

                    if ((j * 4) + 3 < temp_string.size())
                    {
                        temp_data |= ((temp_string.at((j * 4) + 3).toLatin1()) & 0x000000FF) << 24;
                    }

                    if (0 == write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs7Reg + (j * 4), &temp_data))
                    {
                        if (j == 2)
                        {
                            temp_data = temp_string.size(); // length
                            if (0 == write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs6Reg, &temp_data))
                            {
                                temp_data = 0x00001800; // write
                                if (0 == write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDsControlReg, &temp_data))
                                {
                                    ui->PtpOcTimePropertiesDsDisplayNameValue->setText(temp_string);
                                }
                                else
                                {
                                    ui->PtpOcTimePropertiesDsDisplayNameValue->setText("NA");
                                }
                            }
                            else
                            {
                                ui->PtpOcTimePropertiesDsDisplayNameValue->setText("NA");
                            }
                        }
                    }
                    else
                    {
                        ui->PtpOcTimePropertiesDsDisplayNameValue->setText("NA");
                        break;
                    }
                }
            }
        }

        temp_data = 0x00000000; // nothing
        if (ntlts->ptpOc.Enable)
        {
            temp_data |= 0x00000001; // enable
        }
        if (0 == write_reg(temp_addr + Ucm_PtpOc_ControlReg, &temp_data))
        {
            // nothing
        }
        else
        {
            ui->PtpOcEnableCheckBox->setChecked(false);
        }
    */
    return 0;
}
