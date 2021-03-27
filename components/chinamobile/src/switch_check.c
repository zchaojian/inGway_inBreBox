/*
 * @Author: ElonStark
 * @Date: 2020-12-16 13:06:28
 * @LastEditTime: 2020-12-25 15:32:58
 * @LastEditors: ElonStark
 * @Description: 
 * @FilePath: /inGwayV2.0/components/common/src/switch_check.c
 */
#include "switch_check.h"

#include "cm_mqtt_handler.h"

TaskHandle_t switch_check_task=NULL;

void CreateSwitchCheckTask();
void DestorySwitchCheckTask();

static void SwitchCheckTask()
{
    while(1)
    {
        // SwtichCheckHandler();
    }
}

void CreateSwitchCheckTask()
{
    if(switch_check_task != NULL) DestorySwitchCheckTask();
    vTaskDelay(100);
    xTaskCreate(SwitchCheckTask, "Amplitude Detect Task", 2048, NULL, 4, &switch_check_task);
}

void DestorySwitchCheckTask()
{
    if(switch_check_task == NULL) return;
    vTaskDelete(switch_check_task);
    switch_check_task = NULL;
}