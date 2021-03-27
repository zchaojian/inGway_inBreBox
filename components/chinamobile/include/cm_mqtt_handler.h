/**********************************************************************************
  *Copyright(C),2010-2011,CHNT
  *@FileName: eth_mqtt_handler.h
  *@Author:   ElonStark
  *@Version:  V1.0
  *@Date:     2020.11.13
  *@Description: 用于处理来自MQTT的数据
  *@Others:   None
  *History:
  *   1.Date:
  *    Author:
  *    Modification:
**********************************************************************************/
#ifndef ETH_MQTT_HANDLER_H
#define ETH_MQTT_HANDLER_H

#include "mqtt_client.h"
#include "cJSON.h"

#include "device_app.h"

#define CODE_SET_DEVICE "setDevice"
#define CODE_SET_DEVICE_CALLBACK "callbackDevice"
#define CODE_SET_GATEWAY "setGateway"
#define CODE_SET_GATEWAY_CALLBACK "callbackGateway"
#define CODE_CONTROL_DEVICE "controlDevice"
#define CODE_CONTROL_DEVICE_CALLBACK "callbackControl"
#define CODE_REPORT_DEVICE_EVENT "reportDeviceEvent"
#define CODE_REPORT_DEVICE_PROPERTY "reportDeviceProperty"
#define CODE_REPORT_GATEWAY_EVENT "reportGatewayEvent"
#define CODE_REPORT_GATEWAY_PROPERTY "reportGatewayProperty"
#define CODE_REPORT_GATEWAY_INFO "reportGatewayInfo"
#define CODE_OTA "ota"
#define CODE_OTA_CALLBACK "callbackOTA"
#define CODE_GET_GATEWAY_CONFIG "getGatewayConfig"
#define CODE_SET_GATEWAY "setGateway"
#define CODE_GET_GATEWAY_LOG "getGatewayLog"
#define CODE_GET_GATEWAY_LOG_CALLBACK "callbackGatewayLog"

extern char *upgrade_url;

extern void CMConnectedManage(esp_mqtt_event_handle_t event);
extern void CMEventDataHandler(esp_mqtt_event_handle_t event);
extern void TimerReportMessage();

#endif // ETH_MQTT_HANDLER_H
