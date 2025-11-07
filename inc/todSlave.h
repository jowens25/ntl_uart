#ifndef TOD_SLAVE_H

#define TOD_SLAVE_H

#include <stddef.h>
#include <stdint.h>

#define Ucm_TodSlave_ControlReg 0x00000000
#define Ucm_TodSlave_StatusReg 0x00000004
#define Ucm_TodSlave_PolarityReg 0x00000008
#define Ucm_TodSlave_VersionReg 0x0000000C
#define Ucm_TodSlave_CorrectionReg 0x00000010
#define Ucm_TodSlave_UartBaudRateReg 0x00000020
#define Ucm_TodSlave_UtcStatusReg 0x00000030
#define Ucm_TodSlave_TimeToLeapSecondReg 0x00000034
#define Ucm_TodSlave_GnssStatus_Reg_Con 0x00000040
#define Ucm_TodSlave_SatelliteNumber_Reg_Con 0x00000044

int readTodSlaveAll(void);
int readTodSlaveVersion(char *version, size_t size);
int readTodSlaveInstance(char *instance, size_t size);
int readTodSlaveProtocol(char *protocol, size_t size);
int readTodSlaveGnss(char *gnss, size_t size);
int readTodSlaveMsgDisable(char *msgdisable, size_t size);
int readTodSlaveCorrection(char *correction, size_t size);
int readTodSlaveBaudRate(char *baudrate, size_t size);
int readTodSlaveInvertedPolarity(char *inverted, size_t size);
int readTodSlaveUtcOffset(char *utcoffset, size_t size);
int readTodSlaveUtcInfoValid(char *utcinfovalid, size_t size);
int readTodSlaveLeapAnnounce(char *leapannounce, size_t size);
int readTodSlaveLeap59(char *leap59, size_t size);
int readTodSlaveLeap61(char *leap61, size_t size);
int readTodSlaveLeapInfoValid(char *leapinfovalid, size_t size);
int readTodSlaveTimeToLeap(char *timetoleap, size_t size);
int readTodSlaveGnssFix(char *gnssfix, size_t size);
int readTodSlaveGnssFixOk(char *gnssfixok, size_t size);
int readTodSlaveSpoofingState(char *spoofingstate, size_t size);
int readTodSlaveFixAndSpoofingInfoValid(char *fixandspoofinginfovalid, size_t size);
int readTodSlaveJammingLevel(char *jamminglevel, size_t size);
int readTodSlaveJammingState(char *jammingstate, size_t size);
int readTodSlaveAntennaState(char *antennastate, size_t size);
int readTodSlaveAntennaAndJammingInfoValid(char *antennaandjamminginfovalid, size_t size);
int readTodSlaveNrOfSatellitesSeen(char *nrofsatellitesseen, size_t size);
int readTodSlaveNrOfSatellitesLocked(char *nrofsatelliteslocked, size_t size);
int readTodSlaveNrOfSatellitesInfo(char *nrofsatellitesinfo, size_t size);
int readTodSlaveEnable(char *enable, size_t size);
int readTodSlaveInputOk(char *inputok, size_t size);

int writeTodSlaveProtocol(char *protocol, size_t size);
int writeTodSlaveGnss(char *gnss, size_t size);
int writeTodSlaveMsgDisable(char *msgdisable, size_t size);
int writeTodSlaveCorrection(char *correction, size_t size);
int writeTodSlaveBaudRate(char *baudrate, size_t size);
int writeTodSlaveInvertedPolarity(char *inverted, size_t size);

int writeTodSlaveEnable(char *enable, size_t size);




typedef struct
{
    int64_t core_type;
    int64_t core_instance_nr;
    int64_t address_range_low;
    int64_t address_range_high;
    int64_t interrupt_mask;

    char Version[32];
    char Instance[32];
    char Protocol[32];
    char Gnss[32];
    char MsgDisable[32];
    char Correction[32];
    char BaudRate[32];
    char InvertedPolarity[32];
    char UtcOffset[32];
    char UtcInfoValid[32];
    char LeapAnnounce[32];
    char Leap59[32];
    char Leap61[32];
    char LeapInfoValid[32];
    char TimeToLeap[32];
    char GnssFix[32];
    char GnssFixOk[32];
    char SpoofingState[32];
    char FixAndSpoofingInfoValid[32];
    char JammingLevel[32];
    char JammingState[32];
    char AntennaState[32];
    char AntennaAndJammingInfoValid[32];
    char NrOfSatellitesSeen[32];
    char NrOfSatellitesLocked[32];
    char NrOfSatellitesInfo[32];
    char Enable[32];
    char InputOk[32];


} TOD_SLAVE_T;

extern TOD_SLAVE_T TOD;

#endif // TOD_SLAVE_H
