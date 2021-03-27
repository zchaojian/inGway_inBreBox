/**********************************************************************************
  *Copyright(C),2010-2011,CHNT
  *@FileName: eth_mqtt_chnt_mcb_handler.c
  *@Author:   ElonStark
  *@Version:  V1.0
  *@Date:     2020.11.13
  *@Description: 用于智慧微断的MQTT数据处理
  *History:  //修改历史记录列表，每条修改记录应包含修改日期、修改者及修改内容简介
  *   1.Date:
  *    Author:
  *    Modification:
**********************************************************************************/

#include "mqtt_mcb_handler.h"

#include "eth_tsk.h"
#include "cm_mqtt_handler.h"
#include "device_app.h"

#define TAG "CM_MCB_HANDLER"

void ConfigDeviceMCB(uint16_t break_index, cJSON *send_msg, cJSON *recv_property);
void ControlDeviceMCB(uint16_t break_index, cJSON *send_msg, int opr);
void DeviceEventMCB(uint16_t break_index, cJSON *send_json_data);
void DevicePropertyMCB(uint16_t break_index, cJSON *send_json_data, uint16_t mode);

void ConfigDeviceMCB(uint16_t break_index, cJSON *send_msg, cJSON *recv_property)
{
	MyQueueDataTPDF mMyQueueData;
	MyQueueDataTPDF tMyQueueData;

	tMyQueueData.tMyQueueCommand.tMyQueueCommandType = 0x00;
	if(cJSON_GetObjectItem(recv_property, "eventEnable") != NULL)
	{
		cJSON *recv_event_enable = cJSON_GetObjectItem(recv_property, "eventEnable");
		mMyQueueData.tMyQueueCommand.tMyQueueCommandType = (ModbusFunctionTPDF)WriteReg;
		mMyQueueData.tMyQueueCommand.usSlaveAddr = mDevice[break_index].tDeviceConfig.usConvertAddr;
		mMyQueueData.tMyQueueCommand.usRegAddr = 0x010C;
		mMyQueueData.tMyQueueCommand.usRegLen = 1;
		mMyQueueData.tMyQueueCommand.usDatalen = 2;
		mMyQueueData.tMyQueueCommand.cData[0] = 0x00;
		mMyQueueData.tMyQueueCommand.cData[1] = recv_event_enable->valueint;
		xQueueSend(mEthQueueRec, (void * )&mMyQueueData, (TickType_t)2000);
		xQueueReceive(mEthQueueSend, (void * )&tMyQueueData, (TickType_t)2000);
		ESP_LOGI(TAG, "tMyQueueCommandType:%d", tMyQueueData.tMyQueueCommand.tMyQueueCommandType);
		if(tMyQueueData.tMyQueueCommand.tMyQueueCommandType == (ModbusFunctionTPDF)WriteReg)
		{
			cJSON_AddStringToObject(send_msg, "eventEnable", "OK");
		}
		else cJSON_AddStringToObject(send_msg, "eventEnable", "ERR");
	}

	tMyQueueData.tMyQueueCommand.tMyQueueCommandType = 0x00;
	if(cJSON_GetObjectItem(recv_property, "modbusAddr") != NULL)
	{
		cJSON *recv_modbus_addr = cJSON_GetObjectItem(recv_property, "modbusAddr");	

		mMyQueueData.tMyQueueCommand.tMyQueueCommandType = (ModbusFunctionTPDF)WriteReg;
		mMyQueueData.tMyQueueCommand.usSlaveAddr = mDevice[break_index].tDeviceConfig.usConvertAddr;
		mMyQueueData.tMyQueueCommand.usRegAddr = 0x0100;
		mMyQueueData.tMyQueueCommand.usRegLen = 1;
		mMyQueueData.tMyQueueCommand.usDatalen = 2;
		mMyQueueData.tMyQueueCommand.cData[0] = 0x00;
		mMyQueueData.tMyQueueCommand.cData[1] = recv_modbus_addr->valueint;
		xQueueSend(mEthQueueRec, (void * )&mMyQueueData, (TickType_t)2000);
		xQueueReceive(mEthQueueSend, (void * )&tMyQueueData, (TickType_t)2000);
		ESP_LOGI(TAG, "tMyQueueCommandType:%d", tMyQueueData.tMyQueueCommand.tMyQueueCommandType);
		if(tMyQueueData.tMyQueueCommand.tMyQueueCommandType == (ModbusFunctionTPDF)WriteReg)
		{
			cJSON_AddStringToObject(send_msg, "modbusAddr", "OK");
		}
		else cJSON_AddStringToObject(send_msg, "modbusAddr", "ERR");
	}

	tMyQueueData.tMyQueueCommand.tMyQueueCommandType = 0x00;
	if(cJSON_GetObjectItem(recv_property, "bandRate") != NULL)
	{
		cJSON *recv_bandrate = cJSON_GetObjectItem(recv_property, "bandRate");
		mMyQueueData.tMyQueueCommand.tMyQueueCommandType = (ModbusFunctionTPDF)WriteReg;
		mMyQueueData.tMyQueueCommand.usSlaveAddr = mDevice[break_index].tDeviceConfig.usConvertAddr;
		mMyQueueData.tMyQueueCommand.usRegAddr = 0x0101;
		mMyQueueData.tMyQueueCommand.usRegLen = 1;
		mMyQueueData.tMyQueueCommand.usDatalen = 2;
		mMyQueueData.tMyQueueCommand.cData[0] = 0x00;
		mMyQueueData.tMyQueueCommand.cData[1] = recv_bandrate->valueint;
		xQueueSend(mEthQueueRec, (void * )&mMyQueueData, (TickType_t)2000);
		xQueueReceive(mEthQueueSend, (void * )&tMyQueueData, (TickType_t)2000);
		ESP_LOGI(TAG, "tMyQueueCommandType:%d", tMyQueueData.tMyQueueCommand.tMyQueueCommandType);
		if(tMyQueueData.tMyQueueCommand.tMyQueueCommandType == (ModbusFunctionTPDF)WriteReg)
		{
			cJSON_AddStringToObject(send_msg, "bandRate", "OK");
		}
		else cJSON_AddStringToObject(send_msg, "bandRate", "ERR");
	}
	
	tMyQueueData.tMyQueueCommand.tMyQueueCommandType = 0x00;
	if(cJSON_GetObjectItem(recv_property, "parity") != NULL)
	{
		cJSON *recv_parity = cJSON_GetObjectItem(recv_property, "parity");
		mMyQueueData.tMyQueueCommand.tMyQueueCommandType = (ModbusFunctionTPDF)WriteReg;
		mMyQueueData.tMyQueueCommand.usSlaveAddr = mDevice[break_index].tDeviceConfig.usConvertAddr;
		mMyQueueData.tMyQueueCommand.usRegAddr = 0x0102;
		mMyQueueData.tMyQueueCommand.usRegLen = 1;
		mMyQueueData.tMyQueueCommand.usDatalen = 2;
		mMyQueueData.tMyQueueCommand.cData[0] = 0x00;
		mMyQueueData.tMyQueueCommand.cData[1] = recv_parity->valueint;
		xQueueSend(mEthQueueRec, (void * )&mMyQueueData, (TickType_t)2000);
		xQueueReceive(mEthQueueSend, (void * )&tMyQueueData, (TickType_t)2000);
		ESP_LOGI(TAG, "tMyQueueCommandType:%d", tMyQueueData.tMyQueueCommand.tMyQueueCommandType);
		if(tMyQueueData.tMyQueueCommand.tMyQueueCommandType == (ModbusFunctionTPDF)WriteReg)
		{
			cJSON_AddStringToObject(send_msg, "parity", "OK");
		}
		else cJSON_AddStringToObject(send_msg, "parity", "ERR");
	}

	tMyQueueData.tMyQueueCommand.tMyQueueCommandType = 0x00;
	if(cJSON_GetObjectItem(recv_property, "unVolActValue") != NULL)
	{
		cJSON *recv_unVolActValue = cJSON_GetObjectItem(recv_property, "unVolActValue");
		int value = recv_unVolActValue->valueint;
		mMyQueueData.tMyQueueCommand.tMyQueueCommandType = (ModbusFunctionTPDF)WriteReg;
		mMyQueueData.tMyQueueCommand.usSlaveAddr = mDevice[break_index].tDeviceConfig.usConvertAddr;
		mMyQueueData.tMyQueueCommand.usRegAddr = 0x0103;
		mMyQueueData.tMyQueueCommand.usRegLen = 1;
		mMyQueueData.tMyQueueCommand.usDatalen = 2;
		mMyQueueData.tMyQueueCommand.cData[0] = (value >> 8) & 0x000000ff;
		mMyQueueData.tMyQueueCommand.cData[1] = value & 0x000000ff;
		xQueueSend(mEthQueueRec, (void * )&mMyQueueData, (TickType_t)2000);
		xQueueReceive(mEthQueueSend, (void * )&tMyQueueData, (TickType_t)2000);
		ESP_LOGI(TAG, "tMyQueueCommandType:%d", tMyQueueData.tMyQueueCommand.tMyQueueCommandType);
		if(tMyQueueData.tMyQueueCommand.tMyQueueCommandType == (ModbusFunctionTPDF)WriteReg)
		{
			cJSON_AddStringToObject(send_msg, "unVolActValue", "OK");
		}
		else cJSON_AddStringToObject(send_msg, "unVolActValue", "ERR");
	}

	tMyQueueData.tMyQueueCommand.tMyQueueCommandType = 0x00;
	if(cJSON_GetObjectItem(recv_property, "unVolActTime") != NULL)
	{
		cJSON *recv_unVolActTime = cJSON_GetObjectItem(recv_property, "unVolActTime");
		int value = recv_unVolActTime->valueint;
		mMyQueueData.tMyQueueCommand.tMyQueueCommandType = (ModbusFunctionTPDF)WriteReg;
		mMyQueueData.tMyQueueCommand.usSlaveAddr = mDevice[break_index].tDeviceConfig.usConvertAddr;
		mMyQueueData.tMyQueueCommand.usRegAddr = 0x0104;
		mMyQueueData.tMyQueueCommand.usRegLen = 1;
		mMyQueueData.tMyQueueCommand.usDatalen = 2;
		mMyQueueData.tMyQueueCommand.cData[0] = (value >> 8) & 0x000000ff;
		mMyQueueData.tMyQueueCommand.cData[1] = value & 0x000000ff;
		xQueueSend(mEthQueueRec, (void * )&mMyQueueData, (TickType_t)2000);
		xQueueReceive(mEthQueueSend, (void * )&tMyQueueData, (TickType_t)2000);
		ESP_LOGI(TAG, "tMyQueueCommandType:%d", tMyQueueData.tMyQueueCommand.tMyQueueCommandType);
		if(tMyQueueData.tMyQueueCommand.tMyQueueCommandType == (ModbusFunctionTPDF)WriteReg)
		{
			cJSON_AddStringToObject(send_msg, "unVolActTime", "OK");
		}
		else cJSON_AddStringToObject(send_msg, "unVolActTime", "ERR");
	}

	tMyQueueData.tMyQueueCommand.tMyQueueCommandType = 0x00;
	if(cJSON_GetObjectItem(recv_property, "unVolRetValue") != NULL)
	{
		cJSON *recv_unVolRetValue = cJSON_GetObjectItem(recv_property, "unVolRetValue");
		int value = recv_unVolRetValue->valueint;
		mMyQueueData.tMyQueueCommand.tMyQueueCommandType = (ModbusFunctionTPDF)WriteReg;
		mMyQueueData.tMyQueueCommand.usSlaveAddr = mDevice[break_index].tDeviceConfig.usConvertAddr;
		mMyQueueData.tMyQueueCommand.usRegAddr = 0x0105;
		mMyQueueData.tMyQueueCommand.usRegLen = 1;
		mMyQueueData.tMyQueueCommand.usDatalen = 2;
		mMyQueueData.tMyQueueCommand.cData[0] = (value >> 8) & 0x000000ff;
		mMyQueueData.tMyQueueCommand.cData[1] = value & 0x000000ff;
		xQueueSend(mEthQueueRec, (void * )&mMyQueueData, (TickType_t)2000);
		xQueueReceive(mEthQueueSend, (void * )&tMyQueueData, (TickType_t)2000);
		ESP_LOGI(TAG, "tMyQueueCommandType:%d", tMyQueueData.tMyQueueCommand.tMyQueueCommandType);
		if(tMyQueueData.tMyQueueCommand.tMyQueueCommandType == (ModbusFunctionTPDF)WriteReg)
		{
			cJSON_AddStringToObject(send_msg, "unVolRetValue", "OK");
		}
		else cJSON_AddStringToObject(send_msg, "unVolRetValue", "ERR");
	}

	tMyQueueData.tMyQueueCommand.tMyQueueCommandType = 0x00;
	if(cJSON_GetObjectItem(recv_property, "recoveryTime") != NULL)
	{
		cJSON *recv_recoveryTime = cJSON_GetObjectItem(recv_property, "recoveryTime");
		int value = recv_recoveryTime->valueint;
		mMyQueueData.tMyQueueCommand.tMyQueueCommandType = (ModbusFunctionTPDF)WriteReg;
		mMyQueueData.tMyQueueCommand.usSlaveAddr = mDevice[break_index].tDeviceConfig.usConvertAddr;
		mMyQueueData.tMyQueueCommand.usRegAddr = 0x0106;
		mMyQueueData.tMyQueueCommand.usRegLen = 1;
		mMyQueueData.tMyQueueCommand.usDatalen = 2;
		mMyQueueData.tMyQueueCommand.cData[0] = (value >> 8) & 0x000000ff;
		mMyQueueData.tMyQueueCommand.cData[1] = value & 0x000000ff;
		xQueueSend(mEthQueueRec, (void * )&mMyQueueData, (TickType_t)2000);
		xQueueReceive(mEthQueueSend, (void * )&tMyQueueData, (TickType_t)2000);
		ESP_LOGI(TAG, "tMyQueueCommandType:%d", tMyQueueData.tMyQueueCommand.tMyQueueCommandType);
		if(tMyQueueData.tMyQueueCommand.tMyQueueCommandType == (ModbusFunctionTPDF)WriteReg)
		{
			cJSON_AddStringToObject(send_msg, "recoveryTime", "OK");
		}
		else cJSON_AddStringToObject(send_msg, "recoveryTime", "ERR");
	}
	
	tMyQueueData.tMyQueueCommand.tMyQueueCommandType = 0x00;
	if(cJSON_GetObjectItem(recv_property, "ovVolActValue") != NULL)
	{
		cJSON *recv_ovVolActValue = cJSON_GetObjectItem(recv_property, "ovVolActValue");
		int value = recv_ovVolActValue->valueint;
		mMyQueueData.tMyQueueCommand.tMyQueueCommandType = (ModbusFunctionTPDF)WriteReg;
		mMyQueueData.tMyQueueCommand.usSlaveAddr = mDevice[break_index].tDeviceConfig.usConvertAddr;
		mMyQueueData.tMyQueueCommand.usRegAddr = 0x0107;
		mMyQueueData.tMyQueueCommand.usRegLen = 1;
		mMyQueueData.tMyQueueCommand.usDatalen = 2;
		mMyQueueData.tMyQueueCommand.cData[0] = (value >> 8) & 0x000000ff;
		mMyQueueData.tMyQueueCommand.cData[1] = value & 0x000000ff;
		xQueueSend(mEthQueueRec, (void * )&mMyQueueData, (TickType_t)2000);
		xQueueReceive(mEthQueueSend, (void * )&tMyQueueData, (TickType_t)2000);
		ESP_LOGI(TAG, "tMyQueueCommandType:%d", tMyQueueData.tMyQueueCommand.tMyQueueCommandType);
		if(tMyQueueData.tMyQueueCommand.tMyQueueCommandType == (ModbusFunctionTPDF)WriteReg)
		{
			cJSON_AddStringToObject(send_msg, "ovVolActValue", "OK");
		}
		else cJSON_AddStringToObject(send_msg, "ovVolActValue", "ERR");
	}

	tMyQueueData.tMyQueueCommand.tMyQueueCommandType = 0x00;
	if(cJSON_GetObjectItem(recv_property, "ovVolActTime") != NULL)
	{
		cJSON *recv_ovVolActTime = cJSON_GetObjectItem(recv_property, "ovVolActTime");
		int value = recv_ovVolActTime->valueint;
		mMyQueueData.tMyQueueCommand.tMyQueueCommandType = (ModbusFunctionTPDF)WriteReg;
		mMyQueueData.tMyQueueCommand.usSlaveAddr = mDevice[break_index].tDeviceConfig.usConvertAddr;
		mMyQueueData.tMyQueueCommand.usRegAddr = 0x0108;
		mMyQueueData.tMyQueueCommand.usRegLen = 1;
		mMyQueueData.tMyQueueCommand.usDatalen = 2;
		mMyQueueData.tMyQueueCommand.cData[0] = (value >> 8) & 0x000000ff;
		mMyQueueData.tMyQueueCommand.cData[1] = value & 0x000000ff;
		xQueueSend(mEthQueueRec, (void * )&mMyQueueData, (TickType_t)2000);
		xQueueReceive(mEthQueueSend, (void * )&tMyQueueData, (TickType_t)2000);
		ESP_LOGI(TAG, "tMyQueueCommandType:%d", tMyQueueData.tMyQueueCommand.tMyQueueCommandType);
		if(tMyQueueData.tMyQueueCommand.tMyQueueCommandType == (ModbusFunctionTPDF)WriteReg)
		{
			cJSON_AddStringToObject(send_msg, "ovVolActTime", "OK");
		}
		else cJSON_AddStringToObject(send_msg, "ovVolActTime", "ERR");
	}

	tMyQueueData.tMyQueueCommand.tMyQueueCommandType = 0x00;
	if(cJSON_GetObjectItem(recv_property, "ovVolRetValue") != NULL)
	{
		cJSON *recv_ovVolRetValue = cJSON_GetObjectItem(recv_property, "ovVolRetValue");
		int value = recv_ovVolRetValue->valueint;
		mMyQueueData.tMyQueueCommand.tMyQueueCommandType = (ModbusFunctionTPDF)WriteReg;
		mMyQueueData.tMyQueueCommand.usSlaveAddr = mDevice[break_index].tDeviceConfig.usConvertAddr;
		mMyQueueData.tMyQueueCommand.usRegAddr = 0x0109;
		mMyQueueData.tMyQueueCommand.usRegLen = 1;
		mMyQueueData.tMyQueueCommand.usDatalen = 2;
		mMyQueueData.tMyQueueCommand.cData[0] = (value >> 8) & 0x000000ff;
		mMyQueueData.tMyQueueCommand.cData[1] = value & 0x000000ff;
		xQueueSend(mEthQueueRec, (void * )&mMyQueueData, (TickType_t)2000);
		xQueueReceive(mEthQueueSend, (void * )&tMyQueueData, (TickType_t)2000);
		ESP_LOGI(TAG, "tMyQueueCommandType:%d", tMyQueueData.tMyQueueCommand.tMyQueueCommandType);
		if(tMyQueueData.tMyQueueCommand.tMyQueueCommandType == (ModbusFunctionTPDF)WriteReg)
		{
			cJSON_AddStringToObject(send_msg, "ovVolRetValue", "OK");
		}
		else cJSON_AddStringToObject(send_msg, "ovVolRetValue", "ERR");
	}

	tMyQueueData.tMyQueueCommand.tMyQueueCommandType = 0x00;
	if(cJSON_GetObjectItem(recv_property, "lkcCurActValue") != NULL)
	{
		cJSON *recv_lkcCurActValue= cJSON_GetObjectItem(recv_property, "lkcCurActValue");
		int value = recv_lkcCurActValue->valueint;
		mMyQueueData.tMyQueueCommand.tMyQueueCommandType = (ModbusFunctionTPDF)WriteReg;
		mMyQueueData.tMyQueueCommand.usSlaveAddr = mDevice[break_index].tDeviceConfig.usConvertAddr;
		mMyQueueData.tMyQueueCommand.usRegAddr = 0x010a;
		mMyQueueData.tMyQueueCommand.usRegLen = 1;
		mMyQueueData.tMyQueueCommand.usDatalen = 2;
		mMyQueueData.tMyQueueCommand.cData[0] = (value >> 8) & 0x000000ff;
		mMyQueueData.tMyQueueCommand.cData[1] = value & 0x000000ff;
		xQueueSend(mEthQueueRec, (void * )&mMyQueueData, (TickType_t)2000);
		xQueueReceive(mEthQueueSend, (void * )&tMyQueueData, (TickType_t)2000);
		ESP_LOGI(TAG, "tMyQueueCommandType:%d", tMyQueueData.tMyQueueCommand.tMyQueueCommandType);
		if(tMyQueueData.tMyQueueCommand.tMyQueueCommandType == (ModbusFunctionTPDF)WriteReg)
		{
			cJSON_AddStringToObject(send_msg, "lkcCurActValue", "OK");
		}
		else cJSON_AddStringToObject(send_msg, "lkcCurActValue", "ERR");
	}

	tMyQueueData.tMyQueueCommand.tMyQueueCommandType = 0x00;
	if(cJSON_GetObjectItem(recv_property, "lkcCurRetValue") != NULL)
	{
		cJSON *recv_lkcCurRetValue= cJSON_GetObjectItem(recv_property, "lkcCurRetValue");
		int value = recv_lkcCurRetValue->valueint;
		mMyQueueData.tMyQueueCommand.tMyQueueCommandType = (ModbusFunctionTPDF)WriteReg;
		mMyQueueData.tMyQueueCommand.usSlaveAddr = mDevice[break_index].tDeviceConfig.usConvertAddr;
		mMyQueueData.tMyQueueCommand.usRegAddr = 0x010b;
		mMyQueueData.tMyQueueCommand.usRegLen = 1;
		mMyQueueData.tMyQueueCommand.usDatalen = 2;
		mMyQueueData.tMyQueueCommand.cData[0] = (value >> 8) & 0x000000ff;
		mMyQueueData.tMyQueueCommand.cData[1] = value & 0x000000ff;
		xQueueSend(mEthQueueRec, (void * )&mMyQueueData, (TickType_t)2000);
		xQueueReceive(mEthQueueSend, (void * )&tMyQueueData, (TickType_t)2000);
		ESP_LOGI(TAG, "tMyQueueCommandType:%d", tMyQueueData.tMyQueueCommand.tMyQueueCommandType);
		if(tMyQueueData.tMyQueueCommand.tMyQueueCommandType == (ModbusFunctionTPDF)WriteReg)
		{
			cJSON_AddStringToObject(send_msg, "lkcCurRetValue", "OK");
		}
		else cJSON_AddStringToObject(send_msg, "lkcCurRetValue", "ERR");
	}
}

