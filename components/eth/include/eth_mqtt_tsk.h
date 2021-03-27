/*** 
 * @Author: ElonStark
 * @Date: 2020-12-01 17:54:40
 * @LastEditTime: 2020-12-03 20:30:39
 * @LastEditors: ElonStark
 * @Description: 
 * @FilePath: /inGwayV2.0/components/eth/include/eth_mqtt_tsk.h
 */
/*
 * eth_mqtt_tsk.h
 *
 *  Created on: Aug 4, 2020
 *      Author: liwei
 */
#ifndef EXAMPLES_WG_WG_MAIN_ETH_MQTT_TSK_H_
#define EXAMPLES_WG_WG_MAIN_ETH_MQTT_TSK_H_
#include "device_app.h"
#include "../../common/include/myqueue.h"
#define DEFINE_DEVICE								2

#define CONFIG_ETH_MQTT_DEFAULT_PUBLISH_FORMAT		"A0000R0000T00"
#define CONFIG_ETH_MQTT_DEFAULT_SUBSCRIBE_FORMAT	"A%xR%xT%x:%d:"
#define CONFIG_ETH_MQTT_DEFAULT_PUBLISH_MAX			100
#define CONFIG_ETH_MQTT_DEFAULT_PUBLISH_TICKS		200

#define CONFIG_ETH_MQTT_DEFAULT_HOST				"183.230.40.39"
#define CONFIG_ETH_MQTT_DEFAULT_PORT				6002
#define CONFIG_ETH_MQTT_DEFAULT_PRODUCT_ID			"364418"

#if DEFINE_DEVICE == 1
#define CONFIG_ETH_MQTT_DEFAULT_AUTH_INFOR			"20200803"
#define CONFIG_ETH_MQTT_DEFAULT_DEVICE_ID			"615226885"
#elif DEFINE_DEVICE == 2
#define CONFIG_ETH_MQTT_DEFAULT_AUTH_INFOR			"20200811"
#define CONFIG_ETH_MQTT_DEFAULT_DEVICE_ID			"617637799"
#endif

typedef struct
{
	DevicePartitionTableDataTypeTPDF tDevicePartitionTableDataType;
	char *cFromat;
	uint8_t *ucMem;
}MqttPublishDataTPDF;


extern void Eth_Mqtt_vTsk_Delay(MyQueueDataTPDF tMyQueueData, uint16_t usDelay);

#endif
