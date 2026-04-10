/*
 * ntl_uart.h
 *
 *  Created on: Nov 3, 2025
 *      Author: JacobOwens
 */

#ifndef MAIN_INC_NTL_UART_H_
#define MAIN_INC_NTL_UART_H_

// #define USE_SOCKET
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef USE_SOCKET

#include <stdint.h>
#include <string.h>
#include <stdio.h>

extern int socket_fd;

int setup_socket(void);
void readSocket(int socket_fd, char *msg);

#endif

#define Ucm_CoreConfig_ConfSlaveCoreType 1
#define Ucm_CoreConfig_ClkClockCoreType 2
#define Ucm_CoreConfig_ClkSignalGeneratorCoreType 3
#define Ucm_CoreConfig_ClkSignalTimestamperCoreType 4
#define Ucm_CoreConfig_IrigSlaveCoreType 5
#define Ucm_CoreConfig_IrigMasterCoreType 6
#define Ucm_CoreConfig_PpsSlaveCoreType 7
#define Ucm_CoreConfig_PpsMasterCoreType 8
#define Ucm_CoreConfig_PtpOrdinaryClockCoreType 9
#define Ucm_CoreConfig_PtpTransparentClockCoreType 10
#define Ucm_CoreConfig_PtpHybridClockCoreType 11
#define Ucm_CoreConfig_RedHsrPrpCoreType 12
#define Ucm_CoreConfig_RtcSlaveCoreType 13
#define Ucm_CoreConfig_RtcMasterCoreType 14
#define Ucm_CoreConfig_TodSlaveCoreType 15
#define Ucm_CoreConfig_TodMasterCoreType 16
#define Ucm_CoreConfig_TapSlaveCoreType 17
#define Ucm_CoreConfig_DcfSlaveCoreType 18
#define Ucm_CoreConfig_DcfMasterCoreType 19
#define Ucm_CoreConfig_RedTsnCoreType 20
#define Ucm_CoreConfig_TsnIicCoreType 21
#define Ucm_CoreConfig_NtpServerCoreType 22
#define Ucm_CoreConfig_NtpClientCoreType 23
#define Ucm_CoreConfig_ClkFrequencyGeneratorCoreType 25
#define Ucm_CoreConfig_SynceNodeCoreType 26
#define Ucm_CoreConfig_PpsClkToPpsCoreType 27
#define Ucm_CoreConfig_PtpServerCoreType 28
#define Ucm_CoreConfig_PtpClientCoreType 29
#define Ucm_CoreConfig_PhyConfigurationCoreType 10000
#define Ucm_CoreConfig_I2cConfigurationCoreType 10001
#define Ucm_CoreConfig_IoConfigurationCoreType 10002
#define Ucm_CoreConfig_EthernetTestplatformType 10003
#define Ucm_CoreConfig_MinSwitchCoreType 10004
#define Ucm_CoreConfig_ConfExtCoreType 20000

typedef struct ClkRegisters
{
	uint32_t StartAddr;
	uint32_t ControlReg;
	uint32_t StatusReg;
	uint32_t SelectReg;
	uint32_t VersionReg;
	uint32_t TimeValueLReg;
	uint32_t TimeValueHReg;
	uint32_t TimeAdjValueLReg;
	uint32_t TimeAdjValueHReg;
	uint32_t OffsetAdjValueReg;
	uint32_t OffsetAdjIntervalReg;
	uint32_t DriftAdjValueReg;
	uint32_t DriftAdjIntervalReg;
	uint32_t InSyncThresholdReg;
	uint32_t ServoOffsetFactorPReg;
	uint32_t ServoOffsetFactorIReg;
	uint32_t ServoDriftFactorPReg;
	uint32_t ServoDriftFactorIReg;
	uint32_t StatusOffsetReg;
	uint32_t StatusDriftReg;
	uint32_t StatusOffsetFractionsReg;
	uint32_t StatusDriftFractionsReg;
} ClkRegisters;

typedef struct ClkClock
{
	int32_t Seconds;
	int32_t Nanoseconds;
	uint8_t TimeAdjEnable;
	uint8_t InSync;
	uint8_t InHoldover;
	int32_t InSyncThreshold;
	char Source[8];
	int8_t Enable;
	int32_t Version;
	double Drift;
	int32_t DriftInterval;
	// int32_t DriftAdjCheckBox;
	int32_t Offset;
	int32_t OffsetInterval;
	// int32_t OffsetAdjCheckBox;
	// int32_t PiOffsetMulP;
	// int32_t PiOffsetDivP;
	// int32_t PiOffsetMulI;
	// int32_t PiOffsetDivI;
	// int32_t PiDriftMulP;
	// int32_t PiDriftDivP;
	// int32_t PiDriftMulI;
	// int32_t PiDriftDivI;
	// int32_t PiSetCustomParametersCheckBox;
	float CorrectedOffset;
	float CorrectedDrift;
	// int32_t Date;
} ClkClock;

