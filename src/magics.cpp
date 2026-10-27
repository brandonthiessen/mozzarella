#include "magics.h"
#include "magics_data.inc"

#include <cassert>

namespace mozzarella {

Bitboard get_bishop_attacks(int square, Bitboard occupancy) {
    assert(square >= 0 && square < 64);
    const auto blockers = occupancy.value() & GENERATED_BISHOP_RELEVANCY_MASKS[square];
    const auto index = (blockers * GENERATED_BISHOP_MAGICS[square])
                     >> GENERATED_BISHOP_SHIFTS[square];
    return Bitboard{GENERATED_BISHOP_ATTACKS[GENERATED_BISHOP_OFFSETS[square] + index]};
}

Bitboard get_rook_attacks(int square, Bitboard occupancy) {
    assert(square >= 0 && square < 64);
    const auto blockers = occupancy.value() & GENERATED_ROOK_RELEVANCY_MASKS[square];
    const auto index = (blockers * GENERATED_ROOK_MAGICS[square])
                     >> GENERATED_ROOK_SHIFTS[square];
    return Bitboard{GENERATED_ROOK_ATTACKS[GENERATED_ROOK_OFFSETS[square] + index]};
}

} // namespace mozzarella
