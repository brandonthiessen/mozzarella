#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <string>

#include "perft.h"

namespace {

struct PerftCase {
    int depth;
    uint64_t expected_nodes;
};

void expect_perft_results(
    const std::string& fen,
    const PerftCase* cases,
    size_t count
) {
    for (size_t i = 0; i < count; ++i) {
        SCOPED_TRACE(::testing::Message()
                     << "depth = " << cases[i].depth
                     << ", FEN = " << fen);

        Position position(fen);
        EXPECT_EQ(
            perft(&position, cases[i].depth),
            cases[i].expected_nodes
        );
    }
}

TEST(PerftTest, StartPosition) {
    const std::string fen =
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    const PerftCase cases[] = {
        {0, 1},
        {1, 20},
        {2, 400},
        {3, 8902},
        {4, 197281},
    };

    expect_perft_results(fen, cases, std::size(cases));
}

TEST(PerftTest, Position2_Kiwipete) {
    const std::string fen =
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";

    const PerftCase cases[] = {
        {0, 1},
        {1, 48},
        {2, 2039},
        {3, 97862},
        {4, 4085603},
        {5, 193690690},
    };

    expect_perft_results(fen, cases, std::size(cases));
}

TEST(PerftTest, Position3_Endgame) {
    const std::string fen =
        "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1";

    const PerftCase cases[] = {
        {0, 1},
        {1, 14},
        {2, 191},
        {3, 2812},
        {4, 43238},
        {5, 674624},
        {6, 11030083},
    };

    expect_perft_results(fen, cases, std::size(cases));
}

TEST(PerftTest, Position4_PromotionsAndChecks) {
    const std::string fen =
        "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";

    const PerftCase cases[] = {
        {0, 1},
        {1, 6},
        {2, 264},
        {3, 9467},
        {4, 422333},
        {5, 15833292},
        {6, 706045033},
    };

    expect_perft_results(fen, cases, std::size(cases));
}

TEST(PerftTest, Position5_PromotionAndTactics) {
    const std::string fen =
        "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";

    const PerftCase cases[] = {
        {0, 1},
        {1, 44},
        {2, 1486},
        {3, 62379},
        {4, 2103487},
        {5, 89941194},
    };

    expect_perft_results(fen, cases, std::size(cases));
}

}  // namespace
