#include <stddef.h>
#include <stdint.h>

#include "saga.h"

typedef struct saga_buffer {
    // Number of bytes stored in the buffer
    _Atomic size_t bytes;

    // Buffer pointers
    _Atomic uintptr_t tail;
    _Atomic uintptr_t head;

    // Buffer
    uint8_t data[SAGA_BUFFER_SIZE];
} saga_buffer_t;

// Runtime Variables
// ——————————————————

volatile _Atomic bool clearing_space = false;
volatile _Atomic bool adding_log     = false;

volatile static saga_level_t current_level = SAGA_INFO_LEVEL;

volatile static saga_buffer_t saga_buffer = {
    .head   = 0,
    .tail   = 0,
    .data = { [0 ... SAGA_BUFFER_SIZE - 1] = 0x00 }
};

#if SAGA_MAXIMUM_LOG_SIZE > SAGA_BUFFER_SIZE
#error "Saga buffer must be able to hold at least one log of maximum size!"
#endif

// Static Functions
// —————————————————

// #include <stdio.h>

// 0x84 - Not fully interrupt safe yet...
static void saga_push(const uint8_t* log) {
    const size_t log_size = log[0];

    size_t bytes = saga_buffer.bytes;

    // Clear space if buffer full
    // ———————————————————————————

    if (bytes + log_size > SAGA_BUFFER_SIZE) {
#if SAGA_OVERWRITE_OLD_LOGS == true
        size_t tail = saga_buffer.tail;

        // Calculate if will wrap around tail, and delete the oldest entries until we have space for the new one
        while (bytes + log_size > SAGA_BUFFER_SIZE) {

            // Push tail forward deleting the oldest entry. Also discount the amount of bytes deleted from the buffer byte counter
            const size_t oldest_log_size = saga_buffer.data[tail];

            // Subtract the size of the oldest log from the byte count
            bytes -= oldest_log_size;

            // Push head forward by the oldest log size, wrapping around the edge of the buffer
            tail = (tail + oldest_log_size) & SAGA_BUFFER_SIZE_MASK;
        }

        saga_buffer.bytes = bytes;
        saga_buffer.tail  = tail;
#else
        // If we are not overwriting old logs, then if the buffer is full simply return
        return;
#endif
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// FROM HERE IT IS ASSUMED THAT THERE WILL BE SPACE FOR THE NEW LOG. This needs to be safe ///
    ///////////////////////////////////////////////////////////////////////////////////////////////

    // Copy log data into buffer
    // ——————————————————————————

    // Get non-atomic a copy of the head and new value of head for indexing and comparison
    size_t head = saga_buffer.head;
    const size_t target_head = (head + log_size) & SAGA_BUFFER_SIZE_MASK;

    // Increase head before copying data to protect against interrupts also logging
    saga_buffer.head  = target_head;
    saga_buffer.bytes += log_size;

    do {
        saga_buffer.data[head] = *log;

        head = (head + 1) & SAGA_BUFFER_SIZE_MASK;
        ++log;
    } while (head != target_head);
}

// 0x74 - Not interrupt safe?
static size_t saga_pull(uint8_t* buffer, const size_t buffer_size) {

    // Static assert that the buffer pointer is not NULL

    size_t log_size = saga_buffer.data[saga_buffer.tail];
    size_t index    = 0;

    while (index + log_size <= buffer_size) {
        // Calculate what the tail will be once we remove the log from the buffer
        size_t tail        = saga_buffer.tail;
        size_t target_tail = (tail + log_size) & SAGA_BUFFER_SIZE_MASK;

        // Remove the log one byte at the time
        do {
            // Copy byte from saga buffer to target buffer
            buffer[index] = saga_buffer.data[tail];

            // Push tail forward by 1 byte, wrapping around the edge of the buffer
            tail = (tail + 1) & SAGA_BUFFER_SIZE_MASK;

            // Increase index
            ++index;
        } while(tail != target_tail);

        // Subtract the log size from the buffer byte counter
        saga_buffer.bytes -= log_size;

        // Check if buffer is empty, and return if it is
        if (saga_buffer.bytes == 0) {
            // Reset tail and head values to start of buffer
            saga_buffer.tail = (uintptr_t)saga_buffer.data;
            saga_buffer.head = (uintptr_t)saga_buffer.data;
            break;
        }

        saga_buffer.tail = tail;

        // Update log size for the next log entry
        log_size = saga_buffer.data[tail];
    };

    return index;
}

// Public Functions
// —————————————————

saga_level_t saga_get_level(void) {
    return current_level;
}

void saga_set_level(saga_level_t level) {
    current_level = level;
}

void saga_add_log(uint8_t* log, saga_level_t level) {
    if (level >= current_level) {
        // Print log if needed
        SAGA_PRINT_LOG(level, log);

        // Add to ring buffer
        saga_push(log);
    }
}

size_t saga_flush_logs(uint8_t* buffer, const size_t buffer_size) {
    return saga_pull(buffer, buffer_size);
}

// Static metadata for parsing logs from .elf file
// ————————————————————————————————————————————————

typedef struct {
    const uint32_t entry_size;
    const uint32_t max_message_size;
    const uint32_t max_data_size;
} saga_entry_structure_t;

SAGA_METADATA_SECTION static const saga_entry_structure_t saga_structure = {
    .entry_size       = sizeof(saga_entry_data_t),
    .max_message_size = SAGA_MAX_MESSAGE_SIZE,
    .max_data_size    = SAGA_MAX_DATA_SIZE
};

// This function should be optimized away by the compiler, but is still needed for the string format check.
void test_string_format(const char* message, ...) { return; }

// For Unit Testing
// —————————————————

#ifdef SAGA_UNIT_TEST
void saga_call_push(const uint8_t* log) {
    saga_push(log);
}

size_t saga_call_pull(uint8_t* buffer, const size_t buffer_size) {
    return saga_pull(buffer, buffer_size);
}

/** Used to setup and verify buffer during unit testing */
volatile saga_buffer_t* get_buffer_pointer(void) {
    return &saga_buffer;
}
#endif
