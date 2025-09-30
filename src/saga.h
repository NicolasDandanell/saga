#ifndef LOGS_H
#define LOGS_H

// ADD IF PRESENT CHECK, THEN DEFAULT IF NOT PRESENT

// Include as the first header!
// TODO: Add if present check
#include "saga_configurator.h"

#include <stdint.h>

// Configuration of saga
// ——————————————————————

#ifdef SAGA_UNIT_TEST
#define SAGA_ENTRY_SECTION
#define SAGA_METADATA_SECTION
#else
#define SAGA_ENTRY_SECTION    __attribute__((section(".saga_entries"), aligned(1), used, retain))
#define SAGA_METADATA_SECTION __attribute__((section(".saga_metadata"), used, retain))
#endif

#define SAGA_BUFFER_SIZE_MASK (SAGA_BUFFER_SIZE - 1)

#define SAGA_MAX_MESSAGE_SIZE 96 // Includes null terminator
#define SAGA_MAX_DATA_SIZE    16 // Max 31 --> Might be revisited...
#define SAGA_ARGS_EVAL(...)   SAGA_EVAL32(__VA_ARGS__)

#define SAGA_HASH_SIZE 4

#define SAGA_MINIMUM_LOG_SIZE 5 // Size byte + Hash
#define SAGA_MAXIMUM_LOG_SIZE (SAGA_MINIMUM_LOG_SIZE + SAGA_MAX_DATA_SIZE)

#define PACKED __attribute__((packed))

/*
    · Trace - Only when "tracing" the code and trying to find one part of a function specifically.
    · Debug - Information that is diagnostically helpful to people more than just developers (IT, customer support, sysadmins, etc.).
    · Info  - Generally useful information to log (service start/stop, configuration assumptions, etc). Information that is always available but usually not relevant under normal circumstances. This is the out-of-the-box config level.
    · Warn  - Anything that can potentially cause application oddities, but for which I am automatically recovering. (Such as switching from a primary to backup server, retrying an operation, missing secondary data, etc.)
    · Error - Any error which is fatal to the operation, but not the service or application (can't open a required file, missing data, etc.). These errors will force user (administrator, or direct user) intervention. These are usually reserved (in my apps) for incorrect connection strings, missing services, etc.
    · Fatal - Any error that is forcing a shutdown of the service or application to prevent data loss (or further data loss). I reserve these only for the most heinous errors and situations where there is guaranteed to have been data corruption or loss.
*/
typedef enum PACKED {
    SAGA_TRACE_LEVEL    = 0b001, /** Extremely detailed information about everything ongoing */
    SAGA_DEBUG_LEVEL    = 0b010, /** Details about operational behaviour that goes as intended */
    SAGA_INFO_LEVEL     = 0b011, /** Usage and operational information. Should usually always be logged */
    SAGA_WARNING_LEVEL  = 0b100, /** Unintended behaviour that does not affect operations */
    SAGA_ERROR_LEVEL    = 0b101, /** Unintended behaviour that does affect operations */
    SAGA_CRITICAL_LEVEL = 0b110, /** Breaking errors that result in a system failure */
} saga_level_t;

#include "saga_checks.h"
#include "saga_builder.h"

// typedef struct PACKED {
//     uint8_t  size;   /** Full size of the log */
//     uint32_t hash;   /** The unique hash for this log */
//     uint8_t  data[]; /** The data belonging to this log entry */
// } saga_log_entry_t;

typedef struct PACKED {
    const uint32_t         hash;
    const saga_level_t level;
    const char             message[SAGA_MAX_MESSAGE_SIZE];
    const uint8_t          data[SAGA_MAX_DATA_SIZE];
} saga_entry_data_t;

#define SAGA_TRACE_COLOUR    "\033[0;34m" // Teal
#define SAGA_DEBUG_COLOUR    "\033[0;32m" // Green
#define SAGA_INFO_COLOUR     "\033[0m"    // White
#define SAGA_WARNING_COLOUR  "\033[0;33m" // Yellow
#define SAGA_ERROR_COLOUR    "\033[0;31m" // Red
#define SAGA_CRITICAL_COLOUR "\033[0;35m" // Magenta

#define SAGA_TRACE_LABEL    "TRACE: "
#define SAGA_DEBUG_LABEL    "DEBUG: "
#define SAGA_INFO_LABEL     "INFO:  "
#define SAGA_WARNING_LABEL  "WARN:  "
#define SAGA_ERROR_LABEL    "ERROR: "
#define SAGA_CRITICAL_LABEL "CRIT:  "

#define SAGA_ADD_LOG(level, log) saga_add_log(level, log);

#define SAGA_ADD_METADATA(entry_level, entry_message, ...) {                     \
    SAGA_ENTRY_SECTION static const saga_entry_data_t entry_metadata = {         \
        .hash             = HASH_ENTRY(entry_message, entry_level, __VA_ARGS__), \
        .level            = entry_level,                                         \
        .message          = entry_message,                                       \
        .data             = CREATE_ENTRY_PARSING_DATA(__VA_ARGS__)               \
    };                                                                           \
}

#ifdef SAGA_PRINT_LOGS
#define SAGA_PRINT_LOG(level, log) saga_print_log(level, log);
#else
#define SAGA_PRINT_LOG(level, log)
#endif

#define SAGA_LOG_ENTRY(entry_level, entry_message, ...) {                                                      \
    SAGA_RUN_CHECKS(entry_level, entry_message, ##__VA_ARGS__)                                                 \
    SAGA_ADD_METADATA(entry_level, entry_message, ##__VA_ARGS__) /** Adds the log metadata to the .elf file */ \
    SAGA_BUILD_LOG(entry_level, entry_message, ##__VA_ARGS__)    /** Creates the 'log' array */                \
    SAGA_ADD_LOG(entry_level, log)                                                                             \
}

// Main Use Macros
// ————————————————

#define SAGA_TRACE(log_message, ...)    SAGA_LOG_ENTRY(SAGA_TRACE_LEVEL,    log_message, ##__VA_ARGS__)
#define SAGA_DEBUG(log_message, ...)    SAGA_LOG_ENTRY(SAGA_DEBUG_LEVEL,    log_message, ##__VA_ARGS__)
#define SAGA_INFO(log_message, ...)     SAGA_LOG_ENTRY(SAGA_INFO_LEVEL,     log_message, ##__VA_ARGS__)
#define SAGA_WARNING(log_message, ...)  SAGA_LOG_ENTRY(SAGA_WARNING_LEVEL,  log_message, ##__VA_ARGS__)
#define SAGA_ERROR(log_message, ...)    SAGA_LOG_ENTRY(SAGA_ERROR_LEVEL,    log_message, ##__VA_ARGS__)
#define SAGA_CRITICAL(log_message, ...) SAGA_LOG_ENTRY(SAGA_CRITICAL_LEVEL, log_message, ##__VA_ARGS__)

// Do not use this function directly. Use the above macros instead
void saga_add_log(uint8_t* log, saga_level_t level);

#endif // LOGS_H
