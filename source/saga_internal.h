#ifndef SAGA_INTERNAL_H
#define SAGA_INTERNAL_H

#include <stddef.h>
#include <stdint.h>

#if __has_include("saga_configuration.h")
#include "saga_configuration.h"
#else
// Use default values
#define SAGA_OVERWRITE_OLD_LOGS true
#define SAGA_MAX_MESSAGE_SIZE   128 // Includes null terminator
#define SAGA_MAX_DATA_SIZE      250
/// @todo Define @c SAGA_ENTRY_SECTION to default to ".note.saga"
#endif

#include "saga_builder.h"
#include "saga_checks.h"

/// @brief Adds log entry metadata to
#define SAGA_ADD_METADATA(entry_level, entry_message, ...)               \
    SAGA_ENTRY_SECTION static const saga_entry_data_t entry_metadata = { \
        .hash             = entry_hash,                                  \
        .level            = entry_level,                                 \
        .message          = entry_message,                               \
        .data             = SAGA_CREATE_ENTRY_PARSING_DATA(__VA_ARGS__)  \
    };

/// @brief Main log entry. It sanity checks the log, adds metadata to build, encodes the log at runtime, and calls the add log function
#define SAGA_LOG_ENTRY(saga, entry_level, entry_message, ...)                                   \
if (entry_level <= saga_get_level(saga)) {                                                      \
    const uint8_t saga_entry_size = SAGA_MINIMUM_LOG_SIZE + SAGA_COUNT_DATA_BYTES(__VA_ARGS__); \
    const uint32_t saga_entry_hash = SAGA_HASH_ENTRY(entry_message, entry_level, __VA_ARGS__);  \
    SAGA_RUN_CHECKS(entry_level, entry_message, saga_entry_size, ##__VA_ARGS__)                 \
    SAGA_ADD_METADATA(entry_level, entry_message, entry_hash, ##__VA_ARGS__)                    \
    SAGA_BUILD_LOG_ENTRY(entry_level, saga_entry_size, saga_entry_hash, ##__VA_ARGS__)          \
    saga_add_log(saga, entry_level, saga_local_entry.saga_entry_data)                           \
}

// Forward declare the Saga type, as it's defined in the main header
typedef struct saga saga_t;

/// @brief Internal Saga function to add log to log buffer
/// @param[in, out] saga Adds log to the Saga logger
/// @param[in]      log  The encoded log as a data buffer, with the first byte being the entry size
/// @warning DO NOT use this function directly. Use the provided Saga macros instead
void saga_add_log(saga_t* const saga, uint8_t* const log);

#endif // SAGA_INTERNAL_H
