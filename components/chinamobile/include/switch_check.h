/*** 
 * @Author: ElonStark
 * @Date: 2020-12-16 13:06:38
 * @LastEditTime: 2020-12-25 15:32:36
 * @LastEditors: ElonStark
 * @Description: 
 * @FilePath: /inGwayV2.0/components/common/include/switch_check.h
 */

#ifndef SWITCH_CHECK_H
#define SWITCH_CHECK_H
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern TaskHandle_t switch_check_task;

extern void CreateSwitchCheckTask();
extern void DestorySwitchCheckTask();
#endif