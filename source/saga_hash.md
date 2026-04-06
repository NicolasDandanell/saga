This is an excerpt from the article in the following link (which is now login locked): http://lolengine.net/blog/2011/12/20/cpp-constant-string-hash

It can still be read at internet archive: https://web.archive.org/web/20210624162342/http://lolengine.net/blog/2011/12/20/cpp-constant-string-hash

**The following text is a copy of the main post:**

## C/C++ trick: static string hash generation

I am always interested in having the compiler do more things for me, without giving away code clarity or performance for the convenience. Today a colleague linked me to Pope Kim's ​Compile-Time Hash String Generation article which is a perfect example of the things I like: hidden syntactic sugar that does useful things.
Inline hash function

The goal: for a given hash function, write something like HASH_STRING("funny_bone") in the code, and have the compiler directly replace it with the result, 0xf1c6fd7f.

The solution: inline the function and hope that the compiler will be clever enough.

#include <string.h>
#define HASH(str) generateHash(str, strlen(str))

inline unsigned int generateHash(const char *string, size_t len)
{
    unsigned int hash = 0;
    for(size_t i = 0; i < len; ++i)
        hash = 65599 * hash + string[i];
    return hash ^ (hash >> 16);
}

Unfortunately Pope ran into several very problematic issues:

    requires heavy optimisation flags (/O2 with Visual Studio, -O3 with g++)
    limited to 10-character strings with Visual Studio
    limited to 17-character strings with g++

I could personally reproduce the g++ limitations. I believe they are more related to loop unrolling limits than to the actual string size, but they indeed make the technique unusable in practice.
Macro-based hash function

If you read my previous article about C/C++ preprocessor LUT generation, you may remember that it used preprocessor tricks to do loop unrolling.

Hence the following implementation:

#include <string.h>
#include <stdint.h>
#include <stdio.h>

#define H1(s,i,x)   (x*65599u+(uint8_t)s[(i)<strlen(s)?strlen(s)-1-(i):strlen(s)]) \
#define H4(s,i,x)   H1(s,i,H1(s,i+1,H1(s,i+2,H1(s,i+3,x)))) \
#define H16(s,i,x)  H4(s,i,H4(s,i+4,H4(s,i+8,H4(s,i+12,x)))) \
#define H64(s,i,x)  H16(s,i,H16(s,i+16,H16(s,i+32,H16(s,i+48,x)))) \
#define H256(s,i,x) H64(s,i,H64(s,i+64,H64(s,i+128,H64(s,i+192,x)))) \

#define HASH(s)    ((uint32_t)(H256(s,0,0)^(H256(s,0,0)>>16)))

It has the following properties:

    works in C in addition to C++
    strings are always optimised away by gcc or g++ (but not always the computation itself)
    hash computation is optimised away by gcc or g++ even with -O, -O1 or -Os
    string size limit is 256 characters (probably more than enough for most uses) and can be manually increased or decreased

The following code:

int main(void)
{
    printf("%08x\n", HASH("funny_bone"));
    printf("%08x\n", HASH("incredibly_large_string_that_gcc_groks_easily"));
}

Is (correctly) optimised to this with gcc -Os:

  ...
  movl    $-238617217, %esi
  movl    $.LC0, %edi
  xorl    %eax, %eax
  call    printf
  movl    $-453669173, %esi
  movl    $.LC0, %edi
  xorl    %eax, %eax
  call    printf
  ...

I haven't tested it with Visual Studio. Feedback from this compiler would be very appreciated!

    Posted: 2012-01-12 18:05 (Updated: 2012-01-12 18:07)
    Author: sam
    Categories: code tip
