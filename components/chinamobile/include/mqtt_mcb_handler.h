/*** 
 * @Author: ElonStark
 * @Date: 2020-12-11 09:02:25
 * @LastEditTime: 2021-01-05 10:35:15
 * @LastEditors: ElonStark
 * @Description: 
 * @FilePath: /inGwayV2.0/components/eth/include/cm_mqtt_chnt_mcb_handler.h
 */
/**********************************************************************************
  *Copyright(C),2010-2011,CHNT
  *@FileName: eth_mqtt_chnt_mcb_handler.h
  *@Author:   ElonStark
  *@Version:  V1.0
  *@Date:     2020.11.13
  *@Description: 用于CHNT MCB处理来自MQTT的数据
  *@Others:   None
  *History:
  *   1.Date:
  *    Author:
  *    Modification:
**********************************************************************************/

#ifndef ETH_MQTT_MCB_HANDLER_H
#define ETH_MQTT_MCB_HANDLER_H

#include "cJSON.h"
#include "mqtt_client.h"

extern void ConfigDeviceMCB(uint16_t break_index, cJSON *send_msg, cJSON *recv_property);
extern void ControlDeviceMCB(uint16_t break_index, cJSON *message_data, int opr);
extern void DeviceEventMCB(uint16_t break_index, cJSON *send_json_data);
extern void DevicePropertyMCB(uint16_t break_index, cJSON *send_json_data, uint16_t mode);

#endif // ETH_MQTT_MCB_HANDLER_H