typedef struct TodRegisters
{
	uint32_t StartAddr;
	uint32_t ControlReg;
	uint32_t StatusReg;
	uint32_t PolarityReg;
	uint32_t VersionReg;
	uint32_t CorrectionReg;
	uint32_t UartBaudRateReg;
	uint32_t UtcStatusReg;
	uint32_t TimeToLeapSecondReg;
	uint32_t GnssStatus_Reg_Con;
	uint32_t SatelliteNumber_Reg_Con;
} TodRegisters;

typedef struct TodSlave
{
	char Protocol[8];
	char Gnss[16];
	// uint8_t MsgDisable;
	uint8_t Correction;
	uint32_t BaudRate;
	uint8_t Inverted;
	int8_t UtcOffset;
	uint8_t UtcInfoValid;
	// uint8_t LeapAnnounce;
	// uint8_t Leap59;
	// uint8_t Leap61;
	// uint8_t LeapInfoValid;
	// uint8_t TimeToLeap;
	char GnssFixType[32];
	uint8_t GnssFix; // bool
	// uint8_t SpoofingState;
	// uint8_t FixAndSpoofingInfoValid;
	// uint8_t JammingLevel;
	// uint8_t JammingState;
	// uint8_t AntennaState;
	// uint8_t AntennaAndJammingInfoValidCheckBox;
	// uint8_t NrOfSatellitesSeen;
	// uint8_t NrOfSatellitesLocked;
	// uint8_t NrOfSatellitesInfoCheckBox;
	uint8_t Enable;
	uint32_t Version;
	uint8_t InputOk;

} TodSlave;

typedef struct PpsRegisters
{
	uint32_t StartAddr;
	uint32_t ControlReg;
	uint32_t StatusReg;
	uint32_t PolarityReg;
	uint32_t VersionReg;
	uint32_t PulseWidthReg;
	uint32_t CableDelayReg;
} PpsRegisters;

typedef struct PpsSlave
{

	uint8_t PulseWidth;
	uint8_t CableDelay;
	uint8_t Inverted;
	uint8_t Enable;
	uint8_t InputOk;
	int32_t Version;

} PpsSlave;

typedef struct PtpRegisters
{
	uint32_t StartAddr;
	uint32_t ControlReg;
	uint32_t StatusReg;
	uint32_t VersionReg;
	uint32_t NrOfUnicastEntriesReg;
	uint32_t ConfigControlReg;
	uint32_t ConfigProfileReg;
	uint32_t ConfigVlanReg;
	uint32_t ConfigIpReg;
	uint32_t ConfigIpv61Reg;
	uint32_t ConfigIpv62Reg;
	uint32_t ConfigIpv63Reg;
	uint32_t DefaultDsControlReg;
	uint32_t DefaultDs1Reg;
	uint32_t DefaultDs2Reg;
	uint32_t DefaultDs3Reg;
	uint32_t DefaultDs4Reg;
	uint32_t DefaultDs5Reg;
	uint32_t DefaultDs6Reg;
	uint32_t DefaultDs7Reg;
	uint32_t PortDsControlReg;
	uint32_t PortDs1Reg;
	uint32_t PortDs2Reg;
	uint32_t PortDs3Reg;
	uint32_t PortDs4Reg;
	uint32_t PortDs5Reg;
	uint32_t PortDs6Reg;
	uint32_t PortDs7Reg;
	uint32_t PortDs8Reg;
	uint32_t CurrentDsControlReg;
	uint32_t CurrentDs1Reg;
	uint32_t CurrentDs2Reg;
	uint32_t CurrentDs3Reg;
	uint32_t CurrentDs4Reg;
	uint32_t CurrentDs5Reg;
	uint32_t ParentDsControlReg;
	uint32_t ParentDs1Reg;
	uint32_t ParentDs2Reg;
	uint32_t ParentDs3Reg;
	uint32_t ParentDs4Reg;
	uint32_t ParentDs5Reg;
	uint32_t ParentDs6Reg;
	uint32_t ParentDs7Reg;
	uint32_t ParentDs8Reg;
	uint32_t ParentDs9Reg;
	uint32_t TimePropertiesDsControlReg;
	uint32_t TimePropertiesDs1Reg;
	uint32_t TimePropertiesDs2Reg;
	uint32_t TimePropertiesDs3Reg;
	uint32_t TimePropertiesDs4Reg;
	uint32_t TimePropertiesDs5Reg;
	uint32_t TimePropertiesDs6Reg;
	uint32_t TimePropertiesDs7Reg;
	uint32_t TimePropertiesDs8Reg;
	uint32_t TimePropertiesDs9Reg;
	uint32_t UnicastDsControlReg;
	uint32_t UnicastDs1Reg;
	uint32_t UnicastDs2Reg;
	uint32_t UnicastDs3Reg;
	uint32_t UnicastDs4Reg;
	uint32_t UnicastDs5Reg;
	uint32_t UnicastDs6Reg;
	uint32_t UnicastDs7Reg;
	uint32_t UnicastDs8Reg;
	uint32_t UnicastDs9Reg;
	uint32_t UnicastDs10Reg;
	uint32_t UnicastDs11Reg;
	uint32_t UnicastDs12Reg;
} PtpRegisters;

