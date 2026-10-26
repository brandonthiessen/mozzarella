#pragma once

#include <vector>
#include <iostream>

#include "bitboard.h"

inline void render_bitboard(mozzarella::Bitboard bb, int origin_sq = -1, char symbol = 'x') {
    std::vector<std::vector<char>> board(8, std::vector<char>(8, '.'));

    auto bits = bb.value();

    for (int i = 0; i < 64; i++) {
        if ((bits >> i) & 1ULL) {
            int row = 7 - (i / 8);
            int col = i % 8;
            board[row][col] = symbol;
        }
    }

    if (origin_sq != -1) {
        int row = 7 - (origin_sq / 8);
        int col = origin_sq % 8;
        board[row][col] = 'P';
    }

    for (const auto& row : board) {
        for (char c : row) {
            std::cout << c << ' ';
        }
        std::cout << '\n';
    }
    std::cout << '\n';
}