void ControlDeviceMCB(uint16_t break_index, cJSON *send_msg, int opr)
{
	MyQueueDataTPDF mMyQueueData;
	switch(opr)
	{
	case 0:
		if (mDevice[break_index].tDeviceConfig.usMainType == DEVICE_MAIN_TYPE_CHINT_MCB)
		{
			mMyQueueData.tMyQueueCommand.tMyQueueCommandType = (ModbusFunctionTPDF)WriteMultReg;
			mMyQueueData.tMyQueueCommand.usSlaveAddr = mDevice[break_index].tDeviceConfig.usConvertAddr;
			mMyQueueData.tMyQueueCommand.usRegAddr = 0x0000;
			mMyQueueData.tMyQueueCommand.usRegLen = 1;
			mMyQueueData.tMyQueueCommand.usDatalen = 2;
			mMyQueueData.tMyQueueCommand.cData[0] = 0x00;
			mMyQueueData.tMyQueueCommand.cData[1] = 0x02;
			xQueueSend(mEthQueueRec, (void * )&mMyQueueData, (TickType_t)0);		
			// 拉闸
			mMyQueueData.tMyQueueCommand.tMyQueueCommandType = (ModbusFunctionTPDF)WriteMultReg;
			mMyQueueData.tMyQueueCommand.usSlaveAddr = mDevice[break_index].tDeviceConfig.usConvertAddr;
			mMyQueueData.tMyQueueCommand.usRegAddr = 0x0000;
			mMyQueueData.tMyQueueCommand.usRegLen = 2;
			mMyQueueData.tMyQueueCommand.usDatalen = 4;
			mMyQueueData.tMyQueueCommand.cData[0] = 0x00;
			mMyQueueData.tMyQueueCommand.cData[1] = 0x07;
			mMyQueueData.tMyQueueCommand.cData[2] = 0x00;
			mMyQueueData.tMyQueueCommand.cData[3] = 0x00;
			xQueueSend(mEthQueueRec, (void * )&mMyQueueData, (TickType_t)0);
		}
		else if (mDevice[break_index].tDeviceConfig.usMainType == DEVICE_MAIN_TYPE_CHINT_DC_MCB)
		{
			// 拉闸
			mMyQueueData.tMyQueueCommand.tMyQueueCommandType = (ModbusFunctionTPDF)WriteMultReg;
			mMyQueueData.tMyQueueCommand.usSlaveAddr = mDevice[break_index].tDeviceConfig.usConvertAddr;
			mMyQueueData.tMyQueueCommand.usRegAddr = 0x0000;
			mMyQueueData.tMyQueueCommand.usRegLen = 2;
			mMyQueueData.tMyQueueCommand.usDatalen = 4;
			mMyQueueData.tMyQueueCommand.cData[0] = 0x00;
			mMyQueueData.tMyQueueCommand.cData[1] = 0x07;
			mMyQueueData.tMyQueueCommand.cData[2] = 0x00;
			mMyQueueData.tMyQueueCommand.cData[3] = 0x00;
			xQueueSend(mEthQueueRec, (void * )&mMyQueueData, (TickType_t)0);
		}
		break;
	case 1:
		if (mDevice[break_index].tDeviceConfig.usMainType == DEVICE_MAIN_TYPE_CHINT_MCB)
		{
			mMyQueueData.tMyQueueCommand.tMyQueueCommandType = (ModbusFunctionTPDF)WriteMultReg;
			mMyQueueData.tMyQueueCommand.usSlaveAddr = mDevice[break_index].tDeviceConfig.usConvertAddr;
			mMyQueueData.tMyQueueCommand.usRegAddr = 0x0000;
			mMyQueueData.tMyQueueCommand.usRegLen = 1;
			mMyQueueData.tMyQueueCommand.usDatalen = 2;
			mMyQueueData.tMyQueueCommand.cData[0] = 0x00;
			mMyQueueData.tMyQueueCommand.cData[1] = 0x02;
			xQueueSend(mEthQueueRec, (void * )&mMyQueueData, (TickType_t)0);		
			// 合闸
			mMyQueueData.tMyQueueCommand.tMyQueueCommandType = (ModbusFunctionTPDF)WriteMultReg;
			mMyQueueData.tMyQueueCommand.usSlaveAddr = mDevice[break_index].tDeviceConfig.usConvertAddr;
			mMyQueueData.tMyQueueCommand.usRegAddr = 0x0000;
			mMyQueueData.tMyQueueCommand.usRegLen = 2;
			mMyQueueData.tMyQueueCommand.usDatalen = 4;
			mMyQueueData.tMyQueueCommand.cData[0] = 0x00;
			mMyQueueData.tMyQueueCommand.cData[1] = 0x06;
			mMyQueueData.tMyQueueCommand.cData[2] = 0x00;
			mMyQueueData.tMyQueueCommand.cData[3] = 0x00;
			xQueueSend(mEthQueueRec, (void * )&mMyQueueData, (TickType_t)0);
		}
		else if (mDevice[break_index].tDeviceConfig.usMainType == DEVICE_MAIN_TYPE_CHINT_DC_MCB)
		{
			// 合闸
			mMyQueueData.tMyQueueCommand.tMyQueueCommandType = (ModbusFunctionTPDF)WriteMultReg;
			mMyQueueData.tMyQueueCommand.usSlaveAddr = mDevice[break_index].tDeviceConfig.usConvertAddr;
			mMyQueueData.tMyQueueCommand.usRegAddr = 0x0000;
			mMyQueueData.tMyQueueCommand.usRegLen = 2;
			mMyQueueData.tMyQueueCommand.usDatalen = 4;
			mMyQueueData.tMyQueueCommand.cData[0] = 0x00;
			mMyQueueData.tMyQueueCommand.cData[1] = 0x06;
			mMyQueueData.tMyQueueCommand.cData[2] = 0x00;
			mMyQueueData.tMyQueueCommand.cData[3] = 0x00;
			xQueueSend(mEthQueueRec, (void * )&mMyQueueData, (TickType_t)0);
		}
		break;
	default:
		break;
	}
	vTaskDelay(2000);
	
	if((mDevice[break_index].tDevicePartitionTable[0].usMemory[0]& 0b10000000) != 0 && opr == 1)
	{
		cJSON_AddStringToObject(send_msg, "status", "OK");
	}
	else if((mDevice[break_index].tDevicePartitionTable[0].usMemory[0]& 0b10000000) == 0 && opr == 0)
	{
		cJSON_AddStringToObject(send_msg, "status", "OK");
	}
	else cJSON_AddStringToObject(send_msg, "status", "err");
}

