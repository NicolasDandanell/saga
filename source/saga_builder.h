#ifndef LOG_BUILDER_H
#define LOG_BUILDER_H

#include "saga_preprocessor.h" // Included first!
#include "saga_hash.h"
#include "saga_data.h"

/// @brief The minimum size of a saga log. It consists of the 1 byte size, along with by the 4 byte hash
#define SAGA_MINIMUM_LOG_SIZE 5

/// @brief Sets the recursion level of the macro calls
#define SAGA_ARGS_EVAL(...)   SAGA_EVAL64(__VA_ARGS__)

// Log Entry Type Definition
// ——————————————————————————

/// @brief Declares member of the local log entry struct, and does an indirect recursive call if there are more arguments
#define SAGA_DATA_ENTRY(member, ...)                  \
    typeof(member) member;                            \
    SAGA_IF_ELSE(SAGA_HAS_ARGS(__VA_ARGS__))(         \
        SAGA_DEFER_3(_SAGA_DATA_ENTRY)()(__VA_ARGS__) \
    )(                                                \
                                                      \
    )

/// @brief Indirect call to force macro expansion and allow recursion (A macro can by default not call itself)
#define _SAGA_DATA_ENTRY() SAGA_DATA_ENTRY

/// @brief Check if any arguments at all, and does nothing if none were passed. If arguments were passed, it calls the @c SAGA_DATA_ENTRY macro
#define SAGA_DATA_ENTRIES(...)                    \
    SAGA_IF_ELSE(SAGA_HAS_ARGS(__VA_ARGS__))(     \
        SAGA_EVAL32(SAGA_DATA_ENTRY(__VA_ARGS__)) \
    )(                                            \
                                                  \
    )

/// @brief Creates the declaration of the the local log entry union struct, and calls @c SAGA_DATA_ENTRIES with the provided arguments
#define SAGA_ENTRY_TYPE(size, hash, ...)                                                     \
    union __attribute__((packed)) saga_local_entry {                                         \
        struct {                                                                             \
            const uint8_t  size;                                                             \
            const uint32_t hash;                                                             \
            SAGA_DATA_ENTRIES(__VA_ARGS__)                                                   \
        };                                                                                   \
        uint8_t saga_entry_data[SAGA_MINIMUM_LOG_SIZE + SAGA_COUNT_DATA_BYTES(__VA_ARGS__)]; \
    };

// Populate Log Entry
// ———————————————————

/// @brief Populates a member of the local log entry struct, and does an indirect recursive call if there are more arguments
#define SAGA_POPULATE_ENTRY_DATA(member, ...)                  \
    , .member = member                                         \
    SAGA_IF_ELSE(SAGA_HAS_ARGS(__VA_ARGS__))(                  \
        SAGA_DEFER_3(_SAGA_POPULATE_ENTRY_DATA)()(__VA_ARGS__) \
    )(                                                         \
                                                               \
    )

/// @brief Indirect call to force macro expansion and allow recursion (A macro can by default not call itself)
#define _SAGA_POPULATE_ENTRY_DATA() SAGA_POPULATE_ENTRY_DATA

/// @brief Populates the log entry with the provided arguments
#define SAGA_POPULATE_ENTRY(size, hash, ...)               \
    union saga_local_entry {                               \
        .size = size,                                      \
        .hash = hash                                       \
        SAGA_EVAL32(SAGA_POPULATE_ENTRY_DATA(__VA_ARGS__)) \
    };

// Build Log Entry
// ————————————————

/// @brief Builds the local log entry type, as then populates it with the log entry
#define SAGA_BUILD_LOG_ENTRY(entry_level, entry_size, entry_hash, ...) \
    SAGA_ENTRY_TYPE(entry_size, entry_hash, __VA_ARGS__)               \
    SAGA_POPULATE_ENTRY(entry_size, entry_hash, __VA_ARGS__)

#endif // LOG_BUILDER_H
