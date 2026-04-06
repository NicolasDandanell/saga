#ifndef SAGA_HASH_H
#define SAGA_HASH_H

#include "saga_preprocessor.h"

// String SAGA_HASH
// ————————————

// Implements a VERY quick and efficient compile time SDBM hash tokenizer for strings. It seems to essentially build the hash in reverse.
// The specific implementation below of the hashing is based on this article: http://lolengine.net/blog/2011/12/20/cpp-constant-string-hash
// NOTE: Original link does not seem to work anymore, so here is the internet archive version: https://web.archive.org/web/20210624162342/http://lolengine.net/blog/2011/12/20/cpp-constant-string-hash

#define SAGA_STRING_LENGTH(X) (sizeof(X) - 1)

#define SAGA_HASH_LEVEL(hash, level) (hash * 65599u) + (uint8_t) level

#define SAGA_HASH_1(string, index, hash)    hash * 65599u + (uint8_t) string[(index) < SAGA_STRING_LENGTH(string) ? SAGA_STRING_LENGTH(string) - 1 - (index) : SAGA_STRING_LENGTH(string)]
#define SAGA_HASH_2(string, index, hash)    SAGA_HASH_1(string,  index, (SAGA_HASH_1(string,  index + 1, hash)))
#define SAGA_HASH_4(string, index, hash)    SAGA_HASH_1(string,  index, (SAGA_HASH_1(string,  index + 1,  (SAGA_HASH_1(string,  index + 2,  (SAGA_HASH_1(string,  index + 3, hash)))))))
#define SAGA_HASH_8(string, index, hash)    SAGA_HASH_2(string,  index, (SAGA_HASH_2(string,  index + 2,  (SAGA_HASH_2(string,  index + 4,  (SAGA_HASH_2(string,  index + 6, hash)))))))
#define SAGA_HASH_16(string, index, hash)   SAGA_HASH_4(string,  index, (SAGA_HASH_4(string,  index + 4,  (SAGA_HASH_4(string,  index + 8,  (SAGA_HASH_4(string,  index + 12, hash)))))))
#define SAGA_HASH_24(string, index, hash)   SAGA_HASH_8(string,  index, (SAGA_HASH_8(string,  index + 8,  (SAGA_HASH_8(string,  index + 16, hash)))))
#define SAGA_HASH_32(string, index, hash)   SAGA_HASH_8(string,  index, (SAGA_HASH_8(string,  index + 8,  (SAGA_HASH_8(string,  index + 16, (SAGA_HASH_8(string,  index + 24, hash)))))))
#define SAGA_HASH_64(string, index, hash)   SAGA_HASH_16(string, index, (SAGA_HASH_16(string, index + 16, (SAGA_HASH_16(string, index + 32, (SAGA_HASH_16(string, index + 48, hash)))))))
#define SAGA_HASH_96(string, index, hash)   SAGA_HASH_32(string, index, (SAGA_HASH_32(string, index + 32, (SAGA_HASH_32(string, index + 64, hash)))))
#define SAGA_HASH_128(string, index, hash)  SAGA_HASH_32(string, index, (SAGA_HASH_32(string, index + 32, (SAGA_HASH_32(string, index + 64, (SAGA_HASH_32(string, index + 96, hash)))))))
#define SAGA_HASH_256(string, index, hash)  SAGA_HASH_64(string, index, (SAGA_HASH_64(string, index + 64, (SAGA_HASH_64(string, index + 128,(SAGA_HASH_64(string, index + 192, hash)))))))

#if   SAGA_MAX_MESSAGE_SIZE <= 16
#define SAGA_HASH_STRING(string) ((uint32_t)(SAGA_HASH_16(string, 0, 0)))
#elif SAGA_MAX_MESSAGE_SIZE <= 24
#define SAGA_HASH_STRING(string) ((uint32_t)(SAGA_HASH_24(string, 0, 0)))
#elif SAGA_MAX_MESSAGE_SIZE <= 32
#define SAGA_HASH_STRING(string) ((uint32_t)(SAGA_HASH_32(string, 0, 0)))
#elif SAGA_MAX_MESSAGE_SIZE <= 64
#define SAGA_HASH_STRING(string) ((uint32_t)(SAGA_HASH_64(string, 0, 0)))
#elif SAGA_MAX_MESSAGE_SIZE <= 96
#define SAGA_HASH_STRING(string) ((uint32_t)(SAGA_HASH_96(string, 0, 0)))
#elif SAGA_MAX_MESSAGE_SIZE <= 128
#define SAGA_HASH_STRING(string) ((uint32_t)(SAGA_HASH_128(string, 0, 0)))
#elif SAGA_MAX_MESSAGE_SIZE <= 256
#define SAGA_HASH_STRING(string) ((uint32_t)(SAGA_HASH_256(string, 0, 0)))
#endif

// Data Hash
// ——————————

/// @brief Leaf hash macro, which does the SDBM hashing
#define SAGA_DATA_HASH(value, hash) (hash) * 65599u + value

/// @brief Last call of the SDBM hash, finishing the recursion
#define SAGA_HASH_ARG_LAST(level, hash) \
    SAGA_DATA_HASH(level, hash)

/// @brief Check if any arguments at all. Return 0 if none
#define SAGA_HASH_ARG_FIRST(level, hash, ...)                  \
    SAGA_IF_ELSE(SAGA_HAS_ARGS(__VA_ARGS__))(                  \
        SAGA_HASH_ARG(level, hash, __VA_ARGS__)                \
    )(                                                         \
        SAGA_HASH_ARG_LAST(level, hash) /* And nothing else */ \
    )

/// @brief Check if more than one. Return the size of the first if only one, or use it as the starting value of the hash if more than one argument
#define SAGA_HASH_ARG(level, hash, first, ...)                                                                    \
    SAGA_IF_ELSE(SAGA_HAS_ARGS(__VA_ARGS__))(                                                                     \
        SAGA_DEFER_3(_SAGA_HASH_ARG)()(level, SAGA_DATA_HASH(SAGA_GET_TYPE_SIZE(first), hash), __VA_ARGS__)       \
    )(                                                                                                            \
        SAGA_HASH_ARG_LAST(level, SAGA_DATA_HASH(SAGA_GET_TYPE_SIZE(first), hash)) /* And no further recursion */ \
    )

/// @brief Indirect call to force macro expansion and allow recursion (A macro can by default not call itself)
#define _SAGA_HASH_ARG() SAGA_HASH_ARG

/// @brief Calls the hash macro through the macro expander macro @c SAGA_ARGS_EVAL, forcing multiple passes of the preprocessor
#define SAGA_HASH_DATA(hash, ...) SAGA_ARGS_EVAL(SAGA_HASH_ARG_FIRST(hash, __VA_ARGS__))

// Combined Hash
// ——————————————

/// @brief Hashes a log entry, by running the the entry message, arguments, and level through an SDBM hash
#define SAGA_HASH_ENTRY(string, level, ...) SAGA_HASH_DATA(level, SAGA_HASH_STRING(string), __VA_ARGS__)

#endif // SAGA_HASH_H
