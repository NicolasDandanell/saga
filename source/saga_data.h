#ifndef SAGA_DATA_H
#define SAGA_DATA_H

#include "saga_preprocessor.h"

// Count Data Bytes Size
// ——————————————————————

#define _SAGA_GET_TYPE_SIZE(X) sizeof(X)
#define SAGA_GET_TYPE_SIZE(...) _SAGA_GET_TYPE_SIZE(__VA_ARGS__)

// Check if any args at all. Return 0 if none
#define SAGA_COUNT_BYTES_FIRST(function, ...)               \
    SAGA_IF_ELSE(SAGA_HAS_ARGS(__VA_ARGS__))(               \
        SAGA_DEFER_3(_COUNT_BYTES)()(function, __VA_ARGS__) \
    )(                                                      \
        0                                                   \
    )

// Get the size of the arg, and if there are more add the next one:
// First call:  sizeOfArg_1 +
// Second call: sizeOfArg_2 +
// ...
// Last call:   sizeOfArg_Last
#define SAGA_COUNT_BYTES(function, first, ...)                             \
    function(first)                                                        \
    SAGA_IF_ELSE(SAGA_HAS_ARGS(__VA_ARGS__))(                              \
        SAGA_ADD(SAGA_DEFER_3(_SAGA_COUNT_BYTES)()(function, __VA_ARGS__)) \
    )(                                                                     \
        /* Do nothing, just terminate */                                   \
    )

#define _SAGA_COUNT_BYTES() SAGA_COUNT_BYTES // Indirect call to force macro expansion and allow recursion (A macro can by default not call itself)


// Call the first macro of the recursive macro, passing the SAGA_GET_TYPE_SIZE macro as a "function" to be called in each iteration of the macro.
// The macro recurses for the same amount of times as there are arguments
#define _SAGA_COUNT_DATA_BYTES(...) SAGA_ARGS_EVAL(SAGA_COUNT_BYTES_FIRST(SAGA_GET_TYPE_SIZE, __VA_ARGS__))
#define SAGA_COUNT_DATA_BYTES(...) _SAGA_COUNT_DATA_BYTES(__VA_ARGS__)

// Parse entry data
// —————————————————

// 8 Bit

// Signed 8 bit definitions (Generic type comparisons can give problems depending on the compiler, hence these checks)
#if (char == int8_t)
    #define SAGA_I8_TYPE   \
    char:           0x81
#else
    #define SAGA_I8_TYPE   \
    char:           0x81,  \
    int8_t:         0x81
#endif // (char == int8_t)

// Unsigned 8 bit definitions (Generic type comparisons can give problems depending on the compiler, hence these checks)
#if (uchar == uint8_t)
    #define SAGA_U8_TYPE   \
    unsigned char:  0x01
#else
    #define SAGA_U8_TYPE   \
    unsigned char:  0x01,  \
    uint8_t:        0x01
#endif // (uchar == uint8_t)

// 16 Bit

// Signed 16 bit definitions (Generic type comparisons can give problems depending on the compiler, hence these checks)
#if (short == int16_t)
    #define SAGA_I16_TYPE \
    short:          0x82
#else
    #define SAGA_I16_TYPE \
    short:          0x82, \
    uint16_t:       0x82
#endif // (short == int16_t)

// Unsigned 16 bit definitions (Generic type comparisons can give problems depending on the compiler, hence these checks)
#if (ushort == uint16_t)
    #define SAGA_U16_TYPE \
    unsigned short: 0x02
#else
    #define SAGA_U16_TYPE \
    unsigned short: 0x02, \
    uint16_t:       0x02
#endif // (ushort == uint16_t)

// 32 Bit

// Signed 32 bit definitions (Generic type comparisons can give problems depending on the compiler, hence these checks)
#if (int == int32_t)
    #define SAGA_I32_TYPE \
    int:            0x84
#else
    #define SAGA_I32_TYPE \
    int:            0x84, \
    int32_t:        0x84
#endif // (int == int32_t)

