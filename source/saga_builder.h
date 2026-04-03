#ifndef SAGA_BUILDER_H
#define SAGA_BUILDER_H

#include "saga_preprocessor.h" // Included first!
#include "saga_hash.h"
#include "saga_data.h"

// Log array populate macro
// —————————————————————————

// Check if any args. Do nothing if none
#define SAGA_ADD_TO_ARRAY_FIRST(function, array, counter, ...)                    \
    SAGA_IF_ELSE(SAGA_HAS_ARGS(__VA_ARGS__))(                                     \
        SAGA_DEFER_3(_SAGA_ADD_TO_ARRAY)()(function, array, counter, __VA_ARGS__) \
    )(                                                                            \
        /* Do nothing, just terminate */                                          \
    )

// Use memcpy to move args into the array, and use the counter to iterate over the array, increasing it by the size of each arg each time one is copied to the array
#define SAGA_ADD_TO_ARRAY(function, array, counter, first, ...)                                               \
    function(first, array, counter);                                                                          \
    SAGA_IF_ELSE(SAGA_HAS_ARGS(__VA_ARGS__))(                                                                 \
        SAGA_DEFER_3(_SAGA_ADD_TO_ARRAY)()(function, array, counter + SAGA_GET_TYPE_SIZE(first), __VA_ARGS__) \
    )(                                                                                                        \
        /* Do nothing, just terminate */                                                                      \
    )

#define _SAGA_ADD_TO_ARRAY() SAGA_ADD_TO_ARRAY // Indirect call to force macro expansion and allow recursion (A macro can by default not call itself)

// Call the first macro of the recursive macro, passing the SAGA_ADD_BYTES macro as a "function" to be called in each iteration of the macro.
// The macro recurses for the same amount of times as there are arguments
#define _SAGA_PARSE_ARGS(...) SAGA_ARGS_EVAL(SAGA_ADD_TO_ARRAY_FIRST(SAGA_ADD_BYTES, __VA_ARGS__))
#define SAGA_PARSE_ARGS(...) _SAGA_PARSE_ARGS(__VA_ARGS__)

#define SAGA_ADD_BYTES(arg, array, counter) do {              \
    for (uint32_t i = 0; i < SAGA_GET_TYPE_SIZE(arg); ++i) {  \
        ((uint8_t*)&array[counter])[i] = ((uint8_t*)&arg)[i]; \
    }                                                         \
} while(0)

#define SAGA_POPULATE_ARRAY(array, ...) SAGA_PARSE_ARGS(array, 0, ##__VA_ARGS__)

// Main log build macro
// —————————————————————

#define SAGA_BUILD_LOG(entry_level, entry_message, ...)                                                                                                         \
    uint8_t entry_size = SAGA_MINIMUM_LOG_SIZE + SAGA_COUNT_DATA_BYTES(__VA_ARGS__);                                                                            \
    uint32_t entry_hash = SAGA_HASH_ENTRY(entry_message, entry_level, __VA_ARGS__);                                                                             \
    uint8_t log[SAGA_MINIMUM_LOG_SIZE + SAGA_COUNT_DATA_BYTES(__VA_ARGS__)] = { [0 ... (SAGA_MINIMUM_LOG_SIZE + SAGA_COUNT_DATA_BYTES(__VA_ARGS__) - 1)] = 0 }; \
    SAGA_POPULATE_ARRAY(log, entry_size, entry_hash, __VA_ARGS__)

#endif // SAGA_BUILDER_H
