#include "ntl_uart.h"
// #include "cores.h"
//  #include "uartCircBuff.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"

int8_t ntp_server_read_values(NTL_TS_T *ntlts)
{

    uint32_t temp_data = 0;
    uint32_t temp_addr = 0;
    uint64_t temp_ip;
    uint8_t temp_string[STRING_SIZE] = {0};
    uint8_t temp_ip6[16];
    // char *temp_string;

    // temp_addr = ntlts->ntpServer.address_start;
    temp_addr = ntlts->ntpRegs.StartAddr;
    // temp_addr = 0xB0020000;

    // enabled
    if (0 == read_reg(temp_addr + Ucm_NtpServer_ControlReg, &temp_data))
    {
        ntlts->ntpRegs.ControlReg = temp_data;

        if ((temp_data & 0x00000001) == 0)
        {
            ntlts->ntpServer.Enable = 0;
            // rintf("ntp enable: 0\n");
        }
        else
        {
            ntlts->ntpServer.Enable = 1;
            // printf("ntp enable: 1\n");
        }
    }
    else
    {
        ntlts->ntpServer.Enable = 0;
        // printf("ntp enable: 0\n");
    }

    // mac
    uint8_t temp_mac[6];
    if (0 != read_reg(temp_addr + Ucm_NtpServer_ConfigMac1Reg, &temp_data))
    {
        snprintf(ntlts->ntpServer.MacAddr, sizeof(ntlts->ntpServer.MacAddr), "%s", "NA");

        return -1;
    }

    ntlts->ntpRegs.ConfigMac1Reg = temp_data;

    temp_mac[0] = ((temp_data >> 0) & 0x000000FF);
    temp_mac[1] = ((temp_data >> 8) & 0x000000FF);
    temp_mac[2] = ((temp_data >> 16) & 0x000000FF);
    temp_mac[3] = ((temp_data >> 24) & 0x000000FF);

    if (0 != read_reg(temp_addr + Ucm_NtpServer_ConfigMac2Reg, &temp_data))
    {
        snprintf(ntlts->ntpServer.MacAddr, sizeof(ntlts->ntpServer.MacAddr), "%s", "NA");

        return -2;
    }

    ntlts->ntpRegs.ConfigMac2Reg = temp_data;

    temp_mac[4] = ((temp_data >> 0) & 0x000000FF);
    temp_mac[5] = ((temp_data >> 8) & 0x000000FF);

    snprintf(ntlts->ntpServer.MacAddr, sizeof(ntlts->ntpServer.MacAddr), "%02x:%02x:%02x:%02x:%02x:%02x", temp_mac[0], temp_mac[1], temp_mac[2], temp_mac[3], temp_mac[4], temp_mac[5]);

    // vlan
    if (0 == read_reg(temp_addr + Ucm_NtpServer_ConfigVlanReg, &temp_data))
    {
        if ((temp_data & 0x00010000) == 0)
        {
            ntlts->ntpServer.vlanEnable = 0;
            // printf("disabled\n");
        }
        else
        {
            ntlts->ntpServer.vlanEnable = 1;

            // printf("enabled\n");
        }

        temp_data &= 0x0000FFFF;

        // snprintf(ntlts->ntpServer.vlanAddr, sizeof(ntlts->ntpServer.vlanAddr), "0x%04x", temp_data);
        ntlts->ntpServer.vlanAddr = temp_data;
    }
    else
    {
        ntlts->ntpServer.vlanEnable = 0;
        // snprintf(ntlts->ntpServer.vlanAddr, sizeof(ntlts->ntpServer.vlanAddr), "%s", "NA");
        ntlts->ntpServer.vlanAddr = 0;
    }

    // mode & server config
    if (0 == read_reg(temp_addr + Ucm_NtpServer_ConfigModeReg, &temp_data))
    {
        ntlts->ntpRegs.ConfigModeReg = temp_data;

        if (((temp_data >> 0) & 0x00000003) == 1)
        {

            snprintf(ntlts->ntpServer.ipMode, sizeof(ntlts->ntpServer.ipMode), "%s", "IPv4");
        }
        else if (((temp_data >> 0) & 0x00000003) == 2)
        {
            snprintf(ntlts->ntpServer.ipMode, sizeof(ntlts->ntpServer.ipMode), "%s", "IPv6");
        }
        else
        {
            snprintf(ntlts->ntpServer.ipMode, sizeof(ntlts->ntpServer.ipMode), "%s", "NA");
        }

        if ((temp_data & 0x00000010) == 0)
        {
            // ui->NtpServerUnicastModeCheckBox->setChecked(false);
            ntlts->ntpServer.unicastEnable = 0;
        }
        else
        {
            // ui->NtpServerUnicastModeCheckBox->setChecked(true);
            ntlts->ntpServer.unicastEnable = 1;
        }

        if ((temp_data & 0x00000020) == 0)
        {
            // ui->NtpServerMulticastModeCheckBox->setChecked(false);
            ntlts->ntpServer.multicastEnable = 0;
        }
        else
        {
            // ui->NtpServerMulticastModeCheckBox->setChecked(true);
            ntlts->ntpServer.multicastEnable = 1;
        }

        if ((temp_data & 0x00000040) == 0)
        {
            // ui->NtpServerBroadcastModeCheckBox->setChecked(false);
            ntlts->ntpServer.broadcastEnable = 0;
        }
        else
        {
            // ui->NtpServerBroadcastModeCheckBox->setChecked(true);
            ntlts->ntpServer.broadcastEnable = 1;
        }

        // ui->NtpServerPrecisionValue->setText(QString::number((char)((temp_data >> 8) & 0x000000FF)));
        // snprintf(ntlts->ntpServer.precision, sizeof(ntlts->ntpServer.precision), "%d", (int8_t)((temp_data >> 8) & 0x000000FF));
        ntlts->ntpServer.Precision = (int8_t)((temp_data >> 8) & 0x000000FF);
        // ui->NtpServerPollIntervalValue->setText(QString::number(((temp_data >> 16) & 0x000000FF)));
        // snprintf(ntlts->ntpServer.PollInterval, sizeof(ntlts->ntpServer.pollInterval), "%d", ((temp_data >> 16) & 0x000000FF));
        ntlts->ntpServer.PollInterval = ((temp_data >> 16) & 0x000000FF);
        // ui->NtpServerStratumValue->setText(QString::number(((temp_data >> 24) & 0x000000FF)));
        // snprintf(ntlts->ntpServer.stratum, sizeof(ntlts->ntpServer.stratum), "%d", ((temp_data >> 24) & 0x000000FF));
        ntlts->ntpServer.Stratum = ((temp_data >> 24) & 0x000000FF);
    }
    else
    {
        // ui->NtpServerIpModeValue->setCurrentText("NA");
        // ui->NtpServerUnicastModeCheckBox->setChecked(false);
        // ui->NtpServerMulticastModeCheckBox->setChecked(false);
        // ui->NtpServerBroadcastModeCheckBox->setChecked(false);
        // ui->NtpServerStratumValue->setText("NA");
        // ui->NtpServerPollIntervalValue->setText("NA");
        // ui->NtpServerPrecisionValue->setText("NA");

        snprintf(ntlts->ntpServer.ipMode, sizeof(ntlts->ntpServer.ipMode), "%s", "NA");
        ntlts->ntpServer.unicastEnable = 0;
        ntlts->ntpServer.multicastEnable = 0;
        ntlts->ntpServer.broadcastEnable = 0;
        ntlts->ntpServer.Precision = 0;
        ntlts->ntpServer.PollInterval = -1;
        ntlts->ntpServer.Stratum = -1;
        // snprintf(ntlts->ntpServer.stratum, sizeof(ntlts->ntpServer.stratum), "%s", "NA");
        // snprintf(ntlts->ntpServer.pollInterval, sizeof(ntlts->ntpServer.pollInterval), "%s", "NA");
        // snprintf(ntlts->ntpServer.precision, sizeof(ntlts->ntpServer.precision), "%s", "NA");
    }

    memset(temp_string, 0, sizeof(temp_string));

    // reference id
    // temp_string.clear();
    if (0 == read_reg(temp_addr + Ucm_NtpServer_ConfigReferenceIdReg, &temp_data))
    {

        ntlts->ntpRegs.ConfigReferenceIdReg = temp_data;
        // temp_string.append((QChar)((temp_data >> 24) & 0x000000FF));
        // temp_string.append((QChar)((temp_data >> 16) & 0x000000FF));
        // temp_string.append((QChar)((temp_data >> 8) & 0x000000FF));
        // temp_string.append((QChar)((temp_data >> 0) & 0x000000FF));

        temp_string[0] = ((temp_data >> 24) & 0x000000FF);
        temp_string[1] = ((temp_data >> 16) & 0x000000FF);
        temp_string[2] = ((temp_data >> 8) & 0x000000FF);
        temp_string[3] = ((temp_data >> 0) & 0x000000FF);

        // ui->NtpServerReferenceIdValue->setText(temp_string); // TODO

        snprintf(ntlts->ntpServer.referenceId, sizeof(ntlts->ntpServer.referenceId), "%.7s", temp_string);
    }
    else
    {
        // ui->NtpServerReferenceIdValue->setText("NA");
        // ntlts->ntpServer.referenceId
        snprintf(ntlts->ntpServer.referenceId, sizeof(ntlts->ntpServer.referenceId), "%s", "NA");
    }

    // ip
    // temp_string = ui->NtpServerIpModeValue->currentText();
    if (strncmp(ntlts->ntpServer.ipMode, "IPv4", strlen("IPv4")) == 0)
    {
        // temp_string.clear();
        memset(temp_string, 0, sizeof(temp_string));

        if (0 == read_reg(temp_addr + Ucm_NtpServer_ConfigIpReg, &temp_data))
        {
            ntlts->ntpRegs.ConfigIpReg = temp_data;

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

            snprintf(ntlts->ntpServer.ipAddr, sizeof(ntlts->ntpServer.ipAddr), "%d.%d.%d.%d", ip_bytes[3], ip_bytes[2], ip_bytes[1], ip_bytes[0]);

            // temp_string = QHostAddress(temp_ip).toString();

            // ui->NtpServerIpValue->setText(temp_string);
        }
        else
        {
            // ui->NtpServerIpValue->setText("NA");
            snprintf(ntlts->ntpServer.ipAddr, sizeof(ntlts->ntpServer.ipAddr), "%s", "NA");
        }
    }
    else if (strncmp(ntlts->ntpServer.ipMode, "IPv6", strlen("IPv6")) == 0)
    {
        // temp_string.clear();
        memset(temp_string, 0, sizeof(temp_string));

        if (0 == read_reg(temp_addr + Ucm_NtpServer_ConfigIpReg, &temp_data))
        {
            ntlts->ntpRegs.ConfigIpReg = temp_data;
            temp_ip6[0] = (temp_data >> 0) & 0x000000FF;
            temp_ip6[1] = (temp_data >> 8) & 0x000000FF;
            temp_ip6[2] = (temp_data >> 16) & 0x000000FF;
            temp_ip6[3] = (temp_data >> 24) & 0x000000FF;

            if (0 == read_reg(temp_addr + Ucm_NtpServer_ConfigIpv61Reg, &temp_data))
            {
                ntlts->ntpRegs.ConfigIpv61Reg = temp_data;
                temp_ip6[4] = (temp_data >> 0) & 0x000000FF;
                temp_ip6[5] = (temp_data >> 8) & 0x000000FF;
                temp_ip6[6] = (temp_data >> 16) & 0x000000FF;
                temp_ip6[7] = (temp_data >> 24) & 0x000000FF;

                if (0 == read_reg(temp_addr + Ucm_NtpServer_ConfigIpv62Reg, &temp_data))
                {
                    ntlts->ntpRegs.ConfigIpv62Reg = temp_data;

                    temp_ip6[8] = (temp_data >> 0) & 0x000000FF;
                    temp_ip6[9] = (temp_data >> 8) & 0x000000FF;
                    temp_ip6[10] = (temp_data >> 16) & 0x000000FF;
                    temp_ip6[11] = (temp_data >> 24) & 0x000000FF;

                    if (0 == read_reg(temp_addr + Ucm_NtpServer_ConfigIpv63Reg, &temp_data))
                    {
                        ntlts->ntpRegs.ConfigIpv63Reg = temp_data;

                        temp_ip6[12] = (temp_data >> 0) & 0x000000FF;
                        temp_ip6[13] = (temp_data >> 8) & 0x000000FF;
                        temp_ip6[14] = (temp_data >> 16) & 0x000000FF;
                        temp_ip6[15] = (temp_data >> 24) & 0x000000FF;

                        // temp_string = QHostAddress(temp_ip6).toString();

                        snprintf(ntlts->ntpServer.ipAddr, sizeof(ntlts->ntpServer.ipAddr), "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
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

                        // ui->NtpServerIpValue->setText(temp_string);
                    }
                    else
                    {
                        // ui->NtpServerIpValue->setText("NA");
                        snprintf(ntlts->ntpServer.ipAddr, sizeof(ntlts->ntpServer.ipAddr), "%s", "NA");
                    }
                }
                else
                {
                    // ui->NtpServerIpValue->setText("NA");
                    snprintf(ntlts->ntpServer.ipAddr, sizeof(ntlts->ntpServer.ipAddr), "%s", "NA");
                }
            }
            else
            {
                // ui->NtpServerIpValue->setText("NA");
                snprintf(ntlts->ntpServer.ipAddr, sizeof(ntlts->ntpServer.ipAddr), "%s", "NA");
            }
        }
        else
        {
            // ui->NtpServerIpValue->setText("NA");
            snprintf(ntlts->ntpServer.ipAddr, sizeof(ntlts->ntpServer.ipAddr), "%s", "NA");
        }
    }
    else
    {
        // ui->NtpServerIpValue->setText("NA");
        snprintf(ntlts->ntpServer.ipAddr, sizeof(ntlts->ntpServer.ipAddr), "%s", "NA");
    }

    // utc info
    temp_data = 0x40000000;
    if (0 == write_reg(temp_addr + Ucm_NtpServer_UtcInfoControlReg, &temp_data))
    {
        for (int i = 0; i < 10; i++)
        {
            if (0 == read_reg(temp_addr + Ucm_NtpServer_UtcInfoControlReg, &temp_data))
            {
                if ((temp_data & 0x80000000) != 0)
                {
                    if (0 == read_reg(temp_addr + Ucm_NtpServer_UtcInfoReg, &temp_data))
                    {

                        ntlts->ntpRegs.UtcInfoReg = temp_data;

                        if ((temp_data & 0x00000100) == 0)
                        {
                            // ui->NtpServerUtcSmearingCheckBox->setChecked(false);
                            ntlts->ntpServer.utcSmearingEnable = 0;
                        }
                        else
                        {
                            // ui->NtpServerUtcSmearingCheckBox->setChecked(true);
                            ntlts->ntpServer.utcSmearingEnable = 1;
                        }

                        if ((temp_data & 0x00000200) == 0)
                        {
                            // ui->NtpServerUtcLeap61InProgressCheckBox->setChecked(false);
                            ntlts->ntpServer.utcLeap61InProgress = 0;
                        }
                        else
                        {
                            // ui->NtpServerUtcLeap61InProgressCheckBox->setChecked(true);
                            ntlts->ntpServer.utcLeap61InProgress = 1;
                        }

                        if ((temp_data & 0x00000400) == 0)
                        {
                            // ui->NtpServerUtcLeap59InProgressCheckBox->setChecked(false);
                            ntlts->ntpServer.utcLeap59InProgress = 0;
                        }
                        else
                        {
                            // ui->NtpServerUtcLeap59InProgressCheckBox->setChecked(true);
                            ntlts->ntpServer.utcLeap59InProgress = 1;
                        }

                        if ((temp_data & 0x00000800) == 0)
                        {
                            // ui->NtpServerUtcLeap61CheckBox->setChecked(false);
                            ntlts->ntpServer.utcLeap61Enable = 0;
                        }
                        else
                        {
                            // ui->NtpServerUtcLeap61CheckBox->setChecked(true);
                            ntlts->ntpServer.utcLeap61Enable = 1;
                        }

                        if ((temp_data & 0x00001000) == 0)
                        {
                            /// ui->NtpServerUtcLeap59CheckBox->setChecked(false);
                            ntlts->ntpServer.utcLeap59Enable = 0;
                        }
                        else
                        {
                            // ui->NtpServerUtcLeap59CheckBox->setChecked(true);
                            ntlts->ntpServer.utcLeap59Enable = 1;
                        }

                        if ((temp_data & 0x00002000) == 0)
                        {
                            // ui->NtpServerUtcOffsetValCheckBox->setChecked(false);
                            ntlts->ntpServer.utcOffsetEnable = 0;
                        }
                        else
                        {
                            // ui->NtpServerUtcOffsetValCheckBox->setChecked(true);
                            ntlts->ntpServer.utcOffsetEnable = 1;
                        }

                        // ui->NtpServerUtcOffsetValue->setText(QString::number(((temp_data >> 16) & 0x0000FFFF)));
                        // snprintf(ntlts->ntpServer.utcOffset, sizeof(ntlts->ntpServer.utcOffset), "%d", ((temp_data >> 16) & 0x0000FFFF));
                        ntlts->ntpServer.utcOffset = ((temp_data >> 16) & 0x0000FFFF);
                    }
                    else
                    {
                        // ui->NtpServerUtcLeap59CheckBox->setChecked(false);
                        ntlts->ntpServer.utcLeap59Enable = 0;
                        // ui->NtpServerUtcLeap61CheckBox->setChecked(false);
                        ntlts->ntpServer.utcLeap61Enable = 0;
                        // ui->NtpServerUtcLeap59InProgressCheckBox->setChecked(false);
                        ntlts->ntpServer.utcLeap59InProgress = 0;
                        // ui->NtpServerUtcLeap61InProgressCheckBox->setChecked(false);
                        ntlts->ntpServer.utcLeap61InProgress = 0;
                        // ui->NtpServerUtcOffsetValCheckBox->setChecked(false);
                        ntlts->ntpServer.utcOffsetEnable = 0;
                        // ui->NtpServerUtcSmearingCheckBox->setChecked(false);
                        ntlts->ntpServer.utcSmearingEnable = 0;
                        // ui->NtpServerUtcOffsetValue->setText("NA");
                        // ntlts->ntpServer.utcOffset = 0;
                        // snprintf(ntlts->ntpServer.utcOffset, sizeof(ntlts->ntpServer.utcOffset), "%s", "NA");
                        ntlts->ntpServer.utcOffset = -1;
                    }
                    break;
                }
                else if (i == 9)
                {
                    // printf("read did not complete\n");
                    //  ui->NtpServerUtcLeap59CheckBox->setChecked(false);
                    ntlts->ntpServer.utcLeap59Enable = 0;
                    // ui->NtpServerUtcLeap61CheckBox->setChecked(false);
                    ntlts->ntpServer.utcLeap61Enable = 0;
                    // ui->NtpServerUtcLeap59InProgressCheckBox->setChecked(false);
                    ntlts->ntpServer.utcLeap59InProgress = 0;
                    // ui->NtpServerUtcLeap61InProgressCheckBox->setChecked(false);
                    ntlts->ntpServer.utcLeap61InProgress = 0;
                    // ui->NtpServerUtcOffsetValCheckBox->setChecked(false);
                    ntlts->ntpServer.utcOffsetEnable = 0;
                    // ui->NtpServerUtcSmearingCheckBox->setChecked(false);
                    ntlts->ntpServer.utcSmearingEnable = 0;
                    // ui->NtpServerUtcOffsetValue->setText("NA");
                    // ntlts->ntpServer.utcOffset = 0;
                    // snprintf(ntlts->ntpServer.utcOffset, sizeof(ntlts->ntpServer.utcOffset), "%s", "NA");
                    // ntlts->ntpServer.utcOffset = -1;
                }
            }
            else
            {
                // ui->NtpServerUtcLeap59CheckBox->setChecked(false);
                ntlts->ntpServer.utcLeap59Enable = 0;
                // ui->NtpServerUtcLeap61CheckBox->setChecked(false);
                ntlts->ntpServer.utcLeap61Enable = 0;
                // ui->NtpServerUtcLeap59InProgressCheckBox->setChecked(false);
                ntlts->ntpServer.utcLeap59InProgress = 0;
                // ui->NtpServerUtcLeap61InProgressCheckBox->setChecked(false);
                ntlts->ntpServer.utcLeap61InProgress = 0;
                // ui->NtpServerUtcOffsetValCheckBox->setChecked(false);
                ntlts->ntpServer.utcOffsetEnable = 0;
                // ui->NtpServerUtcSmearingCheckBox->setChecked(false);
                ntlts->ntpServer.utcSmearingEnable = 0;
                // ui->NtpServerUtcOffsetValue->setText("NA");
                // ntlts->ntpServer.utcOffset = 0;
                // snprintf(ntlts->ntpServer.utcOffset, sizeof(ntlts->ntpServer.utcOffset), "%s", "NA");
            }
        }
    }
    else
    {
        // ui->NtpServerUtcLeap59CheckBox->setChecked(false);
        ntlts->ntpServer.utcLeap59Enable = 0;
        // ui->NtpServerUtcLeap61CheckBox->setChecked(false);
        ntlts->ntpServer.utcLeap61Enable = 0;
        // ui->NtpServerUtcLeap59InProgressCheckBox->setChecked(false);
        ntlts->ntpServer.utcLeap59InProgress = 0;
        // ui->NtpServerUtcLeap61InProgressCheckBox->setChecked(false);
        ntlts->ntpServer.utcLeap61InProgress = 0;
        // ui->NtpServerUtcOffsetValCheckBox->setChecked(false);
        ntlts->ntpServer.utcOffsetEnable = 0;
        // ui->NtpServerUtcSmearingCheckBox->setChecked(false);
        ntlts->ntpServer.utcSmearingEnable = 0;
        // ui->NtpServerUtcOffsetValue->setText("NA");
        // ntlts->ntpServer.utcOffset = 0;
        // snprintf(ntlts->ntpServer.utcOffset, sizeof(ntlts->ntpServer.utcOffset), "%s", "NA");
    }

    // status
    if (0 == read_reg(temp_addr + Ucm_NtpServer_CountReqReg, &temp_data))
    {
        ntlts->ntpRegs.CountReqReg = temp_data;
        // ui->NtpServerRequestsValue->setText(QString::number(temp_data));
        // snprintf(ntlts->ntpServer.requests, sizeof(ntlts->ntpServer.requests), "%d", temp_data);
        ntlts->ntpServer.requests = temp_data;
    }

    else
    {
        // ui->NtpServerRequestsValue->setText("NA");
        // snprintf(ntlts->ntpServer.requests, sizeof(ntlts->ntpServer.requests), "%s", "NA");
        ntlts->ntpServer.requests = -1;
    }

    if (0 == read_reg(temp_addr + Ucm_NtpServer_CountRespReg, &temp_data))
    {
        ntlts->ntpRegs.CountRespReg = temp_data;
        // ui->NtpServerResponsesValue->setText(QString::number(temp_data));
        // snprintf(ntlts->ntpServer.responses, sizeof(ntlts->ntpServer.responses), "%d", temp_data);
        ntlts->ntpServer.responses = temp_data;
    }

    else
    {
        // snprintf(ntlts->ntpServer.responses, sizeof(ntlts->ntpServer.responses), "%s", "NA");
        ntlts->ntpServer.responses = -1;
    }

    if (0 == read_reg(temp_addr + Ucm_NtpServer_CountReqDroppedReg, &temp_data))
    {
        ntlts->ntpRegs.CountReqDroppedReg = temp_data;
        // snprintf(ntlts->ntpServer.requestsDropped, sizeof(ntlts->ntpServer.requestsDropped), "%d", temp_data);
        ntlts->ntpServer.requestsDropped = temp_data;
    }

    else
    {
        // snprintf(ntlts->ntpServer.requestsDropped, sizeof(ntlts->ntpServer.requestsDropped), "%s", "NA");
        ntlts->ntpServer.requestsDropped = -1;
    }

    if (0 == read_reg(temp_addr + Ucm_NtpServer_CountBroadcastReg, &temp_data))
    {
        ntlts->ntpRegs.CountBroadcastReg = temp_data;
        // snprintf(ntlts->ntpServer.broadcasts, sizeof(ntlts->ntpServer.broadcasts), "%d", temp_data);
        ntlts->ntpServer.broadcasts = temp_data;
    }

    else
    {
        // snprintf(ntlts->ntpServer.broadcasts, sizeof(ntlts->ntpServer.broadcasts), "%s", "NA");
        ntlts->ntpServer.broadcasts = -1;
    }

    if (0 == read_reg(temp_addr + Ucm_NtpServer_CountControlReg, &temp_data))
    {

        ntlts->ntpRegs.CountControlReg = temp_data;

        if ((temp_data & 0x00000001) == 0)
        {
            ntlts->ntpServer.clearCounters = 0;
        }
        else
        {
            ntlts->ntpServer.clearCounters = 1;
        }
    }
    else
    {
        ntlts->ntpServer.clearCounters = 0;
    }

    // version
    if (0 == read_reg(temp_addr + Ucm_NtpServer_VersionReg, &temp_data))
    {
        ntlts->ntpRegs.VersionReg = temp_data;
        ntlts->ntpServer.Version = temp_data;
        // snprintf(ntlts->ntpServer.version, sizeof(ntlts->ntpServer.version), "0x%08x", temp_data);
    }
    else
    {
        // snprintf(ntlts->ntpServer.version, sizeof(ntlts->ntpServer.version), "%s", "NA");
        ntlts->ntpServer.Version = -1;
    }

    return 0;
}

int8_t ntp_server_write_values(NTL_TS_T *ntlts, uint8_t fromRegisters)
{

    uint32_t temp_data = 0;
    uint32_t temp_addr = 0;

    // uint8_t temp_string[STRING_SIZE] = {0};

    char temp_string[64] = {0};

    temp_addr = ntlts->ntpRegs.StartAddr;

    // mac
    if (strlen(ntlts->ntpServer.MacAddr) > 17)
    {
        return -1;
    }
    // int j = 0;
    uint64_t temp_mac = 0;

    memcpy(temp_string, ntlts->ntpServer.MacAddr, sizeof(temp_string));

    for (int i = 0, j = 0; i < strlen(temp_string) + 1; i++)
    {
        if (temp_string[i] != ':')
        {
        	temp_string[j] = temp_string[i];
            j++;
        }

        if (temp_string[i] == '\0')
        {
            break;
        }
    }

    temp_mac = strtoull(temp_string, NULL, 16);

    temp_data = 0x00000000;
    temp_data |= (temp_mac >> 16) & 0x000000FF;
    temp_data = temp_data << 8;
    temp_data |= (temp_mac >> 24) & 0x000000FF;
    temp_data = temp_data << 8;
    temp_data |= (temp_mac >> 32) & 0x000000FF;
    temp_data = temp_data << 8;
    temp_data |= (temp_mac >> 40) & 0x000000FF;

    if (fromRegisters)
    {
        temp_data = ntlts->ntpRegs.ConfigMac1Reg;
    }
    if (0 == write_reg(temp_addr + Ucm_NtpServer_ConfigMac1Reg, &temp_data))
    {

        temp_data = 0x00000000;
        temp_data |= (temp_mac >> 0) & 0x000000FF;
        temp_data = temp_data << 8;
        temp_data |= (temp_mac >> 8) & 0x000000FF;
        if (fromRegisters)
        {
            temp_data = ntlts->ntpRegs.ConfigMac2Reg;
        }
        if (0 == write_reg(temp_addr + Ucm_NtpServer_ConfigMac2Reg, &temp_data))
        {

            temp_data = 0x00000004; // write
                                    // ntlts->ntpRegs.ConfigControlReg = temp_data;
            if (0 == write_reg(temp_addr + Ucm_NtpServer_ConfigControlReg, &temp_data))
            {
                // ui->NtpServerMacValue->setText(temp_string);
            }
            else
            {
                // ui->NtpServerMacValue->setText("NA");
            }
        }
        else
        {
            // ui->NtpServerMacValue->setText("NA");
            return -2;
        }
    }
    else
    {
        // ui->NtpServerMacValue->setText("NA");
        return -3;
    }

    // vlan
    // temp_string = ui->NtpServerVlanValue->text();

    // temp_data = strtol(ntlts->ntpServer.vlanAddr, NULL, 16);
    temp_data = ntlts->ntpServer.vlanAddr;

    // temp_data = temp_string.toUInt(nullptr, 16);
    temp_data &= 0x0000FFFF;
    if (ntlts->ntpServer.vlanEnable == 1)
    {
        // printf("trying to enable?\n");
        temp_data |= 0x00010000; // enable
    }

    if (fromRegisters)
    {
        temp_data = ntlts->ntpRegs.ConfigVlanReg;
    }
    if (0 == write_reg(temp_addr + Ucm_NtpServer_ConfigVlanReg, &temp_data))
    {
        temp_data &= 0x0000FFFF;
        // ui->NtpServerVlanValue->setText(QString("0x%1").arg(temp_data, 4, 16, QLatin1Char('0')));

        // printf("init write\n");

        temp_data = 0x00000002; // write
                                // ntlts->ntpRegs.ConfigControlReg = temp_data;
        if (0 == write_reg(temp_addr + Ucm_NtpServer_ConfigControlReg, &temp_data))
        {
            // nothing
            // printf("second write\n");
        }
        else
        {
            // ui->NtpServerVlanEnableCheckBox->setChecked(false);
            // ui->NtpServerVlanValue->setText("NA");
        }
    }
    else
    {
        // ui->NtpServerVlanEnableCheckBox->setChecked(false);
        // ui->NtpServerVlanValue->setText("NA");
    }

    if (0 == read_reg(temp_addr + Ucm_NtpServer_ConfigModeReg, &temp_data))
    {

        switch (temp_data & 0x00000003)
        {
        case 0x00000001:
            snprintf(ntlts->ntpServer.ipMode, sizeof(ntlts->ntpServer.ipMode), "IPv4");

            break;
        case 0x00000002:
            snprintf(ntlts->ntpServer.ipMode, sizeof(ntlts->ntpServer.ipMode), "IPv6");

            break;
        default:
            snprintf(ntlts->ntpServer.ipMode, sizeof(ntlts->ntpServer.ipMode), "NA");

            break;
        }
    }

    temp_data = 0x00000000;
    // temp_string = ui->NtpServerIpModeValue->currentText();
    if (strncmp(ntlts->ntpServer.ipMode, "IPv4", strlen("IPv4")) == 0)
    // if (temp_string == "IPv4")
    {
        temp_data |= 0x00000001;
    }
    else if (strncmp(ntlts->ntpServer.ipMode, "IPv6", strlen("IPv6")) == 0)
    {
        temp_data |= 0x01000002;
    }
    else
    {
        temp_data |= 0x00000000;
    }

    if (ntlts->ntpServer.unicastEnable)
    {
        temp_data |= 0x00000010;
    }

    if (ntlts->ntpServer.multicastEnable)
    {
        temp_data |= 0x00000020;
    }

    if (ntlts->ntpServer.broadcastEnable)
    {
        temp_data |= 0x00000040;
    }

    // temp_string = ui->NtpServerPrecisionValue->text();
    temp_data |= ((ntlts->ntpServer.Precision & 0x000000FF) << 8);

    // temp_string = ui->NtpServerPollIntervalValue->text();
    // temp_data |= ((temp_string.toUInt(nullptr, 10) & 0x000000FF) << 16);
    temp_data |= ((ntlts->ntpServer.PollInterval & 0x000000FF) << 16);

    // temp_string = ui->NtpServerStratumValue->text();
    // temp_data |= ((temp_string.toUInt(nullptr, 10) & 0x000000FF) << 24);
    temp_data |= ((ntlts->ntpServer.Stratum & 0x000000FF) << 24);

    if (fromRegisters)
    {
        temp_data = ntlts->ntpRegs.ConfigModeReg;
    }
    if (0 == write_reg(temp_addr + Ucm_NtpServer_ConfigModeReg, &temp_data))
    {

        switch (temp_data & 0x00000003)
        {
        case 0x00000001:
            snprintf(ntlts->ntpServer.ipMode, sizeof(ntlts->ntpServer.ipMode), "IPv4");

            break;
        case 0x00000002:
            snprintf(ntlts->ntpServer.ipMode, sizeof(ntlts->ntpServer.ipMode), "IPv6");

            break;
        default:
            snprintf(ntlts->ntpServer.ipMode, sizeof(ntlts->ntpServer.ipMode), "NA");

            break;
        }

        temp_data = 0x00000001; // write
                                // ntlts->ntpRegs.ConfigControlReg = temp_data;
        if (0 == write_reg(temp_addr + Ucm_NtpServer_ConfigControlReg, &temp_data))
        {
            // nothing
        }
        else
        {
            return -4;
        }
    }
    else
    {
        return -6;
    }

    temp_data = 0x00000000;

    char *token;
    char referenceOptions[91] = "NTP,NULL,LOCL,CESM,RBDM,PPS,IRIG,ACTS,USNO,PTB,TDF,DCF,MSF,WWV,WWVB,WWVH,CHU,LORC,OMEG,GPS";

    for (int i = 0; i < strlen(referenceOptions); i++) // num of ref options 20 + 1
    {
        token = (i == 0) ? strtok(referenceOptions, ",") : strtok(NULL, ",");
        if (token == NULL)
        {
            return -5;
        }

        if (0 == strncmp(token, ntlts->ntpServer.referenceId, strlen(ntlts->ntpServer.referenceId)))
        {
            break;
        }
    }

    if (strlen(ntlts->ntpServer.referenceId) == 1)
    {
        temp_data |= ntlts->ntpServer.referenceId[0];
        temp_data = temp_data << 24;
    }
    else if (strlen(ntlts->ntpServer.referenceId) == 2)
    {
        temp_data |= ntlts->ntpServer.referenceId[0];
        temp_data = temp_data << 8;
        temp_data |= ntlts->ntpServer.referenceId[1];
        temp_data = temp_data << 16;
    }
    else if (strlen(ntlts->ntpServer.referenceId) == 3)
    {
        temp_data |= ntlts->ntpServer.referenceId[0];
        temp_data = temp_data << 8;
        temp_data |= ntlts->ntpServer.referenceId[1];
        temp_data = temp_data << 8;
        temp_data |= ntlts->ntpServer.referenceId[2];
        temp_data = temp_data << 8;
    }
    else if (strlen(ntlts->ntpServer.referenceId) >= 4)
    {
        temp_data |= ntlts->ntpServer.referenceId[0];
        temp_data = temp_data << 8;
        temp_data |= ntlts->ntpServer.referenceId[1];
        temp_data = temp_data << 8;
        temp_data |= ntlts->ntpServer.referenceId[2];
        temp_data = temp_data << 8;
        temp_data |= ntlts->ntpServer.referenceId[3];
    }
    else
    {
        // nothing
    }
    if (fromRegisters)
    {
        temp_data = ntlts->ntpRegs.ConfigReferenceIdReg;
    }
    if (0 == write_reg(temp_addr + Ucm_NtpServer_ConfigReferenceIdReg, &temp_data))
    {
        temp_data = 0x00000010; // write
                                // ntlts->ntpRegs.ConfigControlReg = temp_data;
        if (0 == write_reg(temp_addr + Ucm_NtpServer_ConfigControlReg, &temp_data))
        {
            // nothing
        }
        else
        {
            // ui->NtpServerReferenceIdValue->setText("NA");
        }
    }
    else
    {
        // ui->NtpServerReferenceIdValue->setText("NA");
    }

    // ip
    // temp_string = ui->NtpServerIpModeValue->currentText();
    if (strncmp(ntlts->ntpServer.ipMode, "IPv4", strlen("IPv4")) == 0)
    {

        int32_t temp_ip[4] = {0};

        char temp_string[64] = {0};

        memcpy(temp_string, ntlts->ntpServer.ipAddr, sizeof(temp_string));

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
            temp_data = ntlts->ntpRegs.ConfigIpReg;
        }
        if (0 == write_reg(temp_addr + Ucm_NtpServer_ConfigIpReg, &temp_data))
        {
            temp_data = 0x00000008; // write
                                    // ntlts->ntpRegs.ConfigControlReg = temp_data;
            if (0 == write_reg(temp_addr + Ucm_NtpServer_ConfigControlReg, &temp_data))
            {
                // ui->NtpServerIpValue->setText(temp_string);
            }
            else
            {
                // ui->NtpServerIpValue->setText("NA");
            }
        }
        else
        {
            // ui->NtpServerIpValue->setText("NA");
        }
    }
    if (strncmp(ntlts->ntpServer.ipMode, "IPv6", strlen("IPv6")) == 0)
    {
        // temp_string = ui->NtpServerIpValue->text();
        // temp_ip6 = QHostAddress(temp_string).toIPv6Address();

        long temp_ip6[16] = {0};

        char temp_string[64] = {0};

        memcpy(temp_string, ntlts->ntpServer.ipAddr, sizeof(temp_string));

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
                return -77;
            }

            // Extract next two characters
            strncpy(byte, token + 2, 2);
            byte[2] = '\0';
            temp_ip6[i + 1] = strtol(byte, &err, 16);
            if (err == token || *err != '\0')
            {
                return -88;
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
            temp_data = ntlts->ntpRegs.ConfigIpReg;
        }

        // if (temp_string == "NA")
        if (strncmp(ntlts->ntpServer.ipAddr, "NA", strlen("NA")) == 0)
        {
            // nothing
        }
        else if (0 == write_reg(temp_addr + Ucm_NtpServer_ConfigIpReg, &temp_data))
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
                temp_data = ntlts->ntpRegs.ConfigIpv61Reg;
            }
            // if (temp_string == "NA")
            if (strncmp(ntlts->ntpServer.ipAddr, "NA", strlen("NA")) == 0)
            {
                // nothing
            }
            else if (0 == write_reg(temp_addr + Ucm_NtpServer_ConfigIpv61Reg, &temp_data))
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
                    temp_data = ntlts->ntpRegs.ConfigIpv62Reg;
                }
                // if (temp_string == "NA")
                if (strncmp(ntlts->ntpServer.ipAddr, "NA", strlen("NA")) == 0)
                {
                    // nothing
                }
                else if (0 == write_reg(temp_addr + Ucm_NtpServer_ConfigIpv62Reg, &temp_data))
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
                        temp_data = ntlts->ntpRegs.ConfigIpv63Reg;
                    }
                    // if (temp_string == "NA")
                    if (strncmp(ntlts->ntpServer.ipAddr, "NA", strlen("NA")) == 0)

                    {
                        // nothing
                    }
                    else if (0 == write_reg(temp_addr + Ucm_NtpServer_ConfigIpv63Reg, &temp_data))
                    {
                        temp_data = 0x00000008; // write
                                                // ntlts->ntpRegs.ConfigControlReg = temp_data;
                        if (0 == write_reg(temp_addr + Ucm_NtpServer_ConfigControlReg, &temp_data))
                        {
                            // ui->NtpServerIpValue->setText(temp_string);
                        }
                        else
                        {
                            // ui->NtpServerIpValue->setText("NA");
                        }
                    }
                    else
                    {
                        // ui->NtpServerIpValue->setText("NA");
                    }
                }
                else
                {
                    // ui->NtpServerIpValue->setText("NA");
                }
            }
            else
            {
                // ui->NtpServerIpValue->setText("NA");
            }
        }
        else
        {
            // ui->NtpServerIpValue->setText("NA");
        }
    }
    else
    {
        // ui->NtpServerIpValue->setText("NA");
    }

    temp_data = 0x00000000;
    // if (true == ui->NtpServerUtcSmearingCheckBox->isChecked())
    if (ntlts->ntpServer.utcSmearingEnable)
    {
        temp_data |= 0x00000100;
    }

    // if (true == ui->NtpServerUtcLeap61CheckBox->isChecked())
    if (ntlts->ntpServer.utcLeap61Enable)
    {
        temp_data |= 0x00000800;
    }

    // if (true == ui->NtpServerUtcLeap59CheckBox->isChecked())
    if (ntlts->ntpServer.utcLeap59Enable)
    {
        temp_data |= 0x00001000;
    }

    // if (true == ui->NtpServerUtcOffsetValCheckBox->isChecked())
    if (ntlts->ntpServer.utcOffsetEnable)
    {
        temp_data |= 0x00002000;
    }

    // temp_string = ui->NtpServerUtcOffsetValue->text();
    temp_data |= ((ntlts->ntpServer.utcOffset & 0x0000FFFF) << 16);
    if (fromRegisters)
    {
        temp_data = ntlts->ntpRegs.UtcInfoReg;
    }
    if (0 == write_reg(temp_addr + Ucm_NtpServer_UtcInfoReg, &temp_data))
    {
        temp_data = 0x00000003; // write utc info and leap
                                // ntlts->ntpRegs.UtcInfoControlReg = temp_data;
        if (0 == write_reg(temp_addr + Ucm_NtpServer_UtcInfoControlReg, &temp_data))
        {
            // nothing
        }
        else
        {
            // ui->NtpServerUtcLeap59CheckBox->setChecked(false);
        }
    }
    else
    {
        // ui->NtpServerUtcLeap59CheckBox->setChecked(false);
    }

    // status
    temp_data = 0x00000000; // nothing
    if (ntlts->ntpServer.clearCounters)
    {
        temp_data |= 0x00000001; // enable
    }
    if (fromRegisters)
    {
        temp_data = ntlts->ntpRegs.CountControlReg;
    }
    if (0 == write_reg(temp_addr + Ucm_NtpServer_CountControlReg, &temp_data))
    {
        // nothing
    }
    else
    {
    }

    temp_data = 0x00000000; // nothing
    if (ntlts->ntpServer.Enable)
    {
        temp_data |= 0x00000001; // enable
    }
    if (fromRegisters)
    {
        temp_data = ntlts->ntpRegs.ControlReg;
    }
    if (0 == write_reg(temp_addr + Ucm_NtpServer_ControlReg, &temp_data))
    {
        // nothing
    }
    else
    {
    }
    return 0;
}



void ntp_handler(char *temp_rsp, int rsp_size, const char *prop, char *val)
{

    int write = 0;
    int err = 0;

    err = ntp_server_read_values(&ntlts);

    if (err != 0)
    {
        snprintf(temp_rsp, rsp_size, "NTP_READ_ERR: %d", err);
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
            ntlts.ntpServer.Enable = strtoul(val, NULL, 10);
        }

        // return ram value
        snprintf(temp_rsp, rsp_size, "NTP,ENB,%d", ntlts.ntpServer.Enable);
    }

    // IP ADDRESS
    if (strncmp(prop, "IP", 2) == 0)
    {
        if (write)
        {
            memcpy(ntlts.ntpServer.ipAddr, val, sizeof(ntlts.ntpServer.ipAddr));
        }

        // return ram value
        snprintf(temp_rsp, rsp_size, "$NTP,IP,%s", ntlts.ntpServer.ipAddr);
    }

    // MAC ADDRESS
    else if (memcmp(prop, "MAC", 3) == 0)
    {
        if (write)
        {
            memcpy(ntlts.ntpServer.MacAddr, val, sizeof(ntlts.ntpServer.MacAddr));
        }

        // return ram value
        snprintf(temp_rsp, rsp_size, "$NTP,MAC,%s", ntlts.ntpServer.MacAddr);
    }

    // UTC OFFSET EN
    else if (memcmp(prop, "UOE", 3) == 0)
    {
        if (write)
        {

            ntlts.ntpServer.utcOffsetEnable = strtoul(val, NULL, 10);
        }

        // return ram value
        snprintf(temp_rsp, rsp_size, "$NTP,UOE,%d", ntlts.ntpServer.utcOffsetEnable);
    }
    // UTC_OFFSET
    else if (memcmp(prop, "UOF", 3) == 0)
    {
        if (write)
        {

            ntlts.ntpServer.utcOffset = strtoul(val, NULL, 10);
        }

        // return ram value
        snprintf(temp_rsp, rsp_size, "$NTP,UOF,%d", ntlts.ntpServer.utcOffset);
    }

    // GET REQUESTS
    else if (memcmp(prop, "REQ", 3) == 0)
    {
        // return ram value
        snprintf(temp_rsp, rsp_size, "$NTP,REQ,%d", ntlts.ntpServer.requests);
    }

    // GET RESPONSES
    else if (memcmp(prop, "RSP", 3) == 0)
    {
        // return ram value
        snprintf(temp_rsp, rsp_size, "$NTP,RSP,%d", ntlts.ntpServer.responses);
    }

    // GET BROADCASTS
    else if (memcmp(prop, "BCS", 3) == 0)
    {
        // return ram value
        snprintf(temp_rsp, rsp_size, "$NTP,BCS,%d", ntlts.ntpServer.broadcasts);
    }

    // GET / SET UNICAST MODE
    else if (memcmp(prop, "UNI", 3) == 0)
    {
        if (write)
        {
            ntlts.ntpServer.unicastEnable = strtoul(val, NULL, 10);
        }

        // return ram value
        snprintf(temp_rsp, rsp_size, "$NTP,UNI,%d", ntlts.ntpServer.unicastEnable);
    }

    // GET / SET MULTICAST MODE
    else if (memcmp(prop, "MTI", 3) == 0)
    {
        if (write)
        {
            ntlts.ntpServer.multicastEnable = strtoul(val, NULL, 10);
        }

        // return ram value
        snprintf(temp_rsp, rsp_size, "$NTP,MTI,%d", ntlts.ntpServer.multicastEnable);
    }

    // GET / SET BROADCAST MODE
    else if (memcmp(prop, "BRC", 3) == 0)
    {
        if (write)
        {
            ntlts.ntpServer.broadcastEnable = strtoul(val, NULL, 10);
        }

        // return ram value
        snprintf(temp_rsp, rsp_size, "$NTP,BRC,%d", ntlts.ntpServer.broadcastEnable);
    }

    // GET / SET STRATUM
    else if (memcmp(prop, "STR", 3) == 0)
    {
        if (write)
        {
            ntlts.ntpServer.Stratum = strtoul(val, NULL, 10);
        }

        // return ram value
        snprintf(temp_rsp, rsp_size, "$NTP,STR,%d", ntlts.ntpServer.Stratum);
    }

    // GET / SET PRECISION
    else if (memcmp(prop, "PRE", 3) == 0)
    {
        if (write)
        {
            ntlts.ntpServer.Precision = strtol(val, NULL, 10);
        }

        // return ram value
        snprintf(temp_rsp, rsp_size, "$NTP,PRE,%d", ntlts.ntpServer.Precision);
    }

    // GET / SET REF ID
    else if (memcmp(prop, "REF", 3) == 0)
    {
        if (write)
        {
            // ntlts.ntpServer.referenceId = strtol(val, NULL, 10);
            memcpy(ntlts.ntpServer.referenceId, val, sizeof(ntlts.ntpServer.referenceId));
        }

        // return ram value
        snprintf(temp_rsp, rsp_size, "$NTP,REF,%s", ntlts.ntpServer.referenceId);
    }

    // SET (clear) COUNTERS
    else if (memcmp(prop, "CNT", 3) == 0)
    {
        if (write)
        {

            ntlts.ntpServer.clearCounters = strtoul(val, NULL, 10);
        }

        // return ram value
        snprintf(temp_rsp, rsp_size, "$NTP,CNT,CLEARED");
    }

    // WRITE FPGA REGS WITH UPDATED RAM
    if (write)
    {
        err = ntp_server_write_values(&ntlts, 0);
        if (err != 0)
        {
            snprintf(temp_rsp, rsp_size, "NTP_WRITE_ERR: %d", err);
        }
        write = 0;

        err = ntp_server_read_values(&ntlts);

        if (err != 0)
        {
            snprintf(temp_rsp, rsp_size, "NTP_READ_ERR: %d", err);
        }
    }
}
