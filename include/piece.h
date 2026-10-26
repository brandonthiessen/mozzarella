#pragma once

#include <cstdint>

enum Piece : uint8_t {
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
};

constexpr int piece_value(Piece piece) {
    constexpr int values[] = {100, 320, 330, 500, 900, 0};
    return values[piece];
}
