#ifndef SAGA_H
#define SAGA_H

#include <stdatomic.h>
#include <stdint.h>

#include "hringr.h"
#include "saga_internal.h"

/// @brief Status enum representing if the operation was successful, or if an error occurred
typedef enum saga_status {
    /// @brief The operation was successful
    SAGA_SUCCESS      = 0,

    /// @brief The Saga logger was empty, and thus no logs could be pulled out
    SAGA_EMPTY_BUFFER = 1,
} saga_status_t;

/// @brief   The configurable log levels in Saga
/// @details Saga is configurable to only output logs at or below a certain level.
///          Thus having the log level at @c SAGA_INFO_LEVEL means that logs of that or a lower level will be generated
///          They follow the same logic as the military "DEFCON" levels, where lower is more critical
typedef enum saga_level {
    /// @brief Saga is disabled, and no logs are generated
    SAGA_DISABLED       = 0b000,

    /// @brief Breaking errors that result in a system failure
    SAGA_CRITICAL_LEVEL = 0b001,

    /// @brief Unintended behaviour that does affect operations
    SAGA_ERROR_LEVEL    = 0b010,

    /// @brief Unintended behaviour that does not affect operations
    SAGA_WARNING_LEVEL  = 0b011,

    /// @brief Usage and operational information. Should usually always be logged
    SAGA_INFO_LEVEL     = 0b100,

    /// @brief Details about operational behaviour that goes as intended
    SAGA_DEBUG_LEVEL    = 0b101,

    /// @brief Extremely detailed information about everything ongoing
    SAGA_TRACE_LEVEL    = 0b110,
} saga_level_t;

/// @brief
typedef struct saga {
    /// @brief The amount of logs currently in the logger
    atomic_size_t log_count;

    /// @brief The current maximum log level
    atomic_size_t log_level;

    /// @brief The ring buffer where the logs are stored
    hringr_t      log_buffer;
} saga_t;

// Main Use Macros
// ————————————————

/// @brief Trace - Only when "tracing" the code and trying to find one part of a function specifically.
#define SAGA_TRACE(saga, log_message, ...)    SAGA_LOG_ENTRY(saga, SAGA_TRACE_LEVEL,    log_message, ##__VA_ARGS__)

/// @brief Debug - Information that is diagnostically helpful to people more than just developers (IT, customer support, sysadmins, etc.)
#define SAGA_DEBUG(saga, log_message, ...)    SAGA_LOG_ENTRY(saga, SAGA_DEBUG_LEVEL,    log_message, ##__VA_ARGS__)

/// @brief Generally useful information to log (service start/stop, configuration assumptions, etc). Information that is always available but usually not relevant under normal circumstances. This is the default out-of-the-box config level
#define SAGA_INFO(saga, log_message, ...)     SAGA_LOG_ENTRY(saga, SAGA_INFO_LEVEL,     log_message, ##__VA_ARGS__)

/// @brief Anything that can potentially cause application oddities, but for which I am automatically recovering. (Such as switching from a primary to backup server, retrying an operation, missing secondary data, etc.)
#define SAGA_WARNING(saga, log_message, ...)  SAGA_LOG_ENTRY(saga, SAGA_WARNING_LEVEL,  log_message, ##__VA_ARGS__)

/// @brief Any error which is fatal to the operation, but not the service or application (can't open a required file, missing data, etc.). These errors will force user (administrator, or direct user) intervention
#define SAGA_ERROR(saga, log_message, ...)    SAGA_LOG_ENTRY(saga, SAGA_ERROR_LEVEL,    log_message, ##__VA_ARGS__)

/// @brief Any error that is forcing a shutdown of the service or application to prevent data loss (or further data loss). I reserve these only for the most heinous errors and situations where there is guaranteed to have been data corruption or loss
#define SAGA_CRITICAL(saga, log_message, ...) SAGA_LOG_ENTRY(saga, SAGA_CRITICAL_LEVEL, log_message, ##__VA_ARGS__)

/// @brief Pulls logs out of the Saga logger, and onto the provided buffer
/// @details The function will try to pull as many full logs as there is space for in the buffer
/// @param[in, out] saga      Pointer to the Saga logger
/// @param[out]     data      Buffer where the pulled logs will be placed
/// @param[in]      data_size Size of the provided data buffer
/// @param[out]     data_out  Byte size of the logs pulled out of the logger, and onto the data buffer
/// @return
saga_status_t saga_pull_logs(saga_t* const saga, uint8_t* const data, const size_t data_size, size_t* const data_out);

/// @brief Gets the current maximum logging level of the saga logger
/// @param[in, out] saga Pointer to the Saga logger
/// @return              Returns the current maximum logging level from the @c saga_level_t enum
static saga_level_t saga_get_level(saga_t* const saga) {
    /// @note
    return (saga_level_t)atomic_load_explicit(&saga->log_level, memory_order_acquire);
}

/// @brief Atomically sets the maximum log level of the Saga logger
/// @note Any value higher than @c SAGA_TRACE_LEVEL passed to @param level argument will default to @c SAGA_TRACE_LEVEL, as its the highest legal value
/// @param[in, out] saga  Pointer to the Saga logger
/// @param[in]      level The log level to be se as the maximum log level. Any logs of a higher level will not be logged
static void saga_set_level(saga_t* const saga, const saga_level_t level) {
    const saga_level_t level_to_set = (level > SAGA_TRACE_LEVEL) ? SAGA_TRACE_LEVEL : level;

    /// @note
    atomic_store_explicit(&saga->log_level, level_to_set, memory_order_release);
}

#endif // SAGA_H
