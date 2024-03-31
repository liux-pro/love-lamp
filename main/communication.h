#pragma once
#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"

#define LED_STATUS_OFF 0
#define LED_STATUS_ON 1

// 使用 typedef 定义结构体 BaseControl
typedef struct {
    uint8_t state;
    uint8_t r;
    uint8_t g;
    uint8_t b;
} BaseControl;

bool parseBaseControl(const char *jsonData, size_t jsonDataLen, BaseControl *baseControl);
