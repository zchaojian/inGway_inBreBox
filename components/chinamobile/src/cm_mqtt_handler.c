/**********************************************************************************
  *Copyright(C),2010-2011,CHNT
  *@FileName: eth_mqtt_handler.c
  *@Author:   ElonStark
  *@Version:  V1.0
  *@Date:     2020.11.13
  *@Description: 用于处理来自Chnia Mobile MQTT的数据
  *@Others:   None
  *@Function List:   
  *  void MQTTEventDataHandler(esp_mqtt_event_handle_t event);
  *  void GetLocalTime(char * time_str);
  *  void MqttEventConnectedManage(esp_mqtt_event_handle_t event);
  *History:   
  *   1.Date:
  *    Author:
  *    Modification:
**********************************************************************************/

#include "cm_mqtt_handler.h"

#include "nvs_flash.h"
#include "eth_tsk.h"
#include "mqtt_mcb_handler.h"

#include "config_app.h"
#include "driver/timer.h"

#include "../../../main/main.h"
#include "https_ota.h"

#define TAG "MqttHandler"

static esp_mqtt_client_handle_t CMClient;

static char up_topic[60] = {0};
static char down_topic[60] = {0};

char *upgrade_url;

void CMEventDataHandler(esp_mqtt_event_handle_t event);
void TimerReportMessage();

