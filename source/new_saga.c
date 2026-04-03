#include "new_saga.h"

typedef struct saga_buffer {
    // Number of bytes stored in the buffer
    _Atomic size_t bytes;

    // Buffer pointers
    _Atomic uintptr_t tail;
    _Atomic uintptr_t head;

    // Buffer
    uint8_t* const data;
} saga_buffer_t;
