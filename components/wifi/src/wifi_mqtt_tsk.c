/*
 * wifi_mqtt_tsk.c
 *
 *  Created on: Nov 12, 2020
 *      Author: zchaojian
 */
#include "wifi_tsk.h"
#include "wifi_mqtt_tsk.h"

#include "cm_mqtt_handler.h"

static MqttPublishDataTPDF *mMqttPublishData;
static uint16_t mMqttPublishDataCount;
static uint8_t mQttConnected = 0;
static esp_mqtt_client_handle_t mClient;
static MyQueueDataTPDF mMyQueueData;
// china_mobile_client
static esp_mqtt_client_handle_t china_mobile_client;

static const char *TAG = "wifi mqtt task";

static void vMySprintf_Hex(char *source, uint32_t data, uint8_t len)
{
	uint8_t j;
	for(uint8_t i = 0; i < len; i++)
	{
		j = data & 0x0000000f;
		if(j < 10)
		{
			source[len - 1 - i] = j + 0x30;
		}
		else
		{
			source[len - 1 - i] = j - 10 + 'A';
		}
		data >>= 4;
	}
}

static char *vMySprintf_BuildChar(uint8_t *ucMem, DevicePartitionTableDataTypeTPDF tType)
{
	uint32_t i;
	int16_t ii;
	float j;
	char k[128];
	static char  *l;
	free(l);
	memset(k, 0, 32);
	switch(tType)
	{
		case DEVICE_PARTITION_TABLE_DATA_TYPE_FLOAT:
			i = ucMem[2] + (ucMem[3] << 8) + (ucMem[0] << 16) + (ucMem[1] << 24);
			j = *((float *)&i);
			sprintf(k, "%.2f\n", j);
			for(i = 0; k[i] != '\n';i++);
			l = malloc(i + 1);
			memcpy(l,k,i);
			l[i] = 0;
			return(l);
		case DEVICE_PARTITION_TABLE_DATA_TYPE_UINT_F1:
		case DEVICE_PARTITION_TABLE_DATA_TYPE_UINT_F2:
			i = ucMem[0] + (ucMem[1] << 8);
			j = (float)i;
			if(tType == DEVICE_PARTITION_TABLE_DATA_TYPE_UINT_F1)
			{
				j /= 10.0f;
				sprintf(k, "%.1f\n", j);
			}
			else
			{
				j /= 100.0f;
				sprintf(k, "%.2f\n", j);
			}
			for(i = 0; k[i] != '\n';i++);
			l = malloc(i + 1);
			memcpy(l,k,i);
			l[i] = 0;
			return(l);
		case DEVICE_PARTITION_TABLE_DATA_TYPE_SINT_F1:
		case DEVICE_PARTITION_TABLE_DATA_TYPE_SINT_F2:
			ii = ucMem[0] + (ucMem[1] << 8);
			j = (float)ii;
			if(tType == DEVICE_PARTITION_TABLE_DATA_TYPE_UINT_F1)
			{
				j /= 10.0f;
				sprintf(k, "%.1f\n", j);
			}
			else
			{
				j /= 100.0f;
				sprintf(k, "%.2f\n", j);
			}
			for(i = 0; k[i] != '\n';i++);
			l = malloc(i + 1);
			memcpy(l,k,i);
			l[i] = 0;
			return(l);
		case DEVICE_PARTITION_TABLE_DATA_TYPE_STATE:
		{
			uint32_t m,n;
			m = 1;
			n = *ucMem;
			k[0] = '[';
			for(i = 0; i < 16; i++)
			{
				if(n & 1)
				{
					memcpy(&k[m], "true,", 5);
					m += 5;
				}
				else
				{
					memcpy(&k[m], "false,", 6);
					m += 6;
				}
				n >>= 1;
			}
			k[m++] = ']';
			l = malloc(m + 1);
			memcpy(l,k,m);
			l[m] = 0;
			return(l);
		}
		case DEVICE_PARTITION_TABLE_DATA_TYPE_DINT:
			i = ucMem[2] + (ucMem[3] << 8) + (ucMem[0] << 16) + (ucMem[1] << 24);
			sprintf(k, "%d\n", i);
			for(i = 0; k[i] != '\n';i++);
			l = malloc(i + 1);
			memcpy(l,k,i);
			l[i] = 0;
			return(l);
		case DEVICE_PARTITION_TABLE_DATA_TYPE_SINT:
			ii = ucMem[0] + (ucMem[1] << 8);
			sprintf(k, "%d\n", ii);
			for(i = 0; k[i] != '\n';i++);
			l = malloc(i + 1);
			memcpy(l,k,i);
			l[i] = 0;
			return(l);
		default:
			i = ucMem[0] + (ucMem[1] << 8);
			sprintf(k, "%d\n", i);
			for(i = 0; k[i] != '\n';i++);
			l = malloc(i + 1);
			memcpy(l,k,i);
			l[i] = 0;
			return(l);
	}
}