static int64_t GetTime()
{
	struct timeval tv_now;
	gettimeofday(&tv_now, NULL);
	volatile int64_t time_ms = (int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec;

	return time_ms/1000;
}


static void GetBreakerState()
{
	ESP_LOGI(TAG, "GetBreakerState");
	MyQueueDataTPDF mMyQueueData;
	for (size_t i = 0; i < CONFIG_DEVICE_MAXSURPPORT; i++)
	{
		if(mPartitionTable.tDeviceConfig[i].usConvertAddr == 0) break;
		mMyQueueData.tMyQueueCommand.tMyQueueCommandType = (ModbusFunctionTPDF)WriteReg;
		mMyQueueData.tMyQueueCommand.usSlaveAddr = mDevice[i].tDeviceConfig.usConvertAddr;
		mMyQueueData.tMyQueueCommand.usRegAddr = 0x0000;
		mMyQueueData.tMyQueueCommand.usRegLen = 1;
		mMyQueueData.tMyQueueCommand.usDatalen = 2;
		mMyQueueData.tMyQueueCommand.cData[0] = 0x00;
		mMyQueueData.tMyQueueCommand.cData[1] = 0x00;
		xQueueSend(mEthQueueRec, (void * )&mMyQueueData, (TickType_t)2000);
	}
}


void CMConnectedManage(esp_mqtt_event_handle_t event)
{
	for (size_t i = 0; i < 16; i++)
	{
		if(mPartitionTable.tDeviceConfig[i].usConvertAddr == 0) break;
		mDevice[i].tDeviceConfig.usMainType = mPartitionTable.tDeviceConfig[i].usMainType;
		mDevice[i].tDeviceConfig.usCommPort = mPartitionTable.tDeviceConfig[i].usCommPort;
		mDevice[i].tDeviceConfig.usConvertAddr = mPartitionTable.tDeviceConfig[i].usConvertAddr;
		DEV_vCreat_PartitionTable_From_Example(mDevice[i].tDeviceConfig.usMainType, mDevice[i].tDevicePartitionTable);
		ESP_LOGI(TAG, "id:%d, sn:%s", i, mPartitionTable.DeviceAssetCode[i].AssetCode);
	}
	
	mPartitionTable.usGatewayRunMode = GATEWAY_MODE_TRANSMIT;

	if(mPartitionTable.up_period != 0)
	{
		ESP_LOGI(TAG, "up_period:%d", mPartitionTable.up_period);
		uint16_t para[]={1, 5};
		ESP_ERROR_CHECK(esp_event_post_to(event_loop_task, TASK_EVENTS, CREATE_TIMER_TASK, para, sizeof(para), portMAX_DELAY));
	}
	GetBreakerState();
	CMClient = event->client;
	strcpy(up_topic, "gateway/report/mcb/1234");
	strcpy(down_topic, "gateway/receive/mcb/1234");
	cJSON *send_json_data = cJSON_CreateObject();
	cJSON_AddStringToObject(send_json_data, "code", CODE_GET_GATEWAY_CONFIG);
	cJSON_AddNumberToObject(send_json_data, "time", GetTime());

	char *send_data = cJSON_Print(send_json_data);
	ESP_LOGI(TAG, "send data:%s", send_data);
	esp_mqtt_client_subscribe(CMClient, down_topic, mPartitionTable.PlatformConfig.MQTTQos);
	esp_mqtt_client_publish(CMClient, up_topic, send_data, strlen(send_data), mPartitionTable.PlatformConfig.MQTTQos, 0);
	cJSON_free(send_data);
	cJSON_Delete(send_json_data);
}

static void ReportDeviceEvent()
{
	ESP_LOGI(TAG, "ReportDeviceEvent");
	for (size_t i = 0; i < CONFIG_DEVICE_MAXSURPPORT; i++)
	{
		if(mDevice[i].tDeviceConfig.usConvertAddr == 0) break;
		cJSON *send_json_data = cJSON_CreateObject();
		cJSON_AddStringToObject(send_json_data, "code", CODE_REPORT_DEVICE_EVENT);
		cJSON_AddNumberToObject(send_json_data, "time", GetTime());
		cJSON_AddStringToObject(send_json_data, "sn", mPartitionTable.DeviceAssetCode[i].AssetCode);
		switch (mDevice[i].tDeviceConfig.usMainType)
		{
		case DEVICE_MAIN_TYPE_ACB:
			/* code */
			break;
		case DEVICE_MAIN_TYPE_MCCB:
			/* code */
			break;
		case DEVICE_MAIN_TYPE_MCB:
			/* code */
			break;
		case DEVICE_MAIN_TYPE_ELCB:
			/* code */
			break;
		case DEVICE_MAIN_TYPE_TEMPRATURE:
			/* code */
			break;
		case DEVICE_MAIN_TYPE_CHINT_METER_MCB:
			/* code */
			break;
		case DEVICE_MAIN_TYPE_ROUTER:
			/* code */
			break;
		case DEVICE_MAIN_TYPE_CHINT_MCB:
			DeviceEventMCB(i, send_json_data);
			break;
		case DEVICE_MAIN_TYPE_CHINT_DC_MCB:
			break;
		default:
			break;
		}
		char *send_data = cJSON_Print(send_json_data);
		ESP_LOGI(TAG, "send data:%s", send_data);
		esp_mqtt_client_publish(CMClient, up_topic, send_data, strlen(send_data), mPartitionTable.PlatformConfig.MQTTQos, 0);
		cJSON_free(send_data);
		cJSON_Delete(send_json_data);
	}
}

static void ReportDeviceProperty(uint16_t mode)
{
	ESP_LOGI(TAG, "ReportDeviceProperty");
	for (size_t i = 0; i < CONFIG_DEVICE_MAXSURPPORT; i++)
	{
		if(mDevice[i].tDeviceConfig.usConvertAddr == 0) break;
		cJSON *send_json_data = cJSON_CreateObject();
		cJSON_AddStringToObject(send_json_data, "code", CODE_REPORT_DEVICE_PROPERTY);
		cJSON_AddNumberToObject(send_json_data, "time", GetTime());
		cJSON_AddStringToObject(send_json_data, "sn", mPartitionTable.DeviceAssetCode[i].AssetCode);
		switch (mDevice[i].tDeviceConfig.usMainType)
		{
		case DEVICE_MAIN_TYPE_ACB:
			/* code */
			break;
		case DEVICE_MAIN_TYPE_MCCB:
			/* code */
			break;
		case DEVICE_MAIN_TYPE_MCB:
			/* code */
			break;
		case DEVICE_MAIN_TYPE_ELCB:
			/* code */
			break;
		case DEVICE_MAIN_TYPE_TEMPRATURE:
			/* code */
			break;
		case DEVICE_MAIN_TYPE_CHINT_METER_MCB:
			/* code */
			break;
		case DEVICE_MAIN_TYPE_ROUTER:
			/* code */
			break;
		case DEVICE_MAIN_TYPE_CHINT_MCB:
			DevicePropertyMCB(i, send_json_data, mode);
			break;
		case DEVICE_MAIN_TYPE_CHINT_DC_MCB:
			break;
		default:
			break;
		}
		char *send_data = cJSON_Print(send_json_data);
		ESP_LOGI(TAG, "send data:%s", send_data);
		esp_mqtt_client_publish(CMClient, up_topic, send_data, strlen(send_data), mPartitionTable.PlatformConfig.MQTTQos, 0);
		cJSON_free(send_data);
		cJSON_Delete(send_json_data);
	}
}

// static void ReportGatewayEvent()
// {
// 	ESP_LOGI(TAG, "ReportGatewayEvent");
// 	cJSON *send_json_data = cJSON_CreateObject();
// 	cJSON_AddStringToObject(send_json_data, "code", CODE_REPORT_GATEWAY_EVENT);
// 	cJSON_AddNumberToObject(send_json_data, "time", GetTime());
// 	cJSON_AddStringToObject(send_json_data, "gatewayEvent", "gateway event");
// 	char *send_data = cJSON_Print(send_json_data);
// 	ESP_LOGI(TAG, "send data:%s", send_data);
// 	esp_mqtt_client_publish(CMClient, up_topic, send_data, strlen(send_data), mPartitionTable.PlatformConfig.MQTTQos, 0);
// 	cJSON_free(send_data);
// 	cJSON_Delete(send_json_data);
// }

static void ReportGatewayProperty()
{
	ESP_LOGI(TAG, "ReportGatewayProperty");
	cJSON *send_json_data = cJSON_CreateObject();
	cJSON *send_msg = cJSON_CreateObject();
	cJSON_AddStringToObject(send_json_data, "code", CODE_REPORT_GATEWAY_PROPERTY);
	cJSON_AddNumberToObject(send_json_data, "time", GetTime());
	cJSON_AddStringToObject(send_msg, "type", "Gateway");
	cJSON_AddStringToObject(send_msg, "sn", (char *)mPartitionTable.GatewayInfor.AssetCode);
	cJSON_AddStringToObject(send_msg, "softwareVersion", "V1.0");
	cJSON_AddStringToObject(send_msg, "hardwareVersion", "V1.0");
	cJSON_AddNumberToObject(send_msg, "upPeriod", mPartitionTable.up_period);
	cJSON_AddItemToObject(send_json_data, "gatewayProperty", send_msg);
	char *send_data = cJSON_Print(send_json_data);
	ESP_LOGI(TAG, "send data:%s", send_data);
	esp_mqtt_client_publish(CMClient, up_topic, send_data, strlen(send_data), mPartitionTable.PlatformConfig.MQTTQos, 0);
	cJSON_free(send_data);
	cJSON_Delete(send_json_data);
}

// static void ReportGatewayInfo()
// {
// 	ESP_LOGI(TAG, "ReportGatewayInfo");
// 	cJSON *send_json_data = cJSON_CreateObject();
// 	cJSON_AddStringToObject(send_json_data, "code", CODE_REPORT_GATEWAY_INFO);
// 	cJSON_AddNumberToObject(send_json_data, "time", GetTime());
// 	cJSON_AddStringToObject(send_json_data, "gatewayInfo", "gateway information");
// 	char *send_data = cJSON_Print(send_json_data);
// 	ESP_LOGI(TAG, "send data:%s", send_data);
// 	esp_mqtt_client_publish(CMClient, up_topic, send_data, strlen(send_data), mPartitionTable.PlatformConfig.MQTTQos, 0);
// 	cJSON_free(send_data);
// 	cJSON_Delete(send_json_data);
// }

void TimerReportMessage()
{
	static int timer_count = 1;
	
	if((timer_count * 5) % (mPartitionTable.up_period*2) == 0)
	{
		GetBreakerState();
		ReportDeviceEvent();
		ReportDeviceProperty(2);
		// ReportGatewayEvent();
		ReportGatewayProperty();
		// ReportGatewayInfo();
		timer_count = 1;
		return;
	}
	else if((timer_count * 5) % mPartitionTable.up_period == 0)
	{
		ReportDeviceEvent();
		ReportDeviceProperty(2);
		timer_count ++;
		return;
	}
	else
	{
		ReportDeviceProperty(1);
		timer_count ++;
	}
}


static void SetDeviceHandler(cJSON *recv_json_data)
{
	cJSON *send_json_data = cJSON_CreateObject();
	cJSON *send_msg  = cJSON_CreateObject();

	cJSON *recv_sn = cJSON_GetObjectItem(recv_json_data, "sn");
	cJSON *recv_id = cJSON_GetObjectItem(recv_json_data, "messageId");
	cJSON *recv_property = cJSON_GetObjectItem(recv_json_data, "deviceProperty");


	uint16_t break_index = -1;
	for(int i=0; i<CONFIG_DEVICE_MAXSURPPORT; i++)
	{
		if (mDevice[i].tDeviceConfig.usConvertAddr == 0) break;
		if(strcmp(recv_sn->valuestring, mPartitionTable.DeviceAssetCode[i].AssetCode) == 0)
		{
			break_index = i;
			break;
		}
	}
	switch (mDevice[break_index].tDeviceConfig.usMainType)
	{
	case DEVICE_MAIN_TYPE_ACB:
		/* code */
		break;
	case DEVICE_MAIN_TYPE_MCCB:
		/* code */
		break;
	case DEVICE_MAIN_TYPE_MCB:
		/* code */
		break;
	case DEVICE_MAIN_TYPE_ELCB:
		/* code */
		break;
	case DEVICE_MAIN_TYPE_TEMPRATURE:
		/* code */
		break;
	case DEVICE_MAIN_TYPE_CHINT_METER_MCB:
		/* code */
		break;
	case DEVICE_MAIN_TYPE_ROUTER:
		/* code */
		break;
	case DEVICE_MAIN_TYPE_CHINT_MCB:
		ConfigDeviceMCB(break_index, send_msg, recv_property);
		break;
	case DEVICE_MAIN_TYPE_CHINT_DC_MCB:
		break;
	default:
		break;
	}

	cJSON_AddStringToObject(send_json_data, "code", CODE_SET_DEVICE_CALLBACK);
	cJSON_AddStringToObject(send_json_data, "messageId", recv_id->valuestring);
	cJSON_AddNumberToObject(send_json_data, "time", GetTime());
	cJSON_AddItemToObject(send_json_data, "callbackDevice", send_msg);
	char *send_data = cJSON_Print(send_json_data);
	ESP_LOGI(TAG, "send data:%s", send_data);
	esp_mqtt_client_publish(CMClient, up_topic, send_data, strlen(send_data), mPartitionTable.PlatformConfig.MQTTQos, 0);
	cJSON_free(send_data);
	cJSON_Delete(send_json_data);
}

static void SetGatewayHandler(cJSON *recv_json_data)
{
	while(mPartitionTable.usGatewayRunMode == GATEWAY_MODE_TRANSMIT)
	{
		mPartitionTable.usGatewayRunMode = GATEWAY_MODE_PASSTHROUGH;
		vTaskDelay(100);
	}
	MyQueueDataTPDF tMyQueueData;
	MyQueueDataTPDF mMyQueueData;

	uint16_t retry = 0;
	uint32_t uiIndex;
	uint32_t uiSize;

	cJSON *send_json_data = cJSON_CreateObject();
	cJSON *send_msg  = cJSON_CreateObject();
	cJSON *jsonArray = cJSON_CreateArray();

	cJSON *recv_id = cJSON_GetObjectItem(recv_json_data, "messageId");
	cJSON *recv_property = cJSON_GetObjectItem(recv_json_data, "gatewayProperty");
	
	cJSON_AddStringToObject(send_json_data, "code", CODE_SET_GATEWAY_CALLBACK);
	cJSON_AddStringToObject(send_json_data, "messageId", recv_id->valuestring);
	cJSON_AddNumberToObject(send_json_data, "time", GetTime());

	if(cJSON_GetObjectItem(recv_property, "deviceList") != NULL)
	{
		cJSON *recv_device_list = cJSON_GetObjectItem(recv_property, "deviceList");
		memset(mDevice, 0, sizeof(mDevice));
		memset(mPartitionTable.DeviceAssetCode, 0, sizeof(mPartitionTable.DeviceAssetCode));
		uint16_t breaker_number = cJSON_GetArraySize(recv_device_list);
		uint16_t tmp_modbus_address = 0x04;
		for(int i=0; i<breaker_number; i++){
			cJSON *item = cJSON_GetArrayItem(recv_device_list, i);
			cJSON *recv_sn = cJSON_GetObjectItem(item, "sn");
			ESP_LOGI(TAG, "sn=%s\n", recv_sn->valuestring);
			// mDevice field == item->valuestring
			strcpy(mPartitionTable.DeviceAssetCode[i].AssetCode, recv_sn->valuestring);
			mMyQueueData.tMyQueueCommand.tMyQueueCommandType = (ModbusFunctionTPDF)ReportSlaveID;
			mMyQueueData.tMyQueueCommand.usSlaveAddr = 0;
			mMyQueueData.tMyQueueCommand.usRegAddr = 0x0000;
			mMyQueueData.tMyQueueCommand.usRegLen = 10;
			mMyQueueData.tMyQueueCommand.usDatalen = 20;
			mMyQueueData.tMyQueueCommand.cData[0] = 0x00;
			mMyQueueData.tMyQueueCommand.cData[1] = 0x0d;
			for(int j=0; j<strlen(recv_sn->valuestring); j++)
			{
				mMyQueueData.tMyQueueCommand.cData[2+j] = (char)(recv_sn->valuestring[j]);
			}
			mMyQueueData.tMyQueueCommand.cData[18] = 0x00;
			mMyQueueData.tMyQueueCommand.cData[19] = tmp_modbus_address;

			tMyQueueData.tMyQueueCommand.tMyQueueCommandType = 0x00;

			while (tMyQueueData.tMyQueueCommand.tMyQueueCommandType != 0x10 && retry < 10)
			{
				xQueueSend(mEthQueueRec, (void * )&mMyQueueData, (TickType_t)2000);
				xQueueReceive(mEthQueueSend, (void * )&tMyQueueData, (TickType_t)2000);
				retry ++;
			}

			if (retry >= 10)
			{
				retry = 0;
				cJSON *ArrayItem = cJSON_CreateObject();
				cJSON_AddStringToObject(ArrayItem, "sn", recv_sn->valuestring);
				cJSON_AddItemToArray(jsonArray, ArrayItem);
				memset(mPartitionTable.DeviceAssetCode[i].AssetCode, 0, sizeof(mPartitionTable.DeviceAssetCode[i].AssetCode));
				mPartitionTable.tDeviceConfig[i].usConvertAddr = 0x00;
				mPartitionTable.tDeviceConfig[i].usCommPort = 0;
				mDevice[i].tDeviceConfig.usConvertAddr = 0x00;
				mDevice[i].tDeviceConfig.usCommPort = 0;
				mDevice[i].tDeviceConfig.usMainType = 0;
				mPartitionTable.tDeviceConfig[i].usMainType = 0;
				continue;
			}

			mPartitionTable.tDeviceConfig[i].usConvertAddr = tmp_modbus_address;
			mPartitionTable.tDeviceConfig[i].usCommPort = DEVICE_COMMPORT_TYPE_COM1;
			mDevice[i].tDeviceConfig.usConvertAddr = tmp_modbus_address;
			mDevice[i].tDeviceConfig.usCommPort = DEVICE_COMMPORT_TYPE_COM1;
			mDevice[i].tDeviceConfig.usMainType = DEVICE_MAIN_TYPE_CHINT_MCB;
			mPartitionTable.tDeviceConfig[i].usMainType = DEVICE_MAIN_TYPE_CHINT_MCB;
			DEV_vCreat_PartitionTable_From_Example(mDevice[i].tDeviceConfig.usMainType, mDevice[i].tDevicePartitionTable);
			tmp_modbus_address ++;
		}
		cJSON_AddItemToObject(send_msg, "deviceList", jsonArray);
		// save device config
		uiIndex = (((char *)&mPartitionTable.tDeviceConfig) - ((char *)&mPartitionTable)) >> 1;
		uiSize = sizeof(DeviceConfigTPDF) >> 1;
		while(uiSize)
		{
			CFG_vSaveConfig(uiIndex++);
			uiSize--;
		}

		// save device asset code
		uiIndex = (((char *)&mPartitionTable.DeviceAssetCode) - ((char *)&mPartitionTable)) >> 1;
		uiSize = sizeof(DeviceAssetCodeTPDF) >> 1;
		while(uiSize)
		{
			CFG_vSaveConfig(uiIndex++);
			uiSize--;
		}
	}

	while(mPartitionTable.usGatewayRunMode == GATEWAY_MODE_PASSTHROUGH)
	{
		mPartitionTable.usGatewayRunMode = GATEWAY_MODE_TRANSMIT;
		vTaskDelay(3000);
	}

	if(cJSON_GetObjectItem(recv_property, "upPeriod") != NULL)
	{
		cJSON *recv_up_period = cJSON_GetObjectItem(recv_property, "upPeriod");
		mPartitionTable.up_period = recv_up_period->valueint;
		uint16_t para[]={1, 5};
		cJSON_AddStringToObject(send_msg, "upPeriod", "OK");
		uiIndex = (((char *)&mPartitionTable.up_period) - ((char *)&mPartitionTable)) >> 1;
		CFG_vSaveConfig(uiIndex++);
		ESP_ERROR_CHECK(esp_event_post_to(event_loop_task, TASK_EVENTS, CREATE_TIMER_TASK, para, sizeof(para), portMAX_DELAY));
	}

	cJSON_AddItemToObject(send_json_data, "callbackGateway", send_msg);
	char *send_data = cJSON_Print(send_json_data);
	ESP_LOGI(TAG, "send data:%s", send_data);
	esp_mqtt_client_publish(CMClient, up_topic, send_data, strlen(send_data), mPartitionTable.PlatformConfig.MQTTQos, 0);
	cJSON_free(send_data);

	cJSON_Delete(send_json_data);

	ReportDeviceProperty(1);
}

static void ControlDeviceHandler(cJSON *recv_json_data)
{
	cJSON *send_json_data = cJSON_CreateObject();
	cJSON *send_msg  = cJSON_CreateObject();

	cJSON *recv_id = cJSON_GetObjectItem(recv_json_data, "messageId");
	cJSON *recv_sn = cJSON_GetObjectItem(recv_json_data, "sn");
	cJSON *recv_action = cJSON_GetObjectItem(recv_json_data, "action");

	uint16_t break_index = -1;
	for(int i=0; i<CONFIG_DEVICE_MAXSURPPORT; i++)
	{
		if (mDevice[i].tDeviceConfig.usConvertAddr == 0) break;
		if(strcmp(recv_sn->valuestring, mPartitionTable.DeviceAssetCode[i].AssetCode) == 0)
		{
			break_index = i;
			break;
		}
	}
	switch (mDevice[break_index].tDeviceConfig.usMainType)
	{
	case DEVICE_MAIN_TYPE_ACB:
		/* code */
		break;
	case DEVICE_MAIN_TYPE_MCCB:
		/* code */
		break;
	case DEVICE_MAIN_TYPE_MCB:
		/* code */
		break;
	case DEVICE_MAIN_TYPE_ELCB:
		/* code */
		break;
	case DEVICE_MAIN_TYPE_TEMPRATURE:
		/* code */
		break;
	case DEVICE_MAIN_TYPE_CHINT_METER_MCB:
		/* code */
		break;
	case DEVICE_MAIN_TYPE_ROUTER:
		/* code */
		break;
	case DEVICE_MAIN_TYPE_CHINT_MCB:
		ControlDeviceMCB(break_index, send_msg, recv_action->valueint);
		break;
	case DEVICE_MAIN_TYPE_CHINT_DC_MCB:
		break;
	default:
		break;
	}
	cJSON_AddStringToObject(send_json_data, "code", CODE_CONTROL_DEVICE_CALLBACK);
	cJSON_AddStringToObject(send_json_data, "messageId", recv_id->valuestring);
	cJSON_AddNumberToObject(send_json_data, "time", GetTime());
	cJSON_AddStringToObject(send_json_data, "sn", recv_sn->valuestring);
	cJSON_AddItemToObject(send_json_data, "callbackControl", send_msg);
	char *send_data = cJSON_Print(send_json_data);
	ESP_LOGI(TAG, "send data:%s", send_data);
	esp_mqtt_client_publish(CMClient, up_topic, send_data, strlen(send_data), mPartitionTable.PlatformConfig.MQTTQos, 0);
	cJSON_free(send_data);
	cJSON_Delete(send_json_data);
}


static void GetGatewayLogHandler(cJSON *recv_json_data)
{
	cJSON *send_json_data = cJSON_CreateObject();

	cJSON *recv_id = cJSON_GetObjectItem(recv_json_data, "messageId");

	cJSON_AddStringToObject(send_json_data, "code", CODE_GET_GATEWAY_LOG_CALLBACK);
	cJSON_AddStringToObject(send_json_data, "messageId", recv_id->valuestring);
	cJSON_AddNumberToObject(send_json_data, "time", GetTime());
	cJSON_AddStringToObject(send_json_data, "callbackGatewayLog", "log message");
	char *send_data = cJSON_Print(send_json_data);
	ESP_LOGI(TAG, "send data:%s", send_data);
	esp_mqtt_client_publish(CMClient, up_topic, send_data, strlen(send_data), mPartitionTable.PlatformConfig.MQTTQos, 0);
	cJSON_free(send_data);
	cJSON_Delete(send_json_data);
}

static void OTAHandler(cJSON *recv_json_data)
{
	cJSON *send_json_data = cJSON_CreateObject();
	cJSON *send_msg  = cJSON_CreateObject();

	cJSON *recv_ota_data = cJSON_GetObjectItem(recv_json_data, "otaData");
	cJSON *recv_device_type = cJSON_GetObjectItem(recv_ota_data, "deviceType");
	cJSON *recv_file_url = cJSON_GetObjectItem(recv_ota_data, "fileUrl");
	// cJSON *recv_sn_list = cJSON_GetObjectItem(recv_ota_data, "snList");

	switch (recv_device_type->valueint)
	{
	case 0:
		ESP_LOGI(TAG, "Gateway upgrade");
		upgrade_url = malloc(strlen(recv_file_url->valuestring)+2);
		memset(upgrade_url, 0, strlen(recv_file_url->valuestring)+2);
		memcpy(upgrade_url, recv_file_url->valuestring, strlen(recv_file_url->valuestring));
		ESP_LOGI(TAG, "upgrade url:%s", upgrade_url);
		ESP_ERROR_CHECK(esp_event_post_to(event_loop_task, TASK_EVENTS, CREATE_OTA_TASK, NULL, (size_t)0, portMAX_DELAY));
		break;
	case 1:
		ESP_LOGI(TAG, "Breaker upgrade");
	default:
		break;
	}
	
	cJSON_AddStringToObject(send_json_data, "code", CODE_OTA_CALLBACK);
	cJSON_AddNumberToObject(send_json_data, "time", GetTime());
	cJSON_AddItemToObject(send_json_data, "callbackOTA", send_msg);
	char *send_data = cJSON_Print(send_json_data);
	ESP_LOGI(TAG, "send data:%s", send_data);
	esp_mqtt_client_publish(CMClient, up_topic, send_data, strlen(send_data), mPartitionTable.PlatformConfig.MQTTQos, 0);
	cJSON_free(send_data);
	cJSON_Delete(send_json_data);
}


void CMEventDataHandler(esp_mqtt_event_handle_t event)
{
	CMClient = event->client;

	cJSON *recv_json_data = cJSON_Parse(event->data);
	if(cJSON_GetObjectItem(recv_json_data, "code") == NULL)
	{
		cJSON_Delete(recv_json_data);
		return;
	}
	cJSON *code = cJSON_GetObjectItem(recv_json_data, "code");

	if(strcmp(code->valuestring, CODE_SET_DEVICE) == 0)
	{
		SetDeviceHandler(recv_json_data);
	}
	else if(strcmp(code->valuestring, CODE_SET_GATEWAY) == 0)
	{
		SetGatewayHandler(recv_json_data);
	}
	else if(strcmp(code->valuestring, CODE_CONTROL_DEVICE) == 0)
	{
		ControlDeviceHandler(recv_json_data);
	}
	else if(strcmp(code->valuestring, CODE_GET_GATEWAY_LOG) == 0)
	{
		GetGatewayLogHandler(recv_json_data);
	}
	else if(strcmp(code->valuestring, CODE_OTA) == 0)
	{
		OTAHandler(recv_json_data);
	}

	cJSON_Delete(recv_json_data);
}