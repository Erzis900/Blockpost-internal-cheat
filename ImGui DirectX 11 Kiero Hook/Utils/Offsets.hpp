#include "../includes.h"

namespace Offsets {
    std::vector<uintptr_t> entityListOffsets = { 0x00C7BA08, 0x5C, 0xC };
    std::vector<uintptr_t> controllOffsets = { 0x00C7B94C, 0x5C };

    std::ptrdiff_t isMainPlayer = 0x8;
    std::ptrdiff_t team = 0x24;
    std::ptrdiff_t health = 0x38;
    std::ptrdiff_t pos = 0x60;
    std::ptrdiff_t name = 0x2C;

    std::ptrdiff_t yaw = 0x3C;
    std::ptrdiff_t pitch = 0x40;

    std::ptrdiff_t pos_controll = 0x108;
}