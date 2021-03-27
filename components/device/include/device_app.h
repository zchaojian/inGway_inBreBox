/*
 * device_app.h
 *
 *  Created on: Jul 27, 2020
 *      Author: liwei
 */

#ifndef EXAMPLES_WG_WG_MAIN_DEVICE_APP_H_
#define EXAMPLES_WG_WG_MAIN_DEVICE_APP_H_

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "modbus_svc.h"

#define CONFIG_DEVICE_MAXSURPPORT_PARTITION_TABLES	16
#define CONFIG_DEVICE_MAXSURPPORT					32
#define CONFIG_DEVICE_PARTITION_TABLE_LEN			32

typedef enum
{
	DEVICE_COMMPORT_TYPE_COM1 = 1,
	DEVICE_COMMPORT_TYPE_COM2 = 2,
	DEVICE_COMMPORT_TYPE_CAN = 3,
	DEVICE_COMMPORT_TYPE_BLE = 4,
}DeviceCommPortTypeTPDF;

typedef enum
{
	DEVICE_MAIN_TYPE_ACB = 1,
	DEVICE_MAIN_TYPE_MCCB = 2,
	DEVICE_MAIN_TYPE_MCB = 3,
	DEVICE_MAIN_TYPE_ELCB = 4,
	DEVICE_MAIN_TYPE_TEMPRATURE = 5,
	DEVICE_MAIN_TYPE_CHINT_METER_MCB = 6,
	DEVICE_MAIN_TYPE_ROUTER = 7,
	DEVICE_MAIN_TYPE_CHINT_MCB = 8,
	DEVICE_MAIN_TYPE_CHINT_DC_MCB = 9,
}DeviceMainTypeTPDF;

typedef struct
{
	union
	{
		uint8_t ucModbusAddr;
		uint8_t ucMeterAddr[6];
		uint8_t ucBleMac[6];
	}tAddr;
	uint16_t usConvertAddr;
	uint16_t usMainType;
	char SecondType[4];
	uint16_t usCommPort;
	uint16_t onlineState;
	uint16_t Reserver;
}DeviceConfigTPDF;

typedef enum
{
	DEVICE_PARTITION_TABLE_DATA_TYPE_UINT = 0,
	DEVICE_PARTITION_TABLE_DATA_TYPE_STATE = 1,
	DEVICE_PARTITION_TABLE_DATA_TYPE_FLOAT = 2,
	DEVICE_PARTITION_TABLE_DATA_TYPE_UINT_F1 = 3,
	DEVICE_PARTITION_TABLE_DATA_TYPE_UINT_F2 = 4,
	DEVICE_PARTITION_TABLE_DATA_TYPE_DINT = 5,
	DEVICE_PARTITION_TABLE_DATA_TYPE_SINT = 6,
	DEVICE_PARTITION_TABLE_DATA_TYPE_SINT_F1 = 7,
	DEVICE_PARTITION_TABLE_DATA_TYPE_SINT_F2 = 8,
}DevicePartitionTableDataTypeTPDF;

typedef struct
{
	ModbusFunctionTPDF tDevicePartitionTableRegType;	//modbus function code
	DevicePartitionTableDataTypeTPDF tDevicePartitionTableDataType;	//publish data type.like uint, float, double int, etc.
	uint16_t usStartAddr;	//register address
	uint16_t usLen;			//need read/write registe number
	uint16_t usMemory[CONFIG_DEVICE_PARTITION_TABLE_LEN];	//collected register data value
}DevicePartitionTableTPDF;

typedef struct
{
	DeviceConfigTPDF tDeviceConfig;
	DevicePartitionTableTPDF tDevicePartitionTable[CONFIG_DEVICE_MAXSURPPORT_PARTITION_TABLES];
	void *vTag;
}DeviceTPDF;

extern DeviceTPDF mDevice[16];
extern void DEV_vInit(void);
extern void DEV_vClear(void);
extern uint16_t *DEV_dGet_Mem(DeviceTPDF *tDev, uint16_t usAddr, uint16_t usSize);
extern DeviceTPDF *DEV_dFind_By_ConvertAddr(uint16_t usAddr);
extern void DEV_vCreat_PartitionTable_From_Example(DeviceMainTypeTPDF tMaintype, DevicePartitionTableTPDF *tDevicePartitionTable);

#endif /* EXAMPLES_WG_WG_MAIN_DEVICE_APP_H_ */
