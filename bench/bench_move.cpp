#include <benchmark/benchmark.h>

#include <cstdint>
#include <string>

#include "move.h"
#include "position.h"

namespace {

struct TestPosition {
    const char* name;
    const char* fen;
    int move_from;
    int move_to;
};

constexpr TestPosition positions[] = {
    {
        "Start",
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        12, 28
    },
    {
        "Middlegame",
        "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10",
        18, 33
    },
    {
        "Capture",
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
        36, 53
    },
};

constexpr std::size_t MAX_MOVES = 256;

uint32_t FindMove(mozzarella::Position& position, int from, int to) {
    uint32_t moves[MAX_MOVES];
    const int count = position.generate_legal_moves(moves, false);

    for (int i = 0; i < count; ++i) {
        if (get_from_sq(moves[i]) == from &&
            get_to_sq(moves[i]) == to) {
            return moves[i];
        }
    }

    return 0;
}

void BM_MakeUnmake(benchmark::State& state, std::size_t index) {
    const auto& test = positions[index];
    mozzarella::Position position{std::string(test.fen)};
    const uint32_t move =
        FindMove(position, test.move_from, test.move_to);

    if (move == 0) {
        state.SkipWithError("Benchmark move is not legal");
        return;
    }

    // Make the Position's memory observable to the compiler
    mozzarella::Position* escaped_position = &position;
    benchmark::DoNotOptimize(escaped_position);

    for (auto _ : state) {
        position.make_move(move);
        benchmark::ClobberMemory();

        position.unmake_move(move);
        benchmark::ClobberMemory();

        benchmark::DoNotOptimize(escaped_position);
    }

    state.counters["moves_per_iteration"] = 1;
}

const bool registered = [] {
    for (std::size_t i = 0; i < std::size(positions); ++i) {
        benchmark::RegisterBenchmark(
            (std::string("BM_MakeUnmake/") + positions[i].name).c_str(),
            [i](benchmark::State& state) {
                BM_MakeUnmake(state, i);
            }
        );
    }
    return true;
}();

} // namespace

BENCHMARK_MAIN();
