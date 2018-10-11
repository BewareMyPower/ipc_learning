// hex_dump.h
#pragma once
#include <assert.h>
#include <ctype.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

inline void PrintByteArray(void* buf, size_t n, size_t nmax,
                           char delim = ' ') noexcept {
  assert(n <= nmax);
  auto p = static_cast<unsigned char*>(buf);
  if (n > 0) {
    printf("%02x", p[0]);
  } else {
    printf("  ");
    n = 1;  // print (nmax-1)* 3 blanks in following code
  }

  for (size_t i = 1; i < n; i++) printf("%c%02x", delim, p[i]);
  for (size_t i = n; i < nmax; i++) printf("   ");
}

inline void PrintASCIIString(void* buf, size_t n) noexcept {
  auto p = static_cast<unsigned char*>(buf);
  for (size_t i = 0; i < n; i++) {
    if (isprint(p[i]))
      putchar(p[i]);
    else
      putchar('.');
  }
}

/*
 * sample output of file
00000000  68 65 6c 6c 6f 2c 20 77  6f 72 6c 64 2c 20 74 68  |hello, world, th|
00000010  69 73 20 69 73 20 78 79  7a 21 20 64 6f 20 79 6f  |is is xyz! do yo|
00000020  75 20 6b 6e 6f 77 20 6d  65 3f                    |u know me?|
0000002a
*/
inline void HexDump(void* buf, size_t n, size_t nLine = 16) noexcept {
  auto p = static_cast<unsigned char*>(buf);
  size_t nHalfLine = nLine / 2;
  uint32_t offset = 0;

  while (n > nLine) {
    printf("%08" PRIx32 "  ", offset);
    PrintByteArray(p, nHalfLine, nHalfLine, ' ');
    printf("  ");
    PrintByteArray(p + 8, nLine - nHalfLine, nLine - nHalfLine, ' ');
    printf("  |");
    PrintASCIIString(p, nLine);
    printf("|\n");

    offset += nLine;
    p += nLine;
    n -= nLine;
  }

  printf("%08" PRIx32 "  ", offset);
  if (n > 0) {
    if (n >= nHalfLine) {
      PrintByteArray(p, nHalfLine, nHalfLine, ' ');
      printf("  ");
      PrintByteArray(p + nHalfLine, n - nHalfLine, nLine - nHalfLine, ' ');
    } else {
      PrintByteArray(p, n, nHalfLine, ' ');
      printf("  ");
      PrintByteArray(p + nHalfLine, 0, nLine - nHalfLine, ' ');
    }
    printf("  |");
    PrintASCIIString(p, n);
    printf("|\n");

    offset += n;
    printf("%08" PRIx32 "\n", offset);
  }
}
