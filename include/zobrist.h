#pragma once

#include <array>
#include <cstdint>

#include "position.h"

namespace mozzarella {

class Zobrist {
public:
    Zobrist();

    [[nodiscard]] uint64_t hash(const Position& position) const;

private:
    std::array<uint64_t, 781> table_;
};

extern Zobrist zobrist;

}
