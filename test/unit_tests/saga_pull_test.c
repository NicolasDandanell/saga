#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <assert.h>

#include "saga.h"

#define LOG_DATA_0
#define LOG_DATA_1  , 0x01
#define LOG_DATA_2  , 0x01, 0x02
#define LOG_DATA_3  , 0x01, 0x02, 0x03
#define LOG_DATA_4  , 0x01, 0x02, 0x03, 0x04
#define LOG_DATA_5  , 0x01, 0x02, 0x03, 0x04, 0x05
#define LOG_DATA_6  , 0x01, 0x02, 0x03, 0x04, 0x05, 0x06
#define LOG_DATA_7  , 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
#define LOG_DATA_8  , 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
#define LOG_DATA_9  , 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09
#define LOG_DATA_10 , 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A
#define LOG_DATA_11 , 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B
#define LOG_DATA_12 , 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C
#define LOG_DATA_13 , 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D
#define LOG_DATA_14 , 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E
#define LOG_DATA_15 , 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
#define LOG_DATA_16 , 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10

#define _LOG_DATA(data_size) LOG_DATA_ ## data_size
#define LOG_DATA(data_size) _LOG_DATA(data_size)

#define SAGA_TEST_LOG_INIT(data_size) (uint8_t[5 + data_size]){ 5 + data_size, 0xF1, 0xF2, 0xF3, 0xF4 LOG_DATA(data_size) }

#define TEST_PRINT(message, ...)  printf(message"\n", ##__VA_ARGS__)

typedef struct saga_buffer {
    // Number of bytes stored in the buffer
    _Atomic size_t bytes;

    // Buffer pointers
    _Atomic uintptr_t tail;
    _Atomic uintptr_t head;

    // Buffer
    uint8_t data[SAGA_BUFFER_SIZE];
} saga_buffer_t;

extern size_t saga_call_pull(uint8_t* buffer, const size_t buffer_size);
volatile saga_buffer_t* get_buffer_pointer(void);

void beforeEach_pull(void) {
    volatile saga_buffer_t* buffer_pointer = get_buffer_pointer();

    buffer_pointer->bytes = 0;
    buffer_pointer->tail = 0;
    buffer_pointer->head = 0;
    memset((void*)buffer_pointer->data, 0x00, SAGA_BUFFER_SIZE);
}

void test_simple_one_log_pull(void) {
    TEST_PRINT("Pull Test 1");

    // Fill buffer with the log
    // —————————————————————————

    uint8_t log[6] = SAGA_TEST_LOG_INIT(1);

    volatile saga_buffer_t* buffer_pointer = get_buffer_pointer();

    buffer_pointer->bytes = 6;
    buffer_pointer->tail = 0;
    buffer_pointer->head = 6;
    memcpy((void *)buffer_pointer->data, log, 6);

    // Pull the log
    // —————————————

    uint8_t buffer[10] = { [0 ... 9] = 0x00 };

    size_t bytes_pulled = saga_call_pull(buffer, 10);

    // Compare to input, and check buffer values
    // ——————————————————————————————————————————

    buffer_pointer->bytes = 0;
    buffer_pointer->tail = 0;
    buffer_pointer->head = 0;
    assert(memcmp(buffer, (uint8_t*)buffer_pointer->data, 6) == 0);
}