// Unsigned 32 bit definitions (Generic type comparisons can give problems depending on the compiler, hence these checks)
#if (uint == uint32_t)
    #define SAGA_U32_TYPE \
    unsigned int:   0x04
#else
    #define SAGA_U32_TYPE \
    unsigned int:   0x04, \
    uint32_t:       0x04
#endif // (uint == uint32_t)

// 64 Bit

// Signed 64 bit definitions (Generic type comparisons can give problems depending on the compiler, hence these checks)
#if (long == int64_t)
    #define SAGA_I64_TYPE \
    long:           0x88
#else
    #define SAGA_I64_TYPE \
    long:           0x88, \
    int64_t:        0x88
#endif // (long == int64_t)

// Unsigned 64 bit definitions (Generic type comparisons can give problems depending on the compiler, hence these checks)
#if (ulong == uint64_t)
    #define SAGA_U64_TYPE \
    unsigned long:  0x08
#else
    #define SAGA_U64_TYPE \
    unsigned long:  0x08, \
    uint64_t:       0x08
#endif // (ulong == uint64_t)

#define _SAGA_GET_TYPE_INFO(X) _Generic((X), \
    /* Single byte */                        \
    bool:     0x10,                          \
    SAGA_I8_TYPE,                            \
    SAGA_U8_TYPE,                            \
    /* Two byte */                           \
    SAGA_I16_TYPE,                           \
    SAGA_U16_TYPE,                           \
    /* Four byte */                          \
    SAGA_I32_TYPE,                           \
    SAGA_U32_TYPE,                           \
    /* Eight byte */                         \
    SAGA_I64_TYPE,                           \
    SAGA_U64_TYPE,                           \
    /* Default case */                       \
    default:  sizeof(X)                      \
)

#define SAGA_GET_TYPE_INFO(...) _SAGA_GET_TYPE_INFO(__VA_ARGS__)

// Check if any arguments at all
#define SAGA_PARSE_DATA_SIZE_FIRST(...)          \
    SAGA_IF_ELSE(SAGA_HAS_ARGS(__VA_ARGS__))(         \
        SAGA_PARSE_DATA_SIZE_SECOND(__VA_ARGS__) \
    )(                                           \
        { 0 } /* Empty array */                  \
    )

// The first argument needs to be parsed without a comma, resulting in the following: "{ firstArg"
#define SAGA_PARSE_DATA_SIZE_SECOND(first, ...)                                                  \
    /* Begin array brackets here --> */ { SAGA_GET_TYPE_INFO(first) /* No comma on first byte */ \
    SAGA_IF_ELSE(SAGA_HAS_ARGS(__VA_ARGS__))(                                                    \
        SAGA_DEFER_3(_SAGA_PARSE_DATA_SIZE)()(__VA_ARGS__)                                       \
    )(                                                                                           \
        } /* Close brackets and terminate */                                                     \
    )

// Parse all remaining arguments, and close the brackets when there are none left, like so: ", secondArg, thirdArg ... }"
#define SAGA_PARSE_DATA_SIZE(first, ...)                   \
    /* Comma added here --> */, SAGA_GET_TYPE_INFO(first)  \
    SAGA_IF_ELSE(SAGA_HAS_ARGS(__VA_ARGS__))(              \
        SAGA_DEFER_3(_SAGA_PARSE_DATA_SIZE)()(__VA_ARGS__) \
    )(                                                     \
        } /* Close brackets and terminate */               \
    )

#define _SAGA_PARSE_DATA_SIZE() SAGA_PARSE_DATA_SIZE // Indirect call to force macro expansion and allow recursion (A macro can by default not call itself)

// Calls the recursive macro. The macro recurses for the same amount of times as there are arguments
#define _SAGA_CREATE_ENTRY_PARSING_DATA(...) SAGA_ARGS_EVAL(SAGA_PARSE_DATA_SIZE_FIRST(__VA_ARGS__))
#define SAGA_CREATE_ENTRY_PARSING_DATA(...) _SAGA_CREATE_ENTRY_PARSING_DATA(__VA_ARGS__)

#endif // SAGA_DATA_H
