
#include <benchmark/benchmark.h>

#include <cstdint>
#include <string>

#include "perft.h"
#include "position.h"

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
        5
    },
    {
        "Kiwipete",
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
        4
    },
    {
        "Middlegame",
        "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10",
        4
    },
};

void BM_Perft(benchmark::State& state, const TestPosition& test) {
    Position position{std::string(test.fen)};
    const uint64_t expected_nodes = perft(&position, test.depth);

    for (auto _ : state) {
        const uint64_t nodes = perft(&position, test.depth);

        benchmark::DoNotOptimize(&nodes);
        benchmark::ClobberMemory();
    }

    state.counters["nodes"] = static_cast<double>(expected_nodes);
    state.counters["depth"] = test.depth;
    state.counters["nodes_per_second"] = benchmark::Counter(
        static_cast<double>(expected_nodes),
        benchmark::Counter::kIsRate
    );
}

const bool registered = [] {
    for (const auto& test : positions) {
        benchmark::RegisterBenchmark(
            (std::string("BM_Perft/") + test.name).c_str(),
            [&test](benchmark::State& state) {
                BM_Perft(state, test);
            }
        );
    }
    return true;
}();

} // namespace

BENCHMARK_MAIN();
