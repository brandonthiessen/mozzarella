#pragma once

#include <cstdint>

enum CastlingRights: uint8_t {
    KING_UNMOVED  = 1 << 0,
    QROOK = 1 << 1,
    KROOK = 1 << 2
};
