#include <benchmark/benchmark.h>

#include <cstddef>
#include <string>

#include "position.h"
#include "search.h"

namespace {

struct TestPosition {
    const char* name;
    const char* fen;
    int depth;
};

constexpr TestPosition positions[] = {
    {
        "Start",
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        4,
    },
    { "Middlegame",
        "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10",
        4,
    },
    {
        "Tactical",
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
        4,
    },
};

void BM_SearchFixedDepth(benchmark::State& state, std::size_t index) {
    const auto& test = positions[index];

    for (auto _ : state) {
        state.PauseTiming();

        mozzarella::Position position{std::string(test.fen)};
        mozzarella::MovePicker move_picker{64};

        state.ResumeTiming();

        const auto best = move_picker.find_best_move_fixed_depth(position, test.depth);

        if (best == 0) {
            state.SkipWithError("Search returned an invalid move");
            break;
        }
    }

    state.counters["searches_per_second"] =
        benchmark::Counter(
            static_cast<double>(state.iterations()),
            benchmark::Counter::kIsRate);
}

const bool registered = [] {
    for (std::size_t i = 0; i < std::size(positions); ++i) {
        benchmark::RegisterBenchmark(
            (std::string("BM_SearchFixedDepth/") + positions[i].name).c_str(),
            [i](benchmark::State& state) {
                BM_SearchFixedDepth(state, i);
            }
        );
    }
    return true;
}();

} // namespace

BENCHMARK_MAIN();
