// usbredir.c
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void print_bytes(const uint8_t *buf, size_t len) {
    printf("b'");
    for (size_t i = 0; i < len; i++) {
        printf("\\x%02x", buf[i]);
    }
    printf("'\n");
}


void hexdump(const uint8_t *buf, size_t len) {
    for (size_t i = 0; i < len; i += 8) {
        uint64_t addr = 0;
        size_t chunk_len = (i + 8 <= len) ? 8 : len - i;
        memcpy(&addr, buf + i, chunk_len);
        printf("  0x%016lx\n", addr);
    }
}
