#include <benchmark/benchmark.h>

#include <array>
#include <cstdint>
#include <iterator>
#include <string>

#include "move_generation.h"
#include "position.h"

namespace {

using Generator = int (*)(Position*, uint32_t*, bool);

struct TestPosition {
    const char* name;
    const char* fen;
};

constexpr TestPosition positions[] = {
    {
        "Start",
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
    },
    {
        "Kiwipete",
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
    },
    {
        "Middlegame",
        "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10"
    },
};

constexpr std::size_t MAX_MOVES = 256;

void FinishCounters(benchmark::State& state, int expected_count) {
    state.counters["moves_per_call"] =
        static_cast<double>(expected_count);

    state.counters["calls_per_second"] = benchmark::Counter(
        static_cast<double>(state.iterations()),
        benchmark::Counter::kIsRate
    );
}

void BM_GeneratePieceMoves(
    benchmark::State& state,
    std::size_t index,
    Generator generator
) {
    const auto& test = positions[index];

    Position position{std::string(test.fen)};
    std::array<uint32_t, MAX_MOVES> moves{};

    const int expected_count =
        generator(&position, moves.data(), false);

    for (auto _ : state) {
        int count = generator(&position, moves.data(), false);

        benchmark::DoNotOptimize(&count);

        if (count > 0) {
            benchmark::DoNotOptimize(&moves[count - 1]);
        }

        benchmark::ClobberMemory();
    }

    FinishCounters(state, expected_count);
}

void BM_GenerateLegalMoves(
    benchmark::State& state,
    std::size_t index
) {
    const auto& test = positions[index];

    Position position{std::string(test.fen)};
    std::array<uint32_t, MAX_MOVES> moves{};

    const int expected_count =
        generate_legal_moves(&position, moves.data(), false);

    for (auto _ : state) {
        int count =
            generate_legal_moves(&position, moves.data(), false);

        benchmark::DoNotOptimize(&count);

        if (count > 0) {
            benchmark::DoNotOptimize(&moves[count - 1]);
        }

        benchmark::ClobberMemory();
    }

    FinishCounters(state, expected_count);
}

void RegisterPieceBenchmarks(const char* name, Generator generator) {
    for (std::size_t i = 0; i < std::size(positions); ++i) {
        benchmark::RegisterBenchmark(
            (std::string("BM_GeneratePieceMoves/") + name + "/" +
             positions[i].name).c_str(),
            [i, generator](benchmark::State& state) {
                BM_GeneratePieceMoves(state, i, generator);
            }
        );
    }
}

void RegisterLegalMoveBenchmarks() {
    for (std::size_t i = 0; i < std::size(positions); ++i) {
        benchmark::RegisterBenchmark(
            (std::string("BM_GenerateLegalMoves/") +
             positions[i].name).c_str(),
            [i](benchmark::State& state) {
                BM_GenerateLegalMoves(state, i);
            }
        );
    }
}

const bool registered = [] {
    RegisterPieceBenchmarks("Pawns", generate_pawn_moves);
    RegisterPieceBenchmarks("Knights", generate_knight_moves);
    RegisterPieceBenchmarks("Bishops", generate_bishop_moves);
    RegisterPieceBenchmarks("Rooks", generate_rook_moves);
    RegisterPieceBenchmarks("Queens", generate_queen_moves);
    RegisterPieceBenchmarks("Kings", generate_king_moves);
    RegisterLegalMoveBenchmarks();
    return true;
}();

} // namespace

BENCHMARK_MAIN();
