
#include "cores.h"

#include "clkClock.h"
#include "confSlave.h"
#include "ntl_uart.h"
#include "ppsSlave.h"
#include "ptpOc.h"
#include "todSlave.h"
#include "ntpServer.h"

Core cores[NUM_CORES];
int core_count = 0;

int getCores(void)
{
    int err = 0;
    temp_data = 0;
    int64_t type = 0;

    Core tempCore;

    for (int i = 0; i < 256; i++)
    {
        if (0 == readRegister((0x00000000 + ((i * Ucm_Config_BlockSize) + Ucm_Config_TypeInstanceReg)), &temp_data))
        {
            // printf("temp data: %ld \n", temp_data);
            if ((i == 0) && ((((temp_data >> 16) & 0x0000FFFF) != Ucm_CoreConfig_ConfSlaveCoreType) || (((temp_data >> 0) & 0x0000FFFF) != 1)))
            {

                printf("ERROR: not a conf block at the address expected: %d\n", i);
                err = -1;
                break;
            }
            else if (temp_data == 0)
            {
                // printf("ERROR 2 \n");
                // err = -2;
                break;
            }
            else
            {

                // printf("ERROR 3 \n");
                // temp_config.core_type = ((temp_data >> 16) & 0x0000FFFF);
                type = ((temp_data >> 16) & 0x0000FFFF);

                switch (type)
                {

                case Ucm_CoreConfig_PhyConfigurationCoreType: // 30
                    type = 30;
                    break;
                case Ucm_CoreConfig_I2cConfigurationCoreType: // 31
                    type = 31;
                    break;
                case Ucm_CoreConfig_IoConfigurationCoreType: // 32
                    type = 32;
                    break;
                case Ucm_CoreConfig_EthernetTestplatformType: // 33
                    type = 33;
                    break;
                case Ucm_CoreConfig_MinSwitchCoreType: // 34
                    type = 34;
                    break;
                case Ucm_CoreConfig_ConfExtCoreType: // 35
                    type = 35;
                    break;
                default:
                    break;
                }

                //cores[core_count].core_type = type;
                tempCore.core_type = type;

                //cores[core_count].core_instance_nr = ((temp_data >> 0) & 0x0000FFFF);
                tempCore.core_instance_nr = ((temp_data >> 0) & 0x0000FFFF);

                //printf("core type: %lld\n", cores[core_count].core_type);
            }
        }
        else
        {
            //printf("ERROR 4 \n");
            err = -3;

            break;
        }

        if (0 == readRegister((0x00000000 + ((i * Ucm_Config_BlockSize) + Ucm_Config_BaseAddrLReg)), &temp_data))
        {
            //cores[core_count].address_range_low = temp_data;
            tempCore.address_range_low = temp_data;
        }
        else
        {
            err = -4;
            break;
        }

        if (0 == readRegister((0x00000000 + ((i * Ucm_Config_BlockSize) + Ucm_Config_BaseAddrHReg)), &temp_data))
        {
            //cores[core_count].address_range_high = temp_data;
            tempCore.address_range_high = temp_data;
        }
        else
        {
            err = -5;
            break;
        }

        if (0 == readRegister((0x00000000 + ((i * Ucm_Config_BlockSize) + Ucm_Config_IrqMaskReg)), &temp_data))
        {
            //cores[core_count].interrupt_mask = temp_data;
            tempCore.interrupt_mask = temp_data;
        }
        else
        {
            err = -6;
            break;
        }



        switch (tempCore.core_type)
               {
               case Ucm_CoreConfig_TodSlaveCoreType:

                   TOD_SLAVE.core_type = tempCore.core_type;
                   TOD_SLAVE.core_instance_nr = tempCore.core_instance_nr;
                   TOD_SLAVE.address_range_low = tempCore.address_range_low;
                   TOD_SLAVE.address_range_high = tempCore.address_range_high;
                   TOD_SLAVE.interrupt_mask = tempCore.interrupt_mask;
                   break;

               case Ucm_CoreConfig_PtpOrdinaryClockCoreType:
                   PTP_OC.core_type = tempCore.core_type;
                   PTP_OC.core_instance_nr = tempCore.core_instance_nr;
                   PTP_OC.address_range_low = tempCore.address_range_low;
                   PTP_OC.address_range_high = tempCore.address_range_high;
                   PTP_OC.interrupt_mask = tempCore.interrupt_mask;
                   break;

               case Ucm_CoreConfig_PpsSlaveCoreType:
                   PPS_SLAVE.core_type = tempCore.core_type;
                   PPS_SLAVE.core_instance_nr = tempCore.core_instance_nr;
                   PPS_SLAVE.address_range_low = tempCore.address_range_low;
                   PPS_SLAVE.address_range_high = tempCore.address_range_high;
                   PPS_SLAVE.interrupt_mask = tempCore.interrupt_mask;
                   break;

               case Ucm_CoreConfig_NtpServerCoreType:

                   NTP_SERVER.core_type = tempCore.core_type;
                   NTP_SERVER.core_instance_nr = tempCore.core_instance_nr;
                   NTP_SERVER.address_range_low = tempCore.address_range_low;
                   NTP_SERVER.address_range_high = tempCore.address_range_high;
                   NTP_SERVER.interrupt_mask = tempCore.interrupt_mask;
                   break;

               case Ucm_CoreConfig_ConfSlaveCoreType:

                   CONF_SLAVE.core_type = tempCore.core_type;
                   CONF_SLAVE.core_instance_nr = tempCore.core_instance_nr;
                   CONF_SLAVE.address_range_low = tempCore.address_range_low;
                   CONF_SLAVE.address_range_high = tempCore.address_range_high;
                   CONF_SLAVE.interrupt_mask = tempCore.interrupt_mask;
                   break;

               case Ucm_CoreConfig_ClkClockCoreType:

                   CLK_CLOCK.core_type = tempCore.core_type;
                   CLK_CLOCK.core_instance_nr = tempCore.core_instance_nr;
                   CLK_CLOCK.address_range_low = tempCore.address_range_low;
                   CLK_CLOCK.address_range_high = tempCore.address_range_high;
                   CLK_CLOCK.interrupt_mask = tempCore.interrupt_mask;
                   break;
               }

    }

    return err;
}



int getCoresProperties(void) {

		    //readTodSlaveAll();

		    //readPtpOcAll();

		    //readPpsSlaveAll();

		    //readNtpServerAll();

		    //readClkClockAll();

		    return 0;
}

