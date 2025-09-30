#ifndef SAGA_CHECKS_H
#define SAGA_CHECKS_H

#include <stdbool.h>

// TODO: Add if present check
// #include <assert.h>

#ifdef static_assert
#define SAGA_ASSERT(expression, message) static_assert(expression, message)
#else
#define SAGA_ASSERT(expression, message)
#endif

// Used to check that the log message strings and arguments conform to the printf() format. Will throw compile error if they don't.
// This function should however never be called due to the macro having an if(false) check. It will however still do the type check at compilation.
// Based on this article: // https://developer.arm.com/documentation/dui0375/g/Compiler-specific-Features/--attribute----format---function-attribute
void saga_test_string_format(const char* message, ...) __attribute__((format(printf, 1, 2)));
#define SAGA_MESSAGE_FORMAT_CHECK(message, ...) if (false) { saga_test_string_format(message, ##__VA_ARGS__); }

// String related checks
// ——————————————————————

#define SAGA_MESSAGE_STRING_CHECKS(message, ...)                                                                   \
    /* Check that the message length conforms to the SAGA_MAX_MESSAGE_STRING_LENGTH  */                            \
    SAGA_ASSERT(sizeof(message) <= SAGA_MAX_MESSAGE_SIZE, "Message length is larger than SAGA_MAX_MESSAGE_SIZE!"); \
    /* Check if string conforms to specified format */                                                             \
    SAGA_MESSAGE_FORMAT_CHECK(message, ##__VA_ARGS__)

// Main checks macro
// ——————————————————

#define SAGA_RUN_CHECKS(level, message, ...)           \
    SAGA_MESSAGE_STRING_CHECKS(message, ##__VA_ARGS__)

#endif // SAGA_CHECKS_H