void DeviceEventMCB(uint16_t break_index, cJSON *send_json_data)
{
	cJSON *device_event = cJSON_CreateObject();
	
	if(mDevice[break_index].tDevicePartitionTable[11].usMemory[0] && 1<<6 != 0)
	{
		cJSON_AddNumberToObject(device_event, "eventNo", 1);
	}
	else if(mDevice[break_index].tDevicePartitionTable[11].usMemory[0] && 1<<3 != 0)
	{
		cJSON_AddNumberToObject(device_event, "eventNo", 2);
	}
	// else if(mDevice[i].tDevicePartitionTable[10].usMemory[0] && 1<<4 != 0)
	// {
	// 	sprintf(event_str, "%s%d//", event_str, 3);
	// }
	else if(mDevice[break_index].tDevicePartitionTable[11].usMemory[0] && 1<<4 != 0)
	{
		cJSON_AddNumberToObject(device_event, "eventNo", 4);
	}
	else if(mDevice[break_index].tDevicePartitionTable[11].usMemory[0] && 1<<0 != 0)
	{
		cJSON_AddNumberToObject(device_event, "eventNo", 5);
	}
	// else if(mDevice[i].tDevicePartitionTable[10].usMemory[0] && 1<<0 != 0)
	// {
	// 	sprintf(event_str, "%s%d//", event_str, 6);
	// }
	cJSON_AddItemToObject(send_json_data, "deviceEvent", device_event);
}

