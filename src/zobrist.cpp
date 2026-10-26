#include "zobrist.h"

#include <chrono>
#include <cstdint>
#include <random>

#include "castling_rights.h"
#include "piece.h"
#include "position.h"

namespace mozzarella {

Zobrist zobrist; // Global Zobrist hashing object

/*
 * Initializes the randomized Zobrist hash values array.
 * The Zobrist array should be initialized at the creation of a new position.
 */
Zobrist::Zobrist() {
    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<uint64_t> dist;
    for (int i = 0; i < 781; i++) {
        table_[i] = dist(rng);
    }
}

/*
 * Gets the Zobrist hash of this position in the form of a 64-bit integer.
 */
uint64_t Zobrist::hash(const Position& p) const {
    uint64_t z = 0ULL;

    // XOR the values for each piece on each square of the board
    for (int i = 0; i < 12; i++) {
        Bitboard bb = p.bitboards[i / 6][i % 6];
        for (const auto sq : bb) {
            z ^= table_[i + sq * 12];
        }
    }

    // XOR the values for each castling right
    int wcr = p.castling_rights[Player::WHITE];
    int bcr = p.castling_rights[Player::BLACK];

    if (wcr & CastlingRights::KING_UNMOVED) {
        if (wcr & CastlingRights::KROOK) z ^= table_[768];
        if (wcr & CastlingRights::QROOK) z ^= table_[769];
    }

    if (bcr & CastlingRights::KING_UNMOVED) {
        if (bcr & CastlingRights::KROOK) z ^= table_[770];
        if (bcr & CastlingRights::QROOK) z ^= table_[771];
    }

    // Include the EP target in the hash
    if (p.ep_target != -1) {
        int ep_file = p.ep_target % 8;

        if ((ep_file > 0 && p.bitboards[p.player_to_move][Piece::PAWN] & Bitboard{1ULL << (p.ep_target - 1)}) ||
            (ep_file < 7 && p.bitboards[p.player_to_move][Piece::PAWN] & Bitboard{1ULL << (p.ep_target + 1)})) {
            z ^= table_[772 + ep_file];
        }
    }
    
    // Include the player to move in the hash
    if (p.player_to_move == Player::BLACK) z ^= table_[780];

    return z;
}

} // namespace mozzarella
