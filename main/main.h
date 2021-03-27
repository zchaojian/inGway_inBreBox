/*** 
 * @Author: ElonStark
 * @Date: 2020-12-01 17:54:40
 * @LastEditTime: 2020-12-17 17:04:04
 * @LastEditors: ElonStark
 * @Description: 
 * @FilePath: /inGwayV2.0/main/main.h
 */
/*
 * main.h
 *
 *  Created on: Dec 1, 2020
 *      Author: elonstark
 */

#ifndef MAIN_MAIN_H_
#define MAIN_MAIN_H_

#include "freertos/FreeRTOS.h"
#include "esp_system.h"
#include "esp_event_base.h"
#include "esp_event.h"
#include "esp_timer.h"

ESP_EVENT_DECLARE_BASE(TASK_EVENTS);         // declaration of the task events family

extern TaskHandle_t ble_task;

// Event loops
extern esp_event_loop_handle_t event_loop_task;

enum TaskEventTPDF{
	// BLE discovery
	CREATE_BLE_DISCOVERY_TASK = 1,
	DESTORY_BLE_DISCOVERY_TASK,

	CREATE_OTA_TASK,
	DESTORY_OTA_TASK,

	CREATE_TIMER_TASK,
	DESTORY_TIMER_TASK,

	CREATE_AMPLITUDE_DETECTION,
	DESTORY_AMPLITUDE_DETECTION,

	CREATE_SWITCH_CHECK_TASK,
	DESTORY_SWITCH_CHECK_TASK,
};

#endif /* MAIN_MAIN_H_ */
