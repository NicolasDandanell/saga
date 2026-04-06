#ifndef SAGA_CHECKS_H
#define SAGA_CHECKS_H

#include <stdbool.h>

#include "saga_data.h"

/// @brief Check that the C version is at least C11
/// @verifies saga_requirement_00 Verifies that Saga cannot be compiled with a compiler that is not at least C11 compliant
#if !defined(__STDC_VERSION__) || (__STDC_VERSION__ < 201112L)
#error "Saga requires at least C11 to function. Please use a compiler from this century if you wish to use Saga."
#endif

#define SAGA_ASSERT(expression, message) _Static_assert(expression, message);

// Data Related Checks
// ————————————————————

/// @brief Sanity check the configured @c SAGA_MAX_DATA_SIZE
/// @verifies saga_requirement_001 Verifies that the configured @c SAGA_MAX_DATA_SIZE conforms to the 255 log size limit,
///                                as logs 5 bytes for the size and hash, leaving 250 bytes for the data
#if SAGA_MAX_DATA_SIZE > 250
#error "SAGA_MAX_DATA_SIZE is larger than the maximum allowed 250 bytes!"
#endif // SAGA_MAX_DATA_SIZE

/// @brief Sanity check the combined size of the log entry arguments, and see if they conform to the configured @c SAGA_MAX_DATA_SIZE
#define SAGA_SIZE_CHECKS(size)                                                                                                            \
    SAGA_ASSERT(size <= (SAGA_MINIMUM_LOG_SIZE + SAGA_MAX_DATA_SIZE), "Log data size is larger than the max allowed SAGA_MAX_DATA_SIZE!")

// String Related Checks
// ——————————————————————

/// @brief Used to check that the log message strings and arguments conform to the printf format. Will throw compile error if they don't.
/// @note  This function should however never be called due to the macro having an if(false) check. It will however still do the type check at compilation.
//         Based on this article: // https://developer.arm.com/documentation/dui0375/g/Compiler-specific-Features/--attribute----format---function-attribute
void saga_test_string_format(const char* message, ...) __attribute__((format(printf, 1, 2)));

/// @brief    False call to function that requires the string to be printf formatted
/// @verifies saga_requirement_002 Verifies that the provided string and it's arguments follow the printf format, as if they do not, there will be compile error
#define SAGA_MESSAGE_PRINTF_FORMAT_CHECK(message, ...) if (false) { saga_test_string_format(message, ##__VA_ARGS__); }

/// @brief Check that the message length conforms to the @c SAGA_MAX_MESSAGE_STRING_LENGTH, and the message itself conforms to the printf format.
#define SAGA_MESSAGE_STRING_CHECKS(message, ...)                                                                  \
    SAGA_ASSERT(sizeof(message) <= SAGA_MAX_MESSAGE_SIZE, "Message length is larger than SAGA_MAX_MESSAGE_SIZE!") \
    SAGA_MESSAGE_PRINTF_FORMAT_CHECK(message, ##__VA_ARGS__)

// Main checks macro
// ——————————————————

/// @brief Runs all the compile time checks to verify that the log entry is valid
#define SAGA_RUN_CHECKS(level, message, size, ...)     \
    SAGA_MESSAGE_STRING_CHECKS(message, ##__VA_ARGS__) \
    SAGA_SIZE_CHECKS(size)
#endif // SAGA_CHECKS_H
