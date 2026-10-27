#pragma once

#include "bitboard.h"

namespace mozzarella {

Bitboard get_bishop_attacks(int square, Bitboard occupancy);
Bitboard get_rook_attacks(int square, Bitboard occupancy);

inline Bitboard get_queen_attacks(int square, Bitboard occupancy)
{
    return get_bishop_attacks(square, occupancy) | get_rook_attacks(square, occupancy);
}

} // namespace mozzarella