typedef struct PtpOc
{
	uint8_t Enable;
	// uint8_t Vlan;
	// uint8_t VlanEnable;

	char Profile[16];
	char Layer[16];
	char DelayMechanism[16];
	char ipAddr[64];

	char DefaultDsClockId[64];
	uint32_t DefaultDsDomain;
	uint32_t DefaultDsPriority1;
	uint32_t DefaultDsPriority2;
	uint32_t DefaultDsVariance;
	uint32_t DefaultDsAccuracy;
	uint32_t DefaultDsClass;
	uint32_t DefaultDsShortId;
	uint32_t DefaultDsInaccuracy;
	uint8_t DefaultDsNrOfPorts;
	uint8_t DefaultDsTwoStep;
	uint8_t DefaultDsSignaling;
	uint8_t DefaultDsSlaveOnly;
	uint8_t DefaultDsMasterOnly;
	// uint8_t DefaultDsListedUnicastSlavesOnly;
	// uint8_t DefaultDsDisableOffsetCor;
	// uint8_t DefaultDsLpFilterWindow;
	// uint8_t DefaultDsLpFilter;
	int64_t PortDsPeerDelay;
	char PortDsState[16];
	uint8_t PortDsAsymmetry;
	uint8_t PortDsMaxPeerDelay;
	uint8_t PortDsPDelayReqLogMsgInterval;
	uint8_t PortDsDelayReceiptTimeout;
	uint8_t PortDsDelayReqLogMsgInterval;
	uint8_t PortDsAnnounceLogMsgInterval;
	uint8_t PortDsAnnounceReceiptTimeout;
	uint8_t PortDsSyncLogMsgInterval;
	uint8_t PortDsSyncReceiptTimeout;
	int32_t CurrentDsStepsRemoved;
	int32_t CurrentDsOffset;
	int64_t CurrentDsDelay;
	// uint8_t ParentDsParentClockId;
	// uint8_t ParentDsGmClockId;
	// uint8_t ParentDsGmPriority1;
	// uint8_t ParentDsGmPriority2;
	// uint8_t ParentDsGmAccuracy;
	// uint8_t ParentDsGmClass;
	// uint8_t ParentDsGmShortId;
	// uint8_t ParentDsGmInaccuracy;
	// uint8_t ParentDsNwInaccuracy;
	char ParentDsParentClockId[32];
	char ParentDsGmClockId[32];
	uint32_t ParentDsGmPriority1;
	uint32_t ParentDsGmPriority2;
	uint32_t ParentDsGmVariance;
	uint32_t ParentDsGmAccuracy;
	uint32_t ParentDsGmClass;
	uint32_t ParentDsGmShortId;
	uint32_t ParentDsGmInaccuracy;
	uint32_t ParentDsNwInaccuracy;

	uint8_t TimePropertiesDsTimeSource;
	uint8_t TimePropertiesDsPtpTimescale;
	uint8_t TimePropertiesDsFreqTraceable;
	uint8_t TimePropertiesDsTimeTraceable;
	uint8_t TimePropertiesDsLeap59;
	uint8_t TimePropertiesDsLeap61;
	uint8_t TimePropertiesDsUtcOffsetVal;
	int16_t TimePropertiesDsUtcOffset;
	int32_t TimePropertiesDsCurrentOffset;
	int32_t TimePropertiesDsJumpSeconds;
	uint64_t TimePropertiesDsNextJump;
	char TimePropertiesDsDisplayName[32];
	uint8_t TimePropertiesDsSetLocalProperties;
	uint32_t Version;

} PtpOc;

