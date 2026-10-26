#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include "bitboard.h"
#include "piece.h"
#include "player.h"

namespace mozzarella {

// Position struct, containing all relevant game state information
class Position {
public:
    std::array<std::array<Bitboard, 6>, 2> bitboards{}; // indexed with [player][piece]
    Player player_to_move;
    uint8_t castling_rights[2];
    int halfmoves; // currently unused
    int fullmoves; // currently unused
    int ep_target; // currently unused
    int pst_eval;
    int material_value[2];
    std::vector<uint32_t> move_stack;
    std::vector<uint8_t> castling_rights_stack; // Push/pop 2 items at a time
    std::vector<int> pst_eval_stack; // Store recent position eval

    Position() : Position("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {}
    Position(std::string fen);

    void make_move(uint32_t move);
    void unmake_move(uint32_t move);

    Bitboard get_occupied(int player);

    uint64_t zobrist();

    int generate_legal_moves(uint32_t *moves, bool noisy_only);

    int generate_castle_moves(uint32_t *moves, bool noisy_only);

    int generate_en_passant_moves(uint32_t *moves, bool noisy_only);

    int generate_pawn_moves(uint32_t *moves, bool noisy_only);

    int generate_knight_moves(uint32_t *moves, bool noisy_only);
    int generate_king_moves(uint32_t *moves, bool noisy_only);

    int generate_bishop_moves(uint32_t *moves, bool noisy_only);
    int generate_rook_moves(uint32_t *moves, bool noisy_only);
    int generate_queen_moves(uint32_t *moves, bool noisy_only);

    int generate_sliding_moves(Piece piece, int deltas[], int ndeltas, uint32_t *moves, bool noisy_only);

    bool move_gives_check(uint32_t move);

    bool is_direct_attack(Piece moved_piece, int to_sq, int king_sq);
    bool is_discovered_check(int from_sq, int king_sq);

    bool is_in_check(Player player);
    bool is_in_sliding_check(Player player);

    bool is_in_check2(Player player, uint32_t move);
    bool is_in_sliding_check2(Player player, uint32_t move);
};
    
} // namespace mozzarella
