/*
 * devdisc_tsk.c
 *
 *  Created on: Sep 29, 2020
 *      Author: liwei
 */
#include "devdisc_tsk.h"

QueueHandle_t mDevDiscQueueSend;
QueueHandle_t mDevDiscQueueRec;
static uint8_t mRestore[CONFIG_BLEDEVICE_MAXSURPPORT][6];
static uint8_t mIndex = 0;
static uint8_t mIndexBak = 0;
static uint8_t mAddr = 0;
static uint8_t mBleEn = 0;
static uint8_t mTransMitMode = 0;

static void vDevdisc_BleScanResult(uint8_t *ucMac, uint8_t *ucData)
{
	uint8_t i, j;
	if((!mBleEn))
	{
		return;
	}
	j = 0;
	for(i = 0; i < mIndex; i++)
	{
		if(mRestore[i][0] == ucMac[0] && mRestore[i][1] == ucMac[1]
		   && mRestore[i][2] == ucMac[2] && mRestore[i][3] == ucMac[3]
		   && mRestore[i][4] == ucMac[4] && mRestore[i][5] == ucMac[5])
		{
			j = 1;
		}
	}
	if(!j)
	{
		if(ucData[3] != CONFIG_BLEDEVICE_NAMELEN + 1)
		{
			return;
		}
		if(memcmp(CONFIG_BLEDEVICE_NAME, &ucData[5], CONFIG_BLEDEVICE_NAMELEN))
		{
			return;
		}
		mRestore[mIndex][0] = ucMac[0];
		mRestore[mIndex][1] = ucMac[1];
		mRestore[mIndex][2] = ucMac[2];
		mRestore[mIndex][3] = ucMac[3];
		mRestore[mIndex][4] = ucMac[4];
		mRestore[mIndex][5] = ucMac[5];
		mIndex++;
		if(mIndex == CONFIG_BLEDEVICE_MAXSURPPORT)
			mIndex = 0;
		ESP_LOGW("DEVICE DISCOVERY", "Device Found:mac:%x:%x:%x:%x:%x:%x", ucMac[0], ucMac[1], ucMac[2], ucMac[3], ucMac[4], ucMac[5]);
		ESP_LOGI("discovery", "mIndex:%d", mIndex);
	}
}

static void vDevdisc_Start(void)
{
	MyQueueDataTPDF tMyQueueDataUp;
	if(mPartitionTable.usGatewayRunMode == GATEWAY_MODE_TRANSMIT)
	{
		mTransMitMode = 1;
		mPartitionTable.usGatewayRunMode = GATEWAY_MODE_PASSTHROUGH;
	}
	else
	{
		mTransMitMode = 0;
	}
	memset(mRestore, 0, CONFIG_BLEDEVICE_MAXSURPPORT * 6);
	mIndex = 0;
	mIndexBak = 0;
	mAddr = 0;
	mBleEn = 1;
	BLE_C_vStartScan();
	tMyQueueDataUp.tMyQueueCommand.tMyQueueCommandType = BleAdv;
	xQueueSend(mComm1QueueSend, (void * )&tMyQueueDataUp, (TickType_t)0);
}

void DEVDISC_Task(void *pvParameters)
{
	MyQueueDataTPDF tMyQueueDataUp;
	MyQueueDataTPDF tMyQueueDataDown;
	uint16_t usDelay;
	uint8_t ucRetrys = 0;
	ModbusFunctionTPDF tMyQueueCommandType = DeviceDiscoveryStop;
	BLE_C_vRegister_ScanResult_Callback(vDevdisc_BleScanResult);
	while(1)
	{
		if(xQueueReceive(mDevDiscQueueSend, (void * )&tMyQueueDataUp, (portTickType)0))
		{
			tMyQueueCommandType = tMyQueueDataUp.tMyQueueCommand.tMyQueueCommandType;
			switch(tMyQueueCommandType)
			{
				case DeviceDiscoveryReStart:
					ESP_LOGW("DEVICE DISCOVERY", "Start");
					vDevdisc_Start();
					ucRetrys = 0;
					break;
				case DeviceDiscoveryContinue:
					tMyQueueCommandType = DeviceDiscoveryContinue;
					ESP_LOGW("DEVICE DISCOVERY", "Continue");
					break;
				case DeviceDiscoveryStop:
					ESP_LOGW("DEVICE DISCOVERY", "Stop");
					break;
				default:
					break;
			}
		}
		vTaskDelay(1);
		switch(tMyQueueCommandType)
		{
			case DeviceDiscoveryReStart:
				if(mIndexBak < mIndex)
				{
					tMyQueueDataDown.tMyQueueCommand.tMyQueueCommandType = BroadcastAddr;
					tMyQueueDataDown.tMyQueueCommand.cData[0] = mRestore[mIndexBak][0];
					tMyQueueDataDown.tMyQueueCommand.cData[1] = mRestore[mIndexBak][1];
					tMyQueueDataDown.tMyQueueCommand.cData[2] = mRestore[mIndexBak][2];
					tMyQueueDataDown.tMyQueueCommand.cData[3] = mRestore[mIndexBak][3];
					tMyQueueDataDown.tMyQueueCommand.cData[4] = mRestore[mIndexBak][4];
					tMyQueueDataDown.tMyQueueCommand.cData[5] = mRestore[mIndexBak][5];
					tMyQueueDataDown.tMyQueueCommand.cData[6] = 0;
					tMyQueueDataDown.tMyQueueCommand.cData[7] = mAddr + CONFIG_BLEDEVICE_ADDR_OFFSET;
					xQueueSend(mComm1QueueSend, (void * )&tMyQueueDataDown, (TickType_t)0);
					usDelay = mPartitionTable.tCom1Config.usTimeout / 10;
					while(usDelay)
					{
						vTaskDelay(1);
						if(xQueueReceive(mComm1QueueRec, (void * )&tMyQueueDataDown, (portTickType)0))
						{
							break;
						}
						usDelay--;
					}
					if(!usDelay)
					{
						ucRetrys++;
						if(ucRetrys >= 5)
						{
							ucRetrys = 0;
							mIndexBak++;
						}
						break;
					}
					///<
					mDevice[mAddr].tDeviceConfig.usCommPort = DEVICE_COMMPORT_TYPE_COM1;
					mDevice[mAddr].tDeviceConfig.usMainType = DEVICE_MAIN_TYPE_CHINT_MCB;
					mDevice[mAddr].tDeviceConfig.usConvertAddr = mAddr + CONFIG_BLEDEVICE_ADDR_OFFSET;
					DEV_vCreat_PartitionTable_From_Example(mDevice[mAddr].tDeviceConfig.usMainType, mDevice[mAddr].tDevicePartitionTable);
					memcpy(&mPartitionTable.tDeviceConfig[mAddr], &mDevice[mAddr].tDeviceConfig, sizeof(DeviceConfigTPDF));
					for(usDelay = 0; usDelay < sizeof(DeviceConfigTPDF) / 2; usDelay++)
					{
						CFG_vSaveConfig(PARTITION_TABLE_ADDR_GATEWAY_DEVINFOR_ADDR - 1 + (sizeof(DeviceConfigTPDF) / 2) * mAddr + usDelay);
					}
					mIndexBak++;
					mAddr++;
					ESP_LOGI("discovery", "mIndexBak:%d, mAddr:%d\n", mIndexBak, mAddr);
					///<
				}
				break;
			case DeviceDiscoveryContinue:
				break;
			case DeviceDiscoveryStop:
				break;
			default:
				break;
		}
	}
}


