#include <stdint.h>
#include <stdio.h>

#define REINTERPRET_CAST(T, val) ((T)(val))

int main() {
  int32_t i = 42;
  uint8_t *bytes = REINTERPRET_CAST(uint8_t *, &i);

  int32_t words[2] = {0x1234, 0x5678};
  int64_t c = *REINTERPRET_CAST(int64_t *, words);
}
