#include "perft.h"

#include <array>

#include "position.h"

namespace mozzarella {

uint64_t perft(Position& p, int depth) {
    if (depth == 0) return 1;
    std::array<uint32_t, 256> moves;
    int n = p.generate_legal_moves(moves.data(), false);
    if (depth == 1) return n;

    long total = 0;
    for (int i = 0; i < n; i++) {
        uint32_t m = moves[i];
        p.make_move(m);
        long count = perft(p, depth - 1);
        total += count;
        p.unmake_move(m);
    }

    return total;
}

}
