#pragma once

#include <driver/uart.h>

#define UART_PORT UART_NUM_2
#define UART_TX GPIO_NUM_12
#define UART_RX GPIO_NUM_13

// sdcard
#define PIN_NUM_MISO GPIO_NUM_15
#define PIN_NUM_CLK GPIO_NUM_2
#define PIN_NUM_MOSI GPIO_NUM_17
#define PIN_NUM_CS GPIO_NUM_22