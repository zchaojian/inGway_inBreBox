/*** 
 * @Author: ElonStark
 * @Date: 2020-12-02 20:56:25
 * @LastEditTime: 2020-12-03 12:47:30
 * @LastEditors: ElonStark
 * @Description: 
 * @FilePath: /inGwayV2.0/components/common/include/timer_task.h
 */

#ifndef COMMON_TIME_TASK_H
#define COMMON_TIME_TASK_H

#include <stdio.h>

typedef enum{
    CMTimer = 1,
}TimerList;

extern void CreateTimer(uint16_t id, uint16_t minute_count);
extern void DeleteTimer(uint16_t id);

#endif