// driver.c
// #include "ntl_uart.h"
// #include "ntpServer.h"
// #include "cores.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/select.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <syslog.h>
#include <stdint.h>

#include "ntpServer.h"

#include "ntl_uart.h"

// #include "ppsSlave.h"

#include "socket_interface.h"

/*
 * ntl_uart.c
 *
 *  Created on: Nov 3, 2025
 *      Author: JacobOwens
 */

NTL_TS_T ntlts;

int main(int argc, char *argv[])
{

    socket_fd = setup_socket();

    ntlts.clkRegs.StartAddr = 0xB0000000;
    ntlts.todRegs.StartAddr = 0xB0040000;
    ntlts.ppsRegs.StartAddr = 0xB0030000;
    ntlts.ptpRegs.StartAddr = 0xB0010000;
    ntlts.ntpRegs.StartAddr = 0xB0020000;
    int err = clk_clock_read_values(&ntlts);
    /*
    int err = clk_clock_read_values(&ntlts);

    if (err != 0)
    {

        printf("clk read error: %d\n", err);
        return -1;
    }
    printf("=======================CLK====================\n");
    printf("Seconds: %d\r\n", ntlts.clkClock.Seconds);
    printf("Nanoseconds: %d\r\n", ntlts.clkClock.Nanoseconds);
    printf("TimeAdjEnable: %d\r\n", ntlts.clkClock.TimeAdjEnable);
    printf("InSync: %d\r\n", ntlts.clkClock.InSync);
    printf("InHoldover: %d\r\n", ntlts.clkClock.InHoldover);
    printf("InSyncThreshold: %d\r\n", ntlts.clkClock.InSyncThreshold);
    printf("Source: %s\r\n", ntlts.clkClock.Source);
    printf("Enable: %d\r\n", ntlts.clkClock.Enable);
    printf("Version: 0x%08x\r\n", ntlts.clkClock.Version);
    printf("CorrectedOffset: %f\r\n", ntlts.clkClock.CorrectedOffset);
    printf("CorrectedDrift: %f\r\n", ntlts.clkClock.CorrectedDrift);
    printf("==============================================\n");

    // snprintf(ntlts.clkClock.Source, strlen("TOD"), "TOD");

    memcpy(ntlts.clkClock.Source, "TOD", 3);

    ntlts.clkClock.InSyncThreshold = 500;
    ntlts.clkClock.Enable = 1;

    err = clk_clock_write_values(&ntlts, 0);

    if (err != 0)
    {

        printf("clk write error: %d\n", err);
        return -1;
    }

    err = clk_clock_read_values(&ntlts);

    if (err != 0)
    {

        printf("clk read error: %d\n", err);
        return -1;
    }

    printf("=======================CLK====================\n");
    printf("Seconds: %d\r\n", ntlts.clkClock.Seconds);
    printf("Nanoseconds: %d\r\n", ntlts.clkClock.Nanoseconds);
    printf("TimeAdjEnable: %d\r\n", ntlts.clkClock.TimeAdjEnable);
    printf("InSync: %d\r\n", ntlts.clkClock.InSync);
    printf("InHoldover: %d\r\n", ntlts.clkClock.InHoldover);
    printf("InSyncThreshold: %d\r\n", ntlts.clkClock.InSyncThreshold);
    printf("Source: %s\r\n", ntlts.clkClock.Source);
    printf("Enable: %d\r\n", ntlts.clkClock.Enable);
    printf("Version: 0x%08x\r\n", ntlts.clkClock.Version);
    printf("CorrectedOffset: %f\r\n", ntlts.clkClock.CorrectedOffset);
    printf("CorrectedDrift: %f\r\n", ntlts.clkClock.CorrectedDrift);
    printf("==============================================\n");

    */
    // printf("Seconds: %d\r\n", ntlts.clkClock.Seconds);
    // printf("InSync: %d\r\n", ntlts.clkClock.InSync);
    // printf("Source: %s\r\n", ntlts.clkClock.Source);
    /*
        err = tod_slave_read_values(&ntlts);
        if (0 != err)
        {

            printf("tod read error\n");
            return -1;
        }

        printf("=======================TOD====================\n");
        printf("Protocol: %s\r\n", ntlts.todSlave.Protocol);         // char
        printf("Gnss: %s\r\n", ntlts.todSlave.Gnss);                 // char
        printf("BaudRate: %d\r\n", ntlts.todSlave.BaudRate);         // uint32_t
        printf("Inverted: %d\r\n", ntlts.todSlave.Inverted);         // uint8_t
        printf("UtcOffset: %d\r\n", ntlts.todSlave.UtcOffset);       // int8_t
        printf("UtcInfoValid: %d\r\n", ntlts.todSlave.UtcInfoValid); // uint8_t
        printf("GnssFixType: %s\r\n", ntlts.todSlave.GnssFixType);   // char
        printf("GnssFix: %d\r\n", ntlts.todSlave.GnssFix);           // uint8_t
        printf("Enable: %d\r\n", ntlts.todSlave.Enable);             // uint8_t
        printf("Version: 0x%08x\r\n", ntlts.todSlave.Version);       // uint32_t
        printf("InputOk: %d\r\n", ntlts.todSlave.InputOk);           // uint8_t
        printf("==============================================\n");

        ntlts.todSlave.Enable = 1;
        ntlts.todSlave.InputOk = 1;

        memcpy(ntlts.todSlave.Protocol, "NMEA", 6);

        err = tod_slave_write_values(&ntlts, 0);
        if (0 != err)
        {

            printf("tod write error\n");
            return -1;
        }

        printf("=======================TOD====================\n");
        printf("Protocol: %s\r\n", ntlts.todSlave.Protocol);         // char
        printf("Gnss: %s\r\n", ntlts.todSlave.Gnss);                 // char
        printf("BaudRate: %d\r\n", ntlts.todSlave.BaudRate);         // uint32_t
        printf("Inverted: %d\r\n", ntlts.todSlave.Inverted);         // uint8_t
        printf("UtcOffset: %d\r\n", ntlts.todSlave.UtcOffset);       // int8_t
        printf("UtcInfoValid: %d\r\n", ntlts.todSlave.UtcInfoValid); // uint8_t
        printf("GnssFixType: %s\r\n", ntlts.todSlave.GnssFixType);   // char
        printf("GnssFix: %d\r\n", ntlts.todSlave.GnssFix);           // uint8_t
        printf("Enable: %d\r\n", ntlts.todSlave.Enable);             // uint8_t
        printf("Version: 0x%08x\r\n", ntlts.todSlave.Version);       // uint32_t
        printf("InputOk: %d\r\n", ntlts.todSlave.InputOk);           // uint8_t
        printf("==============================================\n");
        */
    /*
    err = pps_slave_read_values(&ntlts);
    if (0 != err)
    {

        printf("pps read error\n");
        return -1;
    }
    printf("=======================PPS====================\n");
    printf("PulseWidth: %d\r\n", ntlts.ppsSlave.PulseWidth);
    printf("CableDelay: %d\r\n", ntlts.ppsSlave.CableDelay);
    printf("Inverted: %d\r\n", ntlts.ppsSlave.Inverted);
    printf("Enable: %d\r\n", ntlts.ppsSlave.Enable);
    printf("InputOk: %d\r\n", ntlts.ppsSlave.InputOk);
    printf("Version: 0x%08x\r\n", ntlts.ppsSlave.Version);
    printf("==============================================\n");

    ntlts.ppsSlave.Enable = 1;

    err = pps_slave_write_values(&ntlts, 0);

    if (0 != err)
    {

        printf("pps write error\n");
        return -1;
    }
    printf("=======================PPS====================\n");
    printf("PulseWidth: %d\r\n", ntlts.ppsSlave.PulseWidth);
    printf("CableDelay: %d\r\n", ntlts.ppsSlave.CableDelay);
    printf("Inverted: %d\r\n", ntlts.ppsSlave.Inverted);
    printf("Enable: %d\r\n", ntlts.ppsSlave.Enable);
    printf("InputOk: %d\r\n", ntlts.ppsSlave.InputOk);
    printf("Version: 0x%08x\r\n", ntlts.ppsSlave.Version);
    printf("==============================================\n");
*/
    // s/*
    err = ptp_oc_read_values(&ntlts);
    if (0 != err)
    {

        printf("ptp read error %d\n", err);
        return -1;
    }
    printf("=======================PTP====================\n");
    printf("Enable: %d\r\n", ntlts.ptpOc.Enable);                             // uint8_t
    printf("Profile: %s\r\n", ntlts.ptpOc.Profile);                           // char
    printf("Layer: %s\r\n", ntlts.ptpOc.Layer);                               // char
    printf("DelayMechanism: %s\r\n", ntlts.ptpOc.DelayMechanism);             // char
    printf("ipAddr: %s\r\n", ntlts.ptpOc.ipAddr);                             // char
    printf("DefaultDsClockId: %s\r\n", ntlts.ptpOc.DefaultDsClockId);         // char
    printf("DefaultDsDomain: 0x%02x\r\n", ntlts.ptpOc.DefaultDsDomain);       // uint32_t
    printf("DefaultDsPriority1: 0x%02x\r\n", ntlts.ptpOc.DefaultDsPriority1); // uint32_t
    printf("DefaultDsPriority2: 0x%02x\r\n", ntlts.ptpOc.DefaultDsPriority2); // uint32_t
    printf("DefaultDsVariance: 0x%02x\r\n", ntlts.ptpOc.DefaultDsVariance);   // uint32_t
    printf("DefaultDsAccuracy: %d\r\n", ntlts.ptpOc.DefaultDsAccuracy);       // uint32_t
    printf("DefaultDsClass: 0x%02x\r\n", ntlts.ptpOc.DefaultDsClass);         // uint32_t
    printf("DefaultDsShortId: 0x%02x\r\n", ntlts.ptpOc.DefaultDsShortId);     // uint32_t
    printf("DefaultDsInaccuracy: %d\r\n", ntlts.ptpOc.DefaultDsInaccuracy);   // uint32_t
    printf("DefaultDsNrOfPorts: %d\r\n", ntlts.ptpOc.DefaultDsNrOfPorts);     // uint8_t
    printf("DefaultDsTwoStep: %d\r\n", ntlts.ptpOc.DefaultDsTwoStep);         // uint8_t
    printf("DefaultDsSignaling: %d\r\n", ntlts.ptpOc.DefaultDsSignaling);     // uint8_t
    printf("DefaultDsSlaveOnly: %d\r\n", ntlts.ptpOc.DefaultDsSlaveOnly);     // uint8_t
    printf("DefaultDsMasterOnly: %d\r\n", ntlts.ptpOc.DefaultDsMasterOnly);   // uint8_t
    printf("Version: 0x%08x\r\n", ntlts.ptpOc.Version);

    printf("PortDsPeerDelay: %d\r\n", ntlts.ptpOc.PortDsPeerDelay);
    printf("PortDsState: %s\r\n", ntlts.ptpOc.PortDsState);
    printf("PortDsAsymmetry: %d\r\n", ntlts.ptpOc.PortDsAsymmetry);
    printf("PortDsMaxPeerDelay: %d\r\n", ntlts.ptpOc.PortDsMaxPeerDelay);
    printf("PortDsPDelayReqLogMsgInterval: %d\r\n", ntlts.ptpOc.PortDsPDelayReqLogMsgInterval);
    printf("PortDsDelayReceiptTimeout: %d\r\n", ntlts.ptpOc.PortDsDelayReceiptTimeout);
    printf("PortDsDelayReqLogMsgInterval: %d\r\n", ntlts.ptpOc.PortDsDelayReqLogMsgInterval);
    printf("PortDsAnnounceLogMsgInterval: %d\r\n", ntlts.ptpOc.PortDsAnnounceLogMsgInterval);
    printf("PortDsAnnounceReceiptTimeout: %d\r\n", ntlts.ptpOc.PortDsAnnounceReceiptTimeout);
    printf("PortDsSyncLogMsgInterval: %d\r\n", ntlts.ptpOc.PortDsSyncLogMsgInterval);
    printf("PortDsSyncReceiptTimeout: %d\r\n", ntlts.ptpOc.PortDsSyncReceiptTimeout);

    printf("==============================================\n");

    // memcpy(ntlts.ptpOc.Layer, "Layer 3v4", 10);

    // memcpy(ntlts.ptpOc.ipAddr, "10.1.10.206", 12);

    err = ptp_oc_write_values(&ntlts, 0);
    if (0 != err)
    {

        printf("ptp write error %d\n", err);
        return -1;
    }
    ptp_oc_read_values(&ntlts);
    printf("=======================PTP====================\n");
    printf("Enable: %d\r\n", ntlts.ptpOc.Enable);                             // uint8_t
    printf("Profile: %s\r\n", ntlts.ptpOc.Profile);                           // char
    printf("Layer: %s\r\n", ntlts.ptpOc.Layer);                               // char
    printf("DelayMechanism: %s\r\n", ntlts.ptpOc.DelayMechanism);             // char
    printf("ipAddr: %s\r\n", ntlts.ptpOc.ipAddr);                             // char
    printf("DefaultDsClockId: %s\r\n", ntlts.ptpOc.DefaultDsClockId);         // char
    printf("DefaultDsDomain: 0x%02x\r\n", ntlts.ptpOc.DefaultDsDomain);       // uint32_t
    printf("DefaultDsPriority1: 0x%02x\r\n", ntlts.ptpOc.DefaultDsPriority1); // uint32_t
    printf("DefaultDsPriority2: 0x%02x\r\n", ntlts.ptpOc.DefaultDsPriority2); // uint32_t
    printf("DefaultDsVariance: 0x%02x\r\n", ntlts.ptpOc.DefaultDsVariance);   // uint32_t
    printf("DefaultDsAccuracy: %d\r\n", ntlts.ptpOc.DefaultDsAccuracy);       // uint32_t
    printf("DefaultDsClass: 0x%02x\r\n", ntlts.ptpOc.DefaultDsClass);         // uint32_t
    printf("DefaultDsShortId: 0x%02x\r\n", ntlts.ptpOc.DefaultDsShortId);     // uint32_t
    printf("DefaultDsInaccuracy: %d\r\n", ntlts.ptpOc.DefaultDsInaccuracy);   // uint32_t
    printf("DefaultDsNrOfPorts: %d\r\n", ntlts.ptpOc.DefaultDsNrOfPorts);     // uint8_t
    printf("DefaultDsTwoStep: %d\r\n", ntlts.ptpOc.DefaultDsTwoStep);         // uint8_t
    printf("DefaultDsSignaling: %d\r\n", ntlts.ptpOc.DefaultDsSignaling);     // uint8_t
    printf("DefaultDsSlaveOnly: %d\r\n", ntlts.ptpOc.DefaultDsSlaveOnly);     // uint8_t
    printf("DefaultDsMasterOnly: %d\r\n", ntlts.ptpOc.DefaultDsMasterOnly);   // uint8_t
    printf("Version: 0x%08x\r\n", ntlts.ptpOc.Version);

    printf("PortDsPeerDelay: %d\r\n", ntlts.ptpOc.PortDsPeerDelay);
    printf("PortDsState: %s\r\n", ntlts.ptpOc.PortDsState);
    printf("PortDsAsymmetry: %d\r\n", ntlts.ptpOc.PortDsAsymmetry);
    printf("PortDsMaxPeerDelay: %d\r\n", ntlts.ptpOc.PortDsMaxPeerDelay);
    printf("PortDsPDelayReqLogMsgInterval: %d\r\n", ntlts.ptpOc.PortDsPDelayReqLogMsgInterval);
    printf("PortDsDelayReceiptTimeout: %d\r\n", ntlts.ptpOc.PortDsDelayReceiptTimeout);
    printf("PortDsDelayReqLogMsgInterval: %d\r\n", ntlts.ptpOc.PortDsDelayReqLogMsgInterval);
    printf("PortDsAnnounceLogMsgInterval: %d\r\n", ntlts.ptpOc.PortDsAnnounceLogMsgInterval);
    printf("PortDsAnnounceReceiptTimeout: %d\r\n", ntlts.ptpOc.PortDsAnnounceReceiptTimeout);
    printf("PortDsSyncLogMsgInterval: %d\r\n", ntlts.ptpOc.PortDsSyncLogMsgInterval);
    printf("PortDsSyncReceiptTimeout: %d\r\n", ntlts.ptpOc.PortDsSyncReceiptTimeout);
    printf("==============================================\n");
    //
    //*/
    /*
    err = ntp_server_read_values(&ntlts);
    if (0 != err)
    {

        printf("ntp read error\n");
        return -1;
    }

    printf("=======================NTP====================\n");

    printf("Enable: %d\r\n", ntlts.ntpServer.Enable);
    printf("MacAddr: %s\r\n", ntlts.ntpServer.MacAddr);
    printf("vlanEnable: %d\r\n", ntlts.ntpServer.vlanEnable);
    printf("vlanAddr: 0x%04x\r\n", ntlts.ntpServer.vlanAddr);
    printf("ipMode: %s\r\n", ntlts.ntpServer.ipMode);
    printf("unicastEnable: %d\r\n", ntlts.ntpServer.unicastEnable);
    printf("multicastEnable: %d\r\n", ntlts.ntpServer.multicastEnable);
    printf("broadcastEnable: %d\r\n", ntlts.ntpServer.broadcastEnable);
    printf("Stratum: %d\r\n", ntlts.ntpServer.Stratum);
    printf("PollInterval: %d\r\n", ntlts.ntpServer.PollInterval);
    printf("Precision: %d\r\n", ntlts.ntpServer.Precision);
    printf("referenceId: %s\r\n", ntlts.ntpServer.referenceId);
    printf("ipAddr: %s\r\n", ntlts.ntpServer.ipAddr);
    printf("utcLeap59Enable: %d\r\n", ntlts.ntpServer.utcLeap59Enable);
    printf("utcLeap61Enable: %d\r\n", ntlts.ntpServer.utcLeap61Enable);
    printf("utcLeap59InProgress: %d\r\n", ntlts.ntpServer.utcLeap59InProgress);
    printf("utcLeap61InProgress: %d\r\n", ntlts.ntpServer.utcLeap61InProgress);
    printf("utcOffsetEnable: %d\r\n", ntlts.ntpServer.utcOffsetEnable);
    printf("utcSmearingEnable: %d\r\n", ntlts.ntpServer.utcSmearingEnable);
    printf("utcOffset: %d\r\n", ntlts.ntpServer.utcOffset);
    printf("requests: %d\r\n", ntlts.ntpServer.requests);
    printf("responses: %d\r\n", ntlts.ntpServer.responses);
    printf("requestsDropped: %d\r\n", ntlts.ntpServer.requestsDropped);
    printf("broadcasts: %d\r\n", ntlts.ntpServer.broadcasts);
    printf("clearCounters: %d\r\n", ntlts.ntpServer.clearCounters);
    printf("Version: 0x%08x\r\n", ntlts.ntpServer.Version);
    printf("==============================================\n");
    // memcpy(ntlts.ntpServer.ipAddr, "10.1.10.222", 12);

    // memcpy(ntlts.ntpServer.ipMode, "IPv4", 5);

    err = ntp_server_write_values(&ntlts, 0);
    if (0 != err)
    {

        printf("ntp write error %d\n", err);
        return -1;
    }

    err = ntp_server_read_values(&ntlts);
    if (0 != err)
    {

        printf("ntp read error\n");
        return -1;
    }

    printf("=======================NTP====================\n");

    printf("Enable: %d\r\n", ntlts.ntpServer.Enable);
    printf("MacAddr: %s\r\n", ntlts.ntpServer.MacAddr);
    printf("vlanEnable: %d\r\n", ntlts.ntpServer.vlanEnable);
    printf("vlanAddr: 0x%04x\r\n", ntlts.ntpServer.vlanAddr);
    printf("ipMode: %s\r\n", ntlts.ntpServer.ipMode);
    printf("unicastEnable: %d\r\n", ntlts.ntpServer.unicastEnable);
    printf("multicastEnable: %d\r\n", ntlts.ntpServer.multicastEnable);
    printf("broadcastEnable: %d\r\n", ntlts.ntpServer.broadcastEnable);
    printf("Stratum: %d\r\n", ntlts.ntpServer.Stratum);
    printf("PollInterval: %d\r\n", ntlts.ntpServer.PollInterval);
    printf("Precision: %d\r\n", ntlts.ntpServer.Precision);
    printf("referenceId: %s\r\n", ntlts.ntpServer.referenceId);
    printf("ipAddr: %s\r\n", ntlts.ntpServer.ipAddr);
    printf("utcLeap59Enable: %d\r\n", ntlts.ntpServer.utcLeap59Enable);
    printf("utcLeap61Enable: %d\r\n", ntlts.ntpServer.utcLeap61Enable);
    printf("utcLeap59InProgress: %d\r\n", ntlts.ntpServer.utcLeap59InProgress);
    printf("utcLeap61InProgress: %d\r\n", ntlts.ntpServer.utcLeap61InProgress);
    printf("utcOffsetEnable: %d\r\n", ntlts.ntpServer.utcOffsetEnable);
    printf("utcSmearingEnable: %d\r\n", ntlts.ntpServer.utcSmearingEnable);
    printf("utcOffset: %d\r\n", ntlts.ntpServer.utcOffset);
    printf("requests: %d\r\n", ntlts.ntpServer.requests);
    printf("responses: %d\r\n", ntlts.ntpServer.responses);
    printf("requestsDropped: %d\r\n", ntlts.ntpServer.requestsDropped);
    printf("broadcasts: %d\r\n", ntlts.ntpServer.broadcasts);
    printf("clearCounters: %d\r\n", ntlts.ntpServer.clearCounters);
    printf("Version: 0x%08x\r\n", ntlts.ntpServer.Version);

    printf("==============================================\n");

    */
    close(socket_fd);
}
