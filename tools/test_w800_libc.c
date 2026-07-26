#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static unsigned char source[544];
static unsigned char target[544];

int main(void)
{
    for (size_t i = 0; i < sizeof(source); i++) source[i] = (unsigned char)((i * 73U) ^ (i >> 2));

    for (size_t src_align = 0; src_align < 8; src_align++) {
        for (size_t dst_align = 0; dst_align < 8; dst_align++) {
            for (size_t len = 0; len <= 512; len++) {
                for (size_t i = 0; i < sizeof(target); i++) target[i] = 0x5AU;
                memcpy(target + dst_align, source + src_align, len);
                for (size_t i = 0; i < sizeof(target); i++) {
                    unsigned char expected = (i >= dst_align && i < dst_align + len)
                        ? source[src_align + i - dst_align] : 0x5AU;
                    if (target[i] != expected) {
                        printf("memcpy failed src=%zu dst=%zu len=%zu index=%zu\n",
                               src_align, dst_align, len, i);
                        return 1;
                    }
                }
            }
        }
    }

    for (size_t dst_align = 0; dst_align < 8; dst_align++) {
        for (size_t len = 0; len <= 512; len++) {
            for (size_t i = 0; i < sizeof(target); i++) target[i] = 0xA5U;
            memset(target + dst_align, 0x3C, len);
            for (size_t i = 0; i < sizeof(target); i++) {
                unsigned char expected =
                    (i >= dst_align && i < dst_align + len) ? 0x3CU : 0xA5U;
                if (target[i] != expected) {
                    printf("memset failed dst=%zu len=%zu index=%zu\n", dst_align, len, i);
                    return 1;
                }
            }
        }
    }

    puts("w800 libc alignment tests passed");
    return 0;
}