static void WIFI_vMqtt_Event_Handler_CB(esp_mqtt_event_handle_t event)
{
    switch (event->event_id)
    {
	case MQTT_EVENT_CONNECTED:
		// mQttConnected = 1;
		ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
		CMConnectedManage(event);
		break;
	case MQTT_EVENT_DISCONNECTED:
		// mQttConnected = 0;
		ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
		break;
	case MQTT_EVENT_SUBSCRIBED:
		ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
		break;
	case MQTT_EVENT_UNSUBSCRIBED:
		ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
		break;
	case MQTT_EVENT_PUBLISHED:
		ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
		break;
	case MQTT_EVENT_DATA:
		ESP_LOGI(TAG, "MQTT_EVENT_DATA");
		// 中移动数据处理函数
		CMEventDataHandler(event);
		break;
	case MQTT_EVENT_ERROR:
		ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
		if (event->error_handle->error_type == MQTT_ERROR_TYPE_ESP_TLS) {
			ESP_LOGI(TAG, "Last error code reported from esp-tls: 0x%x", event->error_handle->esp_tls_last_esp_err);
			if(event->error_handle->esp_tls_last_esp_err == 0x0810)
			{
				esp_restart();
			}
			ESP_LOGI(TAG, "Last tls stack error number: 0x%x", event->error_handle->esp_tls_stack_err);
		} else if (event->error_handle->error_type == MQTT_ERROR_TYPE_CONNECTION_REFUSED) {
			ESP_LOGI(TAG, "Connection refused error: 0x%x", event->error_handle->connect_return_code);
		} else {
			ESP_LOGW(TAG, "Unknown error type: 0x%x", event->error_handle->error_type);
		}
		break;
	default:
		ESP_LOGI(TAG, "Other event id:%d", event->event_id);
		break;
    }
	return ESP_OK;
}

static void WIFI_vMqtt_Event_Handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
	WIFI_vMqtt_Event_Handler_CB(event_data);
}

void Wifi_Mqtt_vInit(void)
{
	vTaskDelay(2000);
	char temp_ip_str[36] = {0};
	char temp_client_id_str[36] = {0};
	char temp_user_name_str[36] = {0};
	char temp_password_str[36] = {0};

	ConvertUint16ToString(mPartitionTable.PlatformConfig.IPConfig, temp_ip_str, 16);
	ESP_LOGI(TAG, "mPartitionTable.PlatformConfig.IPConfig:%s", temp_ip_str);

	ESP_LOGI(TAG, "mPartitionTable.PlatformConfig.CommPort:%d", mPartitionTable.PlatformConfig.CommPort);

	ConvertUint16ToString(mPartitionTable.PlatformConfig.ClientIDConfig, temp_client_id_str, 16);
	ESP_LOGI(TAG, "mPartitionTable.PlatformConfig.ClientIDConfig:%s", temp_client_id_str);

	ConvertUint16ToString(mPartitionTable.PlatformConfig.UserNameConfig, temp_user_name_str, 16);
	ESP_LOGI(TAG, "mPartitionTable.PlatformConfig.UserNameConfig:%s", temp_user_name_str);

	ConvertUint16ToString(mPartitionTable.PlatformConfig.PasswordConfig, temp_password_str, 16);
	ESP_LOGI(TAG, "mPartitionTable.PlatformConfig.PasswordConfig:%s", temp_password_str);

	ESP_LOGI(TAG, "mPartitionTable.PlatformConfig.KeepaliveTime:%d", mPartitionTable.PlatformConfig.KeepaliveTime);
	ESP_LOGI(TAG, "mPartitionTable.PlatformConfig.MQTTQos:%d", mPartitionTable.PlatformConfig.MQTTQos);

	esp_mqtt_client_config_t china_mobile_cfg = {
		.host = temp_ip_str,
		.port = mPartitionTable.PlatformConfig.CommPort,
		.client_id = temp_client_id_str,
		.username = temp_user_name_str,
		.password = temp_password_str,
		// .transport = MQTT_TRANSPORT_OVER_SSL,
		.keepalive = mPartitionTable.PlatformConfig.MQTTQos,
		.lwt_qos = mPartitionTable.PlatformConfig.LWTQos,
		.task_prio = 4,
		.reconnect_timeout_ms = 10000,
	};

	china_mobile_client = esp_mqtt_client_init(&china_mobile_cfg);
	esp_mqtt_client_register_event(china_mobile_client, ESP_EVENT_ANY_ID, WIFI_vMqtt_Event_Handler, china_mobile_client);
	esp_mqtt_client_start(china_mobile_client);
}

void Wifi_Mqtt_vTsk_Delay(MyQueueDataTPDF tMyQueueData, uint16_t usDelay)
{
	while(usDelay)
	{
		vTaskDelay(1);
		if(xQueueReceive(mWifiQueueSend, (void * )&tMyQueueData, (portTickType)0))
		{

		}
		usDelay--;
	}
}

void Wifi_Mqtt_vPublish(MqttPublishDataTPDF *tData)
{
	uint16_t j;
	static char buf[128];
	memset(buf, 0, sizeof(buf));
	sprintf(&buf[3], "{\"%s\":%s}", tData->cFromat, vMySprintf_BuildChar(tData->ucMem,tData->tDevicePartitionTableDataType));
	j = strlen(&buf[3]);
	buf[0] = 3;
	buf[1] = j >> 8;
	buf[2] = j & 0xFF;
	esp_mqtt_client_publish(mClient, "$dp", buf, j + 3, 0, 0);
}

void Wifi_Mqtt_vTsk(uint8_t ucConnected, MyQueueDataTPDF tMyQueueData)
{
	uint16_t i;

	mMyQueueData = tMyQueueData;
	if(ucConnected && mQttConnected)
	{
		Wifi_Mqtt_vTsk_Delay(tMyQueueData, CONFIG_WIFI_MQTT_DEFAULT_PUBLISH_TICKS);
		for(i = 0; i < mMqttPublishDataCount; i++)
		{
			Wifi_Mqtt_vPublish(&mMqttPublishData[i]);
			Wifi_Mqtt_vTsk_Delay(tMyQueueData, 1);
		}
	}
	else
	{
		Wifi_Mqtt_vTsk_Delay(tMyQueueData, 1);
	}
}
