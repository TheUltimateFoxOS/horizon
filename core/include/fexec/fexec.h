#pragma once

#include <stdint.h>

namespace fexec {
    constexpr uint32_t MAGIC = 0x45584546;

    typedef struct {
        uint32_t magic;
        uint8_t version;
        uint8_t reserved[11];

        uint64_t entry;
        uint64_t size;
    } fexec_header_t;
}