void DevicePropertyMCB(uint16_t break_index, cJSON *send_json_data, uint16_t mode)
{
	cJSON *device_property = cJSON_CreateObject();

	// int retry = 0;

	// mMyQueueData.tMyQueueCommand.tMyQueueCommandType = (ModbusFunctionTPDF)ReportSlaveID;
	// mMyQueueData.tMyQueueCommand.usSlaveAddr = mDevice[break_index].tDeviceConfig.usConvertAddr;

	// tMyQueueData.tMyQueueCommand.tMyQueueCommandType = 0x00;

	// while(tMyQueueData.tMyQueueCommand.tMyQueueCommandType != (ModbusFunctionTPDF)ReportSlaveID && retry < 10)
	// {
	// 	xQueueSend(mEthQueueRec, (void * )&mMyQueueData, (TickType_t)0);
	// 	vTaskDelay(mPartitionTable.tCom1Config.usTimeout/2);
	// 	xQueueReceive(mEthQueueSend, (void * )&tMyQueueData, (TickType_t)0);
	// 	retry ++;
	// }

	// cJSON_AddNumberToObject(device_property, "factoryID", 1);
	// cJSON_AddNumberToObject(device_property, "Class", 4);
	// cJSON_AddNumberToObject(device_property, "subClass", 3);
	// cJSON_AddNumberToObject(device_property, "model", 1);
	if (mDevice[break_index].tDeviceConfig.onlineState == 0)
	{
		cJSON_AddNumberToObject(device_property, "onlineState", 0);
		cJSON_AddItemToObject(send_json_data, "deviceProperty", device_property);
		return;
	}
	

	
	if((mDevice[break_index].tDevicePartitionTable[0].usMemory[0] & 0x1<<7) != 0)
	{
		cJSON_AddNumberToObject(device_property, "swtichState", 1);
	}
	else
	{
		cJSON_AddNumberToObject(device_property, "swtichState", 0);
	}
	
	if((mDevice[break_index].tDevicePartitionTable[0].usMemory[0] & 0x1<<11) != 0)
	{
		cJSON_AddNumberToObject(device_property, "controlState", 1);
	}
	else
	{
		cJSON_AddNumberToObject(device_property, "controlState", 0); 
	}

	if (mode == 2)
	{
		char temp_str[34] = {0};		

		if((mDevice[break_index].tDevicePartitionTable[0].usMemory[0] & 0x1<<2) != 0)
		{
			cJSON_AddNumberToObject(device_property, "voltageState", 1);
		}
		else if((mDevice[break_index].tDevicePartitionTable[0].usMemory[0] & 0x1<<3) != 0)
		{
			cJSON_AddNumberToObject(device_property, "voltageState", 2);
		}
		else
		{
			cJSON_AddNumberToObject(device_property, "voltageState", 0);
		}

		// 电流
		float current_A = mDevice[break_index].tDevicePartitionTable[1].usMemory[0];
		sprintf(temp_str, "%.2f", current_A/100);
		cJSON_AddStringToObject(device_property, "current", temp_str);
		memset(temp_str, 0, sizeof(temp_str));

		// 电压
		float voltage_A = mDevice[break_index].tDevicePartitionTable[2].usMemory[0];
		sprintf(temp_str, "%.1f", voltage_A/10);
		cJSON_AddStringToObject(device_property, "voltage", temp_str);
		memset(temp_str, 0, sizeof(temp_str));

		// 频率
		float frequency = mDevice[break_index].tDevicePartitionTable[3].usMemory[1]/100;
		cJSON_AddNumberToObject(device_property, "freq", frequency);

		// 功率因素
		int16_t the_power_factor = mDevice[break_index].tDevicePartitionTable[3].usMemory[0];
		cJSON_AddNumberToObject(device_property, "pF", the_power_factor);

		// leakage_current
		uint16_t leakage_current = mDevice[break_index].tDevicePartitionTable[3].usMemory[2];
		cJSON_AddNumberToObject(device_property, "leakCurrent", leakage_current);

		// 有功功率
		int32_t active_power = (mDevice[break_index].tDevicePartitionTable[4].usMemory[1]) + (mDevice[break_index].tDevicePartitionTable[4].usMemory[0]<<16);
		cJSON_AddNumberToObject(device_property, "actPower", active_power);
		
		// 无功功率
		int32_t reactive_power = (mDevice[break_index].tDevicePartitionTable[5].usMemory[1]) + (mDevice[break_index].tDevicePartitionTable[5].usMemory[0]<<16);
		cJSON_AddNumberToObject(device_property, "reactPower", reactive_power);

		// apparent power
		int32_t apparent_power = (mDevice[break_index].tDevicePartitionTable[6].usMemory[1]) + (mDevice[break_index].tDevicePartitionTable[6].usMemory[0]<<16);
		cJSON_AddNumberToObject(device_property, "apparentPower", apparent_power);

		// active energy
		int32_t active_energy = (mDevice[break_index].tDevicePartitionTable[4].usMemory[3]) + (mDevice[break_index].tDevicePartitionTable[4].usMemory[2]<<16);
		cJSON_AddNumberToObject(device_property, "actEnergy", active_energy);

		// reactive energy
		int32_t reactive_energy = (mDevice[break_index].tDevicePartitionTable[4].usMemory[5]) + (mDevice[break_index].tDevicePartitionTable[4].usMemory[4]<<16);
		cJSON_AddNumberToObject(device_property, "reactEnergy", reactive_energy);

		// apparent energy
		int32_t apparent_energy = (mDevice[break_index].tDevicePartitionTable[4].usMemory[7]) + (mDevice[break_index].tDevicePartitionTable[4].usMemory[6]<<16);
		cJSON_AddNumberToObject(device_property, "apparentEnergy", apparent_energy);



		// last off reason
		int16_t lastOFFReason = mDevice[break_index].tDevicePartitionTable[11].usMemory[0];
		cJSON_AddNumberToObject(device_property, "lastOFFReason", lastOFFReason);



		// temperature
		uint16_t temper = mDevice[break_index].tDevicePartitionTable[7].usMemory[0];
		cJSON_AddNumberToObject(device_property, "temper", temper);


		
		uint16_t afund = mDevice[break_index].tDevicePartitionTable[12].usMemory[0];
		cJSON_AddNumberToObject(device_property, "afund", afund);
		
		uint16_t tHDi = mDevice[break_index].tDevicePartitionTable[12].usMemory[1];
		cJSON_AddNumberToObject(device_property, "tHDi", tHDi);

		uint16_t vfund = mDevice[break_index].tDevicePartitionTable[13].usMemory[0];
		cJSON_AddNumberToObject(device_property, "vfund", vfund);
		
		uint16_t tHDu = mDevice[break_index].tDevicePartitionTable[13].usMemory[1];
		cJSON_AddNumberToObject(device_property, "tHDu", tHDu);


		cJSON_AddNumberToObject(device_property, "onlineState", mDevice[break_index].tDeviceConfig.onlineState);

		// event enable
		int16_t event_status = mDevice[break_index].tDevicePartitionTable[10].usMemory[0];
		cJSON_AddNumberToObject(device_property, "eventEnable", event_status);



		// modbus addr
		int16_t modbus_addr = mDevice[break_index].tDevicePartitionTable[8].usMemory[0];
		cJSON_AddNumberToObject(device_property, "modbusAddr", modbus_addr);

		// modbus bandrate
		int16_t modbus_bandrate = mDevice[break_index].tDevicePartitionTable[8].usMemory[1];
		cJSON_AddNumberToObject(device_property, "bandRate", modbus_bandrate);

		// modbus parity
		int16_t modbus_parity = mDevice[break_index].tDevicePartitionTable[8].usMemory[2];
		cJSON_AddNumberToObject(device_property, "parity", modbus_parity);
		


		int16_t unVolActValue = mDevice[break_index].tDevicePartitionTable[9].usMemory[0];
		cJSON_AddNumberToObject(device_property, "unVolActValue", unVolActValue);

		int16_t unVolActTime = mDevice[break_index].tDevicePartitionTable[9].usMemory[1];
		cJSON_AddNumberToObject(device_property, "unVolActTime", unVolActTime);

		int16_t unVolRetValue = mDevice[break_index].tDevicePartitionTable[9].usMemory[2];
		cJSON_AddNumberToObject(device_property, "unVolRetValue", unVolRetValue);

		int16_t recoveryTime = mDevice[break_index].tDevicePartitionTable[9].usMemory[3];
		cJSON_AddNumberToObject(device_property, "recoveryTime", recoveryTime);

		int16_t ovVolActValue = mDevice[break_index].tDevicePartitionTable[9].usMemory[4];
		cJSON_AddNumberToObject(device_property, "ovVolActValue", ovVolActValue);

		int16_t ovVolActTime = mDevice[break_index].tDevicePartitionTable[9].usMemory[5];
		cJSON_AddNumberToObject(device_property, "ovVolActTime", ovVolActTime);
		
		int16_t ovVolRetValue = mDevice[break_index].tDevicePartitionTable[9].usMemory[6];
		cJSON_AddNumberToObject(device_property, "ovVolRetValue", ovVolRetValue);

		int16_t lkcCurActValue = mDevice[break_index].tDevicePartitionTable[9].usMemory[7];
		cJSON_AddNumberToObject(device_property, "lkcCurActValue", lkcCurActValue);

		int16_t lkcCurRetValue = mDevice[break_index].tDevicePartitionTable[9].usMemory[8];
		cJSON_AddNumberToObject(device_property, "lkcCurRetValue", lkcCurRetValue);
	}

	cJSON_AddItemToObject(send_json_data, "deviceProperty", device_property);
}
