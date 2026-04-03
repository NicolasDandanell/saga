#ifndef SAGA_PREPROCESSOR_H
#define SAGA_PREPROCESSOR_H

// A collection of preprocessor macros to aid in creating more complex macros
// Based on this wonderful article: http://jhnet.co.uk/articles/cpp_magic

// Another useful article for GCC compilers: https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html

// Macro Expansion
// ————————————————

#define SAGA_EVAL(...)     SAGA_EVAL1024(__VA_ARGS__)
#define SAGA_EVAL1024(...) SAGA_EVAL512(SAGA_EVAL512(__VA_ARGS__))
#define SAGA_EVAL512(...)  SAGA_EVAL256(SAGA_EVAL256(__VA_ARGS__))
#define SAGA_EVAL256(...)  SAGA_EVAL128(SAGA_EVAL128(__VA_ARGS__))
#define SAGA_EVAL128(...)  SAGA_EVAL64(SAGA_EVAL64(__VA_ARGS__))
#define SAGA_EVAL64(...)   SAGA_EVAL32(SAGA_EVAL32(__VA_ARGS__))
#define SAGA_EVAL32(...)   SAGA_EVAL16(SAGA_EVAL16(__VA_ARGS__))
#define SAGA_EVAL16(...)   SAGA_EVAL8(SAGA_EVAL8(__VA_ARGS__))
#define SAGA_EVAL8(...)    SAGA_EVAL4(SAGA_EVAL4(__VA_ARGS__))
#define SAGA_EVAL4(...)    SAGA_EVAL2(SAGA_EVAL2(__VA_ARGS__))
#define SAGA_EVAL2(...)    SAGA_EVAL1(SAGA_EVAL1(__VA_ARGS__))
#define SAGA_EVAL1(...)    __VA_ARGS__

#define SAGA_EXPAND_EMPTY() /* This macro expands to nothing */

#define SAGA_DEFER_1(x) x SAGA_EXPAND_EMPTY()
#define SAGA_DEFER_2(x) x SAGA_EXPAND_EMPTY SAGA_EXPAND_EMPTY()()
#define SAGA_DEFER_3(x) x SAGA_EXPAND_EMPTY SAGA_EXPAND_EMPTY SAGA_EXPAND_EMPTY()()()

// Preprocessor IF ELSE Logic Macros
// ——————————————————————————————————

#define SAGA_FIRST_(a, ...) a
#define SAGA_SECOND_(a, b, ...) b

#define SAGA_FIRST(...)  SAGA_FIRST_(__VA_ARGS__,)
#define SAGA_SECOND(...) SAGA_SECOND_(__VA_ARGS__,)

#define SAGA_IS_PROBE(...) SAGA_SECOND(__VA_ARGS__, 0)
#define SAGA_PROBE() ~, 1

#define SAGA_CAT(a,b) a ## b

#define SAGA_NOT(x) SAGA_IS_PROBE(SAGA_CAT(_SAGA_NOT_, x))
#define _SAGA_NOT_0 SAGA_PROBE()

#define SAGA_BOOL(x) SAGA_NOT(SAGA_NOT(x))

#define SAGA_IF_ELSE(condition) _SAGA_IF_ELSE(SAGA_BOOL(condition))
#define _SAGA_IF_ELSE(condition) SAGA_CAT(_SAGA_IF_, condition)

#define _SAGA_IF_1(...) __VA_ARGS__ _SAGA_IF_1_ELSE
#define _SAGA_IF_0(...)             _SAGA_IF_0_ELSE

#define _SAGA_IF_1_ELSE(...)
#define _SAGA_IF_0_ELSE(...) __VA_ARGS__

#define SAGA_HAS_ARGS(...) SAGA_BOOL(SAGA_FIRST(_SAGA_END_OF_ARGUMENTS_ __VA_ARGS__)())
#define _SAGA_END_OF_ARGUMENTS_() 0

#define SAGA_ADD(X) + X

#define _SAGA_TO_STRING(X) #X
#define SAGA_TO_STRING(X) _SAGA_TO_STRING(X) // Indirect call to force macro expansion

// __VA_ARGS__ expander
// —————————————————————

#define SAGA_ARGS_256(...) SAGA_ARGS_128(__VA_ARGS__) SAGA_ARGS_128(__VA_ARGS__)
#define SAGA_ARGS_128(...) SAGA_ARGS_64(__VA_ARGS__)  SAGA_ARGS_64(__VA_ARGS__)
#define SAGA_ARGS_64(...)  SAGA_ARGS_32(__VA_ARGS__)  SAGA_ARGS_32(__VA_ARGS__)
#define SAGA_ARGS_32(...)  SAGA_ARGS_16(__VA_ARGS__)  SAGA_ARGS_16(__VA_ARGS__)
#define SAGA_ARGS_16(...)  SAGA_ARGS_8(__VA_ARGS__)   SAGA_ARGS_8(__VA_ARGS__)
#define SAGA_ARGS_8(...)   SAGA_ARGS_4(__VA_ARGS__)   SAGA_ARGS_4(__VA_ARGS__)
#define SAGA_ARGS_4(...)   SAGA_ARGS_2(__VA_ARGS__)   SAGA_ARGS_2(__VA_ARGS__)
#define SAGA_ARGS_2(...)   SAGA_ARGS_1(__VA_ARGS__)   SAGA_ARGS_1(__VA_ARGS__)
#define SAGA_ARGS_1(...)   0,

#endif // SAGA_PREPROCESSOR_H