typedef struct NtpRegisters
{
	uint32_t StartAddr;
	uint32_t ControlReg;
	uint32_t StatusReg;
	uint32_t VersionReg;
	uint32_t CountControlReg;
	uint32_t CountReqReg;
	uint32_t CountRespReg;
	uint32_t CountReqDroppedReg;
	uint32_t CountBroadcastReg;
	uint32_t ConfigControlReg;
	uint32_t ConfigModeReg;
	uint32_t ConfigVlanReg;
	uint32_t ConfigMac1Reg;
	uint32_t ConfigMac2Reg;
	uint32_t ConfigIpReg;
	uint32_t ConfigIpv61Reg;
	uint32_t ConfigIpv62Reg;
	uint32_t ConfigIpv63Reg;
	uint32_t ConfigReferenceIdReg;
	uint32_t UtcInfoControlReg;
	uint32_t UtcInfoReg;
} NtpRegisters;

typedef struct NtpServer
{
	uint8_t Enable;
	char MacAddr[32];
	uint8_t vlanEnable;
	uint32_t vlanAddr;
	char ipMode[8];
	uint8_t unicastEnable;
	uint8_t multicastEnable;
	uint8_t broadcastEnable;
	int8_t Stratum;
	int8_t PollInterval;
	int8_t Precision;
	char referenceId[8];
	char ipAddr[64];
	uint8_t utcLeap59Enable;
	uint8_t utcLeap61Enable;
	uint8_t utcLeap59InProgress;
	uint8_t utcLeap61InProgress;
	uint8_t utcOffsetEnable;
	uint8_t utcSmearingEnable;
	int32_t utcOffset;
	int32_t requests;
	int32_t responses;
	int32_t requestsDropped;
	int32_t broadcasts;
	uint8_t clearCounters;
	int32_t Version;

} NtpServer;

typedef struct NTL_TS_T
{

	ClkRegisters clkRegs;
	ClkClock clkClock;
	TodRegisters todRegs;
	TodSlave todSlave;
	PpsRegisters ppsRegs;
	PpsSlave ppsSlave;
	PtpRegisters ptpRegs;
	PtpOc ptpOc;
	NtpRegisters ntpRegs;
	NtpServer ntpServer;

} NTL_TS_T;

extern struct NTL_TS_T ntlts;

int get_length_by_char(char *myString, char *myChar);

int8_t clk_clock_read_values(NTL_TS_T *ntlts);
int8_t clk_clock_write_values(NTL_TS_T *ntlts, uint8_t fromRegisters);

int8_t tod_slave_read_values(NTL_TS_T *ntlts);
int8_t tod_slave_write_values(NTL_TS_T *ntlts, uint8_t fromRegisters);

int8_t pps_slave_read_values(NTL_TS_T *ntlts);
int8_t pps_slave_write_values(NTL_TS_T *ntlts, uint8_t fromRegisters);

int8_t ptp_oc_read_values(NTL_TS_T *ntlts);
int8_t ptp_oc_write_values(NTL_TS_T *ntlts, uint8_t fromRegisters);

int8_t ntp_server_read_values(NTL_TS_T *ntlts);
int8_t ntp_server_write_values(NTL_TS_T *ntlts, uint8_t fromRegisters);

void fpga_read_all(void);

void fpga_write_all(uint8_t fromRegisters);
void ntp_handler(char *temp_rsp, int rsp_size, const char *prop, char *val);

#include "clkClock.h"
#include "ntl_uart.h"
#include "ppsSlave.h"
#include "ptpOc.h"
#include "todSlave.h"
#include "ntpServer.h"

extern volatile int ntlRspReceived;
extern volatile int ntlCmdReceived;

extern char ntlCommandBuff[32];
extern char ntlResponseBuff[32];

extern char ntlCmd[64];
extern char ntlRsp[64];
extern char gpntlBuff[64];

extern char *ntl;

void NTL_COM_HANDLER(char *temp_rsp, uint32_t temp_rsp_size);

extern int ntlWriteSuccess;

#define NTL_RESPONSE_TIMEOUT 1
#define STRING_SIZE 32

uint8_t read_reg(const uint32_t addr, uint32_t *data);
uint8_t write_reg(const uint32_t addr, uint32_t *data);

#endif /* MAIN_INC_NTL_UART_H_ */
