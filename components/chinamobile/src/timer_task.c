/*
 * @Author: ElonStark
 * @Date: 2020-12-02 20:56:11
 * @LastEditTime: 2020-12-28 16:39:45
 * @LastEditors: ElonStark
 * @Description: Create timer for other task.
 * @FilePath: /inGwayV2.0/components/common/src/timer_task.c
 */

#include "../include/timer_task.h"

#include <stdio.h>
#include "esp_log.h"
#include "esp_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"

#include "cm_mqtt_handler.h"

// static const char *TAG = "timer_task";

/*
 * A sample structure to pass events
 * from the timer interrupt handler to the main program.
 */
typedef struct {
    int type;  // the type of timer's event
    int timer_group;
    int timer_idx;
    uint64_t timer_counter_value;
} timer_event_t;

typedef enum{
    CMMqttTimer = 1,
}EventTypeTPDF;

static timer_event_t evt;

static xQueueHandle timer_queue;

TaskHandle_t timer_task=NULL;

#define TIMER_DIVIDER         50000  //  Hardware timer clock divider
#define TIMER_SCALE           (TIMER_BASE_CLK / TIMER_DIVIDER)  // convert counter value to seconds
#define TEST_WITHOUT_RELOAD   0        // testing will be done without auto reload
#define TEST_WITH_RELOAD      1        // testing will be done with auto reload

void CreateTimer(uint16_t id, uint16_t minute_count);
void DeleteTimer(uint16_t id);


/*
 * Timer group0 ISR handler
 *
 * Note:
 * We don't call the timer API here because they are not declared with IRAM_ATTR.
 * If we're okay with the timer irq not being serviced while SPI flash cache is disabled,
 * we can allocate this interrupt without the ESP_INTR_FLAG_IRAM flag and use the normal API.
 */
void IRAM_ATTR timer_group0_isr(void *para)
{
    timer_spinlock_take(TIMER_GROUP_0);
    int timer_idx = (int) para;

    /* Retrieve the interrupt status and the counter value
       from the timer that reported the interrupt */
    uint32_t timer_intr = timer_group_get_intr_status_in_isr(TIMER_GROUP_0);
    uint64_t timer_counter_value = timer_group_get_counter_value_in_isr(TIMER_GROUP_0, timer_idx);

    /* Prepare basic event data
    that will be then sent back to the main program task */
    evt.timer_group = 0;
    evt.timer_idx = timer_idx;
    evt.timer_counter_value = timer_counter_value;

    /* Clear the interrupt
       and update the alarm time for the timer with without reload */
    if (timer_intr & TIMER_INTR_T1) {
        evt.type = CMMqttTimer;
        timer_group_clr_intr_status_in_isr(TIMER_GROUP_0, TIMER_1);// clear the status.
    }

    /* After the alarm has been triggered
      we need enable it again, so it is triggered the next time */
    timer_group_enable_alarm_in_isr(TIMER_GROUP_0, timer_idx);

    /* Now just send the event data back to the main program task */
    xQueueSendFromISR(timer_queue, &evt, NULL);
    timer_spinlock_give(TIMER_GROUP_0);
}

static void CMMqttTimerTask()
{
    while(1)
    {
        if(xQueueReceive(timer_queue, &evt, portMAX_DELAY))
        {
            // ESP_LOGI(TAG, "timer event interrupt");
            TimerReportMessage();
        }
    }
}

static void GatewayTimerInit(timer_group_t group_num, timer_idx_t timer_idx,
                                   bool auto_reload, uint64_t count_value)
{
    /* Select and initialize basic parameters of the timer */
    timer_config_t config = {
        .divider = TIMER_DIVIDER,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_PAUSE,
        .alarm_en = TIMER_ALARM_EN,
        .auto_reload = auto_reload,
    }; // default clock source is APB
    timer_init(group_num, timer_idx, &config);

    /* Timer's counter will initially start from value below.
       Also, if auto_reload is set, this value will be automatically reload on alarm */
    timer_set_counter_value(group_num, timer_idx, 0x00000000ULL);

    /* Configure the alarm value and the interrupt on alarm. */
    timer_set_alarm_value(group_num, timer_idx, count_value);
    timer_enable_intr(group_num, timer_idx);
    timer_isr_register(group_num, timer_idx, timer_group0_isr,
                       (void *) timer_idx, ESP_INTR_FLAG_IRAM, NULL);

    timer_start(group_num, timer_idx);
}

static void CreateCMTimer(uint16_t minute_count)
{
    if(timer_task != NULL || timer_queue != NULL) DeleteTimer(CMTimer);
    vTaskDelay(100);
    timer_queue = xQueueCreate(5, sizeof(timer_event_t));
    GatewayTimerInit(TIMER_GROUP_0, TIMER_1, TEST_WITH_RELOAD, 1600*minute_count);
    xTaskCreate(CMMqttTimerTask, "china mobile mqtt timer task", 1024*3, NULL, 4, &timer_task);
}

static void DeleteCMTimer()
{
    if(timer_task == NULL && timer_queue == NULL) return;
    vQueueDelete(timer_queue);
    timer_pause(TIMER_GROUP_0, TIMER_1);
    timer_disable_intr(TIMER_GROUP_0, TIMER_1);
    timer_deinit(TIMER_GROUP_0, TIMER_1);
    vTaskDelete(timer_task);
    timer_task = NULL;
    timer_queue = NULL;
}

void CreateTimer(uint16_t id, uint16_t minute_count)
{
    switch (id)
    {
    // China mobile MQTT timer.
    case CMTimer:
        CreateCMTimer(minute_count);
        break;
    
    default:
        break;
    }    
}


void DeleteTimer(uint16_t id)
{
    switch (id)
    {
    // China mobile MQTT timer.
    case CMTimer:
        DeleteCMTimer();
        break;
    
    default:
        break;
    }  
    
}