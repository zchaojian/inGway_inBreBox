/*
 * device_app.c
 *
 *  Created on: Jul 27, 2020
 *      Author: liwei
 */
#include "device_app.h"

static const char *TAG = "Device_app";

DeviceTPDF mDevice[16];

/* Through MainType to confirm DEV partition table info, and set every parameter */
void DEV_vCreat_PartitionTable_From_Example(DeviceMainTypeTPDF tMaintype, DevicePartitionTableTPDF *tDevicePartitionTable)
{
	ESP_LOGI(TAG, "tMaintype:%d", tMaintype);
	switch(tMaintype)
	{
		// case DEVICE_MAIN_TYPE_CHINT_METER_MCB:
		// 	tDevicePartitionTable[0].usLen = 18;
		// 	tDevicePartitionTable[0].usStartAddr = 0x0b00;
		// 	tDevicePartitionTable[0].tDevicePartitionTableRegType = ReadInputReg;
		// 	tDevicePartitionTable[0].tDevicePartitionTableDataType = DEVICE_PARTITION_TABLE_DATA_TYPE_FLOAT;
		// 	tDevicePartitionTable[1].usLen = 1;
		// 	tDevicePartitionTable[1].usStartAddr = 0x0a04;
		// 	tDevicePartitionTable[1].tDevicePartitionTableRegType = ReadInputReg;
		// 	tDevicePartitionTable[1].tDevicePartitionTableDataType = DEVICE_PARTITION_TABLE_DATA_TYPE_UINT;

		// 	tDevicePartitionTable[2].usLen = 1;
		// 	tDevicePartitionTable[2].usStartAddr = 0x0900;
		// 	tDevicePartitionTable[2].tDevicePartitionTableRegType = ReadInputReg;
		// 	tDevicePartitionTable[2].tDevicePartitionTableDataType = DEVICE_PARTITION_TABLE_DATA_TYPE_UINT;
		// 	break;
		// case DEVICE_MAIN_TYPE_TEMPRATURE:
		// 	tDevicePartitionTable[0].usLen = 1;
		// 	tDevicePartitionTable[0].usStartAddr = 0x0040;
		// 	tDevicePartitionTable[0].tDevicePartitionTableRegType = ReadInputReg;
		// 	tDevicePartitionTable[0].tDevicePartitionTableDataType = DEVICE_PARTITION_TABLE_DATA_TYPE_UINT;
		// 	break;
		// case DEVICE_MAIN_TYPE_ROUTER:
		// 	tDevicePartitionTable[0].usLen = 8;
		// 	tDevicePartitionTable[0].usStartAddr = 0x3400;
		// 	tDevicePartitionTable[0].tDevicePartitionTableRegType = ReadReg;
		// 	tDevicePartitionTable[0].tDevicePartitionTableDataType = DEVICE_PARTITION_TABLE_DATA_TYPE_UINT_F2;
		// 	break;
		case DEVICE_MAIN_TYPE_CHINT_MCB:
			///<state
			tDevicePartitionTable[0].usLen = 1;
			tDevicePartitionTable[0].usStartAddr = 0x0000;
			tDevicePartitionTable[0].tDevicePartitionTableRegType = ReadReg;
			tDevicePartitionTable[0].tDevicePartitionTableDataType = DEVICE_PARTITION_TABLE_DATA_TYPE_STATE;
			///<current
			tDevicePartitionTable[1].usLen = 1;
			tDevicePartitionTable[1].usStartAddr = 0x0001;
			tDevicePartitionTable[1].tDevicePartitionTableRegType = ReadReg;
			tDevicePartitionTable[1].tDevicePartitionTableDataType = DEVICE_PARTITION_TABLE_DATA_TYPE_UINT_F2;
			///<voltage
			tDevicePartitionTable[2].usLen = 1;
			tDevicePartitionTable[2].usStartAddr = 0x0005;
			tDevicePartitionTable[2].tDevicePartitionTableRegType = ReadReg;
			tDevicePartitionTable[2].tDevicePartitionTableDataType = DEVICE_PARTITION_TABLE_DATA_TYPE_UINT_F1;
			///<fac freq
			tDevicePartitionTable[3].usLen = 3;
			tDevicePartitionTable[3].usStartAddr = 0x000b;
			tDevicePartitionTable[3].tDevicePartitionTableRegType = ReadReg;
			tDevicePartitionTable[3].tDevicePartitionTableDataType = DEVICE_PARTITION_TABLE_DATA_TYPE_SINT;
			///<lkc power
			tDevicePartitionTable[4].usLen = 8;
			tDevicePartitionTable[4].usStartAddr = 0x00014;
			tDevicePartitionTable[4].tDevicePartitionTableRegType = ReadReg;
			tDevicePartitionTable[4].tDevicePartitionTableDataType = DEVICE_PARTITION_TABLE_DATA_TYPE_SINT;
			///<Reactive power
			tDevicePartitionTable[5].usLen = 2;
			tDevicePartitionTable[5].usStartAddr = 0x0022;
			tDevicePartitionTable[5].tDevicePartitionTableRegType = ReadReg;
			tDevicePartitionTable[5].tDevicePartitionTableDataType = DEVICE_PARTITION_TABLE_DATA_TYPE_DINT;
			
			///apparent power
			tDevicePartitionTable[6].usLen = 2;
			tDevicePartitionTable[6].usStartAddr = 0x002A;
			tDevicePartitionTable[6].tDevicePartitionTableRegType = ReadReg;
			tDevicePartitionTable[6].tDevicePartitionTableDataType = DEVICE_PARTITION_TABLE_DATA_TYPE_SINT;
			
			///Temperature
			tDevicePartitionTable[7].usLen = 1;
			tDevicePartitionTable[7].usStartAddr = 0x0040;
			tDevicePartitionTable[7].tDevicePartitionTableRegType = ReadReg;
			tDevicePartitionTable[7].tDevicePartitionTableDataType = DEVICE_PARTITION_TABLE_DATA_TYPE_UINT;
			
			///Modbus info
			tDevicePartitionTable[8].usLen = 3;
			tDevicePartitionTable[8].usStartAddr = 0x0100;
			tDevicePartitionTable[8].tDevicePartitionTableRegType = ReadReg;
			tDevicePartitionTable[8].tDevicePartitionTableDataType = DEVICE_PARTITION_TABLE_DATA_TYPE_UINT;

			/// action threshold
			tDevicePartitionTable[9].usLen = 9;
			tDevicePartitionTable[9].usStartAddr = 0x0103;
			tDevicePartitionTable[9].tDevicePartitionTableRegType = ReadReg;
			tDevicePartitionTable[9].tDevicePartitionTableDataType = DEVICE_PARTITION_TABLE_DATA_TYPE_UINT;
			
			/// event enable
			tDevicePartitionTable[10].usLen = 1;
			tDevicePartitionTable[10].usStartAddr = 0x010C;
			tDevicePartitionTable[10].tDevicePartitionTableRegType = ReadReg;
			tDevicePartitionTable[10].tDevicePartitionTableDataType = DEVICE_PARTITION_TABLE_DATA_TYPE_UINT;
			
			/// Switch reason
			tDevicePartitionTable[11].usLen = 1;
			tDevicePartitionTable[11].usStartAddr = 0x0200;
			tDevicePartitionTable[11].tDevicePartitionTableRegType = ReadReg;
			tDevicePartitionTable[11].tDevicePartitionTableDataType = DEVICE_PARTITION_TABLE_DATA_TYPE_STATE;
			
			tDevicePartitionTable[12].usLen = 2;
			tDevicePartitionTable[12].usStartAddr = 0x0280;
			tDevicePartitionTable[12].tDevicePartitionTableRegType = ReadReg;
			tDevicePartitionTable[12].tDevicePartitionTableDataType = DEVICE_PARTITION_TABLE_DATA_TYPE_UINT;
			
			tDevicePartitionTable[13].usLen = 2;
			tDevicePartitionTable[13].usStartAddr = 0x02C8;
			tDevicePartitionTable[13].tDevicePartitionTableRegType = ReadReg;
			tDevicePartitionTable[13].tDevicePartitionTableDataType = DEVICE_PARTITION_TABLE_DATA_TYPE_UINT;
			break;
		case DEVICE_MAIN_TYPE_CHINT_DC_MCB:
			///<state
			tDevicePartitionTable[0].usLen = 1;
			tDevicePartitionTable[0].usStartAddr = 0x0001;
			tDevicePartitionTable[0].tDevicePartitionTableRegType = ReadReg;
			tDevicePartitionTable[0].tDevicePartitionTableDataType = DEVICE_PARTITION_TABLE_DATA_TYPE_STATE;
			///<voltage
			tDevicePartitionTable[1].usLen = 2;
			tDevicePartitionTable[1].usStartAddr = 0x0002;
			tDevicePartitionTable[1].tDevicePartitionTableRegType = ReadReg;
			tDevicePartitionTable[1].tDevicePartitionTableDataType = DEVICE_PARTITION_TABLE_DATA_TYPE_UINT_F2;
			///<current
			tDevicePartitionTable[2].usLen = 2;
			tDevicePartitionTable[2].usStartAddr = 0x0004;
			tDevicePartitionTable[2].tDevicePartitionTableRegType = ReadReg;
			tDevicePartitionTable[2].tDevicePartitionTableDataType = DEVICE_PARTITION_TABLE_DATA_TYPE_UINT_F2;
			///<active power
			tDevicePartitionTable[3].usLen = 2;
			tDevicePartitionTable[3].usStartAddr = 0x0006;
			tDevicePartitionTable[3].tDevicePartitionTableRegType = ReadReg;
			tDevicePartitionTable[3].tDevicePartitionTableDataType = DEVICE_PARTITION_TABLE_DATA_TYPE_UINT_F1;

			///<Temperature
			tDevicePartitionTable[4].usLen = 1;
			tDevicePartitionTable[4].usStartAddr = 0x0040;
			tDevicePartitionTable[4].tDevicePartitionTableRegType = ReadReg;
			tDevicePartitionTable[4].tDevicePartitionTableDataType = DEVICE_PARTITION_TABLE_DATA_TYPE_UINT;
			
			/// action threshold
			tDevicePartitionTable[8].usLen =11;
			tDevicePartitionTable[8].usStartAddr = 0x2000;
			tDevicePartitionTable[8].tDevicePartitionTableRegType = ReadReg;
			tDevicePartitionTable[8].tDevicePartitionTableDataType = DEVICE_PARTITION_TABLE_DATA_TYPE_UINT;
			
			/// event enable
			tDevicePartitionTable[10].usLen = 1;
			tDevicePartitionTable[10].usStartAddr = 0x0200;
			tDevicePartitionTable[10].tDevicePartitionTableRegType = ReadReg;
			tDevicePartitionTable[10].tDevicePartitionTableDataType = DEVICE_PARTITION_TABLE_DATA_TYPE_STATE;
			break;
		default:
			break;
	}
}

uint16_t *DEV_dGet_Mem(DeviceTPDF *tDev, uint16_t usAddr, uint16_t usSize)
{
	DevicePartitionTableTPDF *j;
	if(!tDev)
	{
		return(0);
	}
	for(uint16_t i = 0; i < CONFIG_DEVICE_MAXSURPPORT_PARTITION_TABLES; i++)
	{
		j = &tDev->tDevicePartitionTable[i];
		if(usAddr >= j->usStartAddr
		   &&((usAddr + usSize) <= (j->usStartAddr + j->usLen)))
		{
			return(j->usMemory + usAddr - j->usStartAddr);
		}
	}
	return(0);
}

DeviceTPDF *DEV_dFind_By_ConvertAddr(uint16_t usAddr)
{
	if(!usAddr)
	{
		return(0);
	}
	for(uint8_t i = 0; i < CONFIG_DEVICE_MAXSURPPORT; i++)
	{
		/* judge added device addr is same with addr in received data frame? */
		if(mDevice[i].tDeviceConfig.usConvertAddr == usAddr)
		{
			ESP_LOGI(TAG, "usAddr:0x%02x", usAddr);
			ESP_LOGI(TAG, "mDevice[%x].tDeviceConfig.usConvertAddr:0x%02x", i,  mDevice[i].tDeviceConfig.usConvertAddr);
			ESP_LOGI(TAG, "mDevice[%x].tDeviceConfig.usCommPort:%d", i, mDevice[i].tDeviceConfig.usCommPort);
			//ESP_LOGI(TAG, "mDevice[i].tDeviceConfig.tAddr:%s", mDevice[i].tDeviceConfig.tAddr);
			ESP_LOGI(TAG, "mDevice[%x].tDeviceConfig.SecondType:%s", i, mDevice[i].tDeviceConfig.SecondType);
			ESP_LOGI(TAG, "mDevice[%x].tDeviceConfig.usMainType:%d", i, mDevice[i].tDeviceConfig.usMainType);
			return(&mDevice[i]);
		}
	}
	ESP_LOGI(TAG, "usAddr:0x%02x, Device addr don't add to \ndevice list! Only passthrough", usAddr);
	return(0);
}

void DEV_vClear(void)
{
	uint8_t i,j;
	for(i = 0; i < CONFIG_DEVICE_MAXSURPPORT; i++)
	{
		mDevice[i].tDeviceConfig.usConvertAddr = 0;
		for(j = 0; j < CONFIG_DEVICE_MAXSURPPORT_PARTITION_TABLES; j++)
		{
				mDevice[i].tDevicePartitionTable[j].usLen = 0;
		}
	}
}

void DEV_vInit(void)
{
	uint8_t i,j;
	for(i = 0; i < 16; i++)
	{
		if(mDevice[i].tDeviceConfig.usConvertAddr != 0)
		{
			DEV_vCreat_PartitionTable_From_Example(mDevice[i].tDeviceConfig.usMainType, mDevice[i].tDevicePartitionTable);
			for(j = 0; j < CONFIG_DEVICE_MAXSURPPORT_PARTITION_TABLES; j++)
			{
				if(mDevice[i].tDevicePartitionTable[j].usLen)
				{
					memset(mDevice[i].tDevicePartitionTable[j].usMemory, 0, CONFIG_DEVICE_PARTITION_TABLE_LEN * 2);
				}
			}
		}
	}
}
