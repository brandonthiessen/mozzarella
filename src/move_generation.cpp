#include <cmath>
#include <cstdint>
#include <vector>

#include "bitboard.h"
#include "castling_rights.h"
#include "magics.h"
#include "masks.h"
#include "move.h"
#include "piece.h"
#include "position.h"

namespace mozzarella {

int
Position::generate_legal_moves(uint32_t *moves, bool noisy_only)
{
    Player player = player_to_move;

    int n = 0;

    n += generate_pawn_moves(moves + n, noisy_only);
    n += generate_en_passant_moves(moves + n, noisy_only);
    n += generate_knight_moves(moves + n, noisy_only);
    n += generate_bishop_moves(moves + n, noisy_only);
    n += generate_rook_moves(moves + n, noisy_only);
    n += generate_queen_moves(moves + n, noisy_only);
    n += generate_king_moves(moves + n, noisy_only);

    // Cannot castle while you're in check
    if (!is_in_check(player)) {
        n += generate_castle_moves(moves + n, noisy_only);
    }

    // Filter for legal moves
    int j = 0;

    for (int i = 0; i < n; i++) {
        uint32_t m = moves[i];

        make_move(m);
        if (!is_in_check(player)) {
            moves[j++] = m;
        }
        unmake_move(m);
    }

    return j;
}


int
Position::generate_castle_moves(uint32_t *moves, bool noisy_only)
{
    int n = 0;

    Bitboard all_occupied = get_occupied(Player::WHITE) | get_occupied(Player::BLACK);
    
    // TODO: merge these shifts since they are the same, just different directions
    if (castling_rights[player_to_move] & CastlingRights::KING_UNMOVED) {
        if (castling_rights[player_to_move] & CastlingRights::QROOK) {
            bool castle_through_check = false;
            bitboards[player_to_move][Piece::KING] >>= 1;
            if (is_in_check(player_to_move)) castle_through_check = true;
            bitboards[player_to_move][Piece::KING] <<= 1;

            if (!castle_through_check) {
                if (player_to_move == Player::WHITE && !(W_QS_CASTLE_MASK & all_occupied)) {
                    uint32_t move = encode_move(4, 2, Piece::KING, 0, 0, MoveFlags::QUEEN_CASTLE);

                    // Noisy-only filtering
                    if (noisy_only) {
                        // We need to use the regular check detection for castles
                        make_move(move);
                        if (is_in_check(Player::BLACK)) {
                            moves[n++] = move;
                        }
                        unmake_move(move);
                    }
                    else {
                        moves[n++] = move;

                    }
                } else if (player_to_move == Player::BLACK && !(B_QS_CASTLE_MASK & all_occupied)) {
                    uint32_t move = encode_move(60, 58, Piece::KING, 0, 0, MoveFlags::QUEEN_CASTLE);

                    // Noisy-only filtering
                    if (noisy_only) {
                        // We need to use the regular check detection for castles
                        make_move(move);
                        if (is_in_check(Player::WHITE)) {
                            moves[n++] = move;
                        }
                        unmake_move(move);
                    }
                    else {
                        moves[n++] = move;

                    }

                }
            }
        }

        if (castling_rights[player_to_move] & CastlingRights::KROOK) {
            bool castle_through_check = false;
            bitboards[player_to_move][Piece::KING] <<= 1;
            if (is_in_check(player_to_move)) castle_through_check = true;
            bitboards[player_to_move][Piece::KING] >>= 1;

            if (!castle_through_check) {
                if (player_to_move == Player::WHITE && !(W_KS_CASTLE_MASK & all_occupied)) {
                    uint32_t move = encode_move(4, 6, Piece::KING, 0, 0, KING_CASTLE);

                    // Noisy-only filtering
                    if (noisy_only) {
                        // We need to use the regular check detection for castles
                        make_move(move);
                        if (is_in_check(Player::BLACK)) {
                            moves[n++] = move;
                        }
                        unmake_move(move);
                    }
                    else {
                        moves[n++] = move;
                    }

                } else if (player_to_move == Player::BLACK && !(B_KS_CASTLE_MASK & all_occupied)) {
                    uint32_t move = encode_move(60, 62, Piece::KING, 0, 0, KING_CASTLE);

                    // Noisy-only filtering
                    if (noisy_only) {
                        // We need to use the regular check detection for castles
                        make_move(move);
                        if (is_in_check(Player::WHITE)) {
                            moves[n++] = move;
                        }
                        unmake_move(move);
                    }
                    else {
                        moves[n++] = move;
                    }
                }
            }
        }
    }

    return n;
}

int
Position::generate_en_passant_moves(uint32_t *moves, bool noisy_only)
{
    (void) noisy_only;
    int n = 0;

    if (!(move_stack.empty()) && get_flags(move_stack.back()) == MoveFlags::DOUBLE_PAWN_PUSH) {
        Bitboard occupied = (get_occupied(Player::WHITE) | get_occupied(Player::BLACK));
        
        int push_to_sq = get_to_sq(move_stack.back());
        int to_sq = push_to_sq + (1 - 2 * (player_to_move)) * 8;
        int double_push_file = push_to_sq % 8;

        if (!(Bitboard{1ULL << to_sq} & occupied)) {
            if (bitboards[player_to_move][Piece::PAWN] & Bitboard{1ULL << (push_to_sq - 1)} && double_push_file != 0) {
                moves[n++] = encode_move(push_to_sq - 1, to_sq, Piece::PAWN, Piece::PAWN, 0, MoveFlags::EN_PASSANT | MoveFlags::CAPTURE);
            }

            if (bitboards[player_to_move][Piece::PAWN] & Bitboard{1ULL << (push_to_sq + 1)} && double_push_file != 7) {
                moves[n++] = encode_move(push_to_sq + 1, to_sq, Piece::PAWN, Piece::PAWN, 0, MoveFlags::EN_PASSANT | MoveFlags::CAPTURE);
            }
        }
    }

    return n;
}

int
Position::generate_pawn_moves(uint32_t *moves, bool noisy_only)
{
    int n = 0;

    Player player = player_to_move;
    Player opponent = Player (1 - player);

    Bitboard occupied = get_occupied(player);
    Bitboard opponent_occupied = get_occupied(opponent);
    Bitboard all_occupied = occupied | opponent_occupied;
    Bitboard pawn_bb = bitboards[player_to_move][Piece::PAWN];

    for (const auto pawn_sq : pawn_bb) {

        Bitboard all_pawn_advances = PAWN_ADVANCE_MASKS[player][pawn_sq] & ~(all_occupied);

        // Edge case: a pawn cannot advance two steps if obstructed
        // TODO: change this to use bitboards and shifting directly (there is an (incomplete) Python implementation of this)
        if (player == Player::WHITE && 8 <= pawn_sq && pawn_sq <= 15) {
            if (all_occupied & Bitboard{1ULL << (pawn_sq + 8)}) {
                all_pawn_advances &= Bitboard{~(1ULL << (pawn_sq + 16))};
            }
        } else if (player == Player::BLACK && 48 <= pawn_sq && pawn_sq <= 55) {
            if (all_occupied & Bitboard{1ULL << (pawn_sq - 8)}) {
                all_pawn_advances &= Bitboard{~(1ULL << (pawn_sq - 16))};
            }
        }

        // This is kind of ridiculous.
        // Why am I iterating over a mask which will (83% of the time) have one item?
        for (const auto to_sq : all_pawn_advances) {

            uint32_t flags = 0;

            // If the target square is a promotion
            if ((56 * (opponent)) <= to_sq && to_sq <= (7 + 56 * (opponent))) {
                moves[n++] = encode_move(pawn_sq, to_sq, Piece::PAWN, 0, Piece::QUEEN, MoveFlags::QUEEN_PROMO);
                moves[n++] = encode_move(pawn_sq, to_sq, Piece::PAWN, 0, Piece::ROOK, MoveFlags::ROOK_PROMO);
                moves[n++] = encode_move(pawn_sq, to_sq, Piece::PAWN, 0, Piece::BISHOP, MoveFlags::BISHOP_PROMO);
                moves[n++] = encode_move(pawn_sq, to_sq, Piece::PAWN, 0, Piece::KNIGHT, MoveFlags::KNIGHT_PROMO);
                break;
            }
            if (std::abs(pawn_sq - to_sq) == 16) {
                flags = DOUBLE_PAWN_PUSH;
            }
            if (noisy_only) {
                uint32_t move = encode_move(pawn_sq, to_sq, Piece::PAWN, 0, 0, flags);
                if (move_gives_check(move)) {
                    moves[n++] = move;
                }
            } else {
                moves[n++] = encode_move(pawn_sq, to_sq, Piece::PAWN, 0, 0, flags);
            }
        }

        Bitboard all_pawn_attacks = PAWN_ATTACK_MASKS[player][pawn_sq] & opponent_occupied;

        constexpr Piece attackable_pieces[] = {Piece::PAWN, Piece::KNIGHT, Piece::BISHOP, Piece::ROOK, Piece::QUEEN};

        for (const auto to_sq : all_pawn_attacks) {

            for (Piece attackable_piece: attackable_pieces) {
                if (bitboards[opponent][attackable_piece] & Bitboard{1ULL << to_sq}) {
                    if ((56 * opponent) <= to_sq && to_sq <= (7 + 56 * opponent)) {
                        moves[n++] = encode_move(pawn_sq, to_sq, Piece::PAWN, attackable_piece, Piece::QUEEN, MoveFlags::QUEEN_PROMO | MoveFlags::CAPTURE);
                        moves[n++] = encode_move(pawn_sq, to_sq, Piece::PAWN, attackable_piece, Piece::ROOK, MoveFlags::ROOK_PROMO | MoveFlags::CAPTURE);
                        moves[n++] = encode_move(pawn_sq, to_sq, Piece::PAWN, attackable_piece, Piece::BISHOP, MoveFlags::BISHOP_PROMO | MoveFlags::CAPTURE);
                        moves[n++] = encode_move(pawn_sq, to_sq, Piece::PAWN, attackable_piece, Piece::KNIGHT, MoveFlags::KNIGHT_PROMO | MoveFlags::CAPTURE);
                        break;
                    }
                    moves[n++] = encode_move(pawn_sq, to_sq, Piece::PAWN, attackable_piece, 0, MoveFlags::CAPTURE);
                }
            }
        }
    }

    return n;
}

int
Position::generate_knight_moves(uint32_t *moves, bool noisy_only)
{
    int n = 0;

    Player player = player_to_move;
    Player opponent = (Player)(1 - player_to_move);

    Bitboard occupied = get_occupied(player);
    Bitboard opponent_occupied = get_occupied(opponent);

    Bitboard knight_bb = bitboards[player][Piece::KNIGHT];

    constexpr Piece attackable_pieces[] = {Piece::PAWN, Piece::KNIGHT, Piece::BISHOP, Piece::ROOK, Piece::QUEEN};

    for (const auto knight_sq : knight_bb) {
        Bitboard all_knight_moves = KNIGHT_MOVE_MASKS[knight_sq] & ~occupied;

        for (const auto to_sq : all_knight_moves) {
            Bitboard to_bb{1ULL << to_sq};

            if (opponent_occupied & to_bb) {
                for (Piece attackable_piece : attackable_pieces) {
                    if (bitboards[opponent][attackable_piece] & to_bb) {
                        // Always store captures
                        uint32_t move = encode_move(knight_sq, to_sq, Piece::KNIGHT, attackable_piece, 0, MoveFlags::CAPTURE);
                        moves[n++] = move;
                    }
                }
            } else {
                uint32_t move = encode_move(knight_sq, to_sq, Piece::KNIGHT, 0, 0, 0);
                if (noisy_only) {
                    if (!(move_gives_check(move))) {
                        continue;
                    }
                }
                moves[n++] = move;
            }
        }
    }

    return n;
}

int
Position::generate_king_moves(uint32_t *moves, bool noisy_only)
{
    int n = 0;

    Player opponent = (Player)(1 - player_to_move);
    Bitboard king_bb = bitboards[player_to_move][Piece::KING];

    int king_sq = king_bb.pop_lsb();

    Bitboard king_move_bb = KING_MOVE_MASKS[king_sq] & ~get_occupied(player_to_move);

    Bitboard captures = KING_MOVE_MASKS[king_sq] & get_occupied(opponent);

    if (!noisy_only) {
        Bitboard non_captures = king_move_bb & ~get_occupied(opponent);

        for (const auto to_sq : non_captures) {
            uint32_t move = encode_move(king_sq, to_sq, Piece::KING, 0, 0, 0);
            moves[n++] = move;
        }
    } else {
        // Noisy only king move generation - only need to check to see if a move gives discovered check
        Bitboard non_captures = king_move_bb & ~get_occupied(opponent);
        for (const auto to_sq : non_captures) {
            uint32_t move = encode_move(king_sq, to_sq, Piece::KING, 0, 0, 0);
            if (move_gives_check(move)) {
                moves[n++] = move;
            }
        }
    }

    constexpr Piece attackable_pieces[] = {Piece::PAWN, Piece::KNIGHT, Piece::BISHOP, Piece::ROOK, Piece::QUEEN};
    for (const auto to_sq : captures) {
        Bitboard to_bb{1ULL << to_sq};

        for (const Piece attackable_piece : attackable_pieces) {
            if (to_bb & bitboards[opponent][attackable_piece]) {
                moves[n++] = encode_move(king_sq, to_sq, Piece::KING, attackable_piece, 0, MoveFlags::CAPTURE);
            }
        }
    }

    return n;
}

int
Position::generate_bishop_moves(uint32_t *moves, bool noisy_only)
{
    return generate_sliding_moves(
        Piece::BISHOP, moves, noisy_only);
}

int
Position::generate_rook_moves(uint32_t *moves, bool noisy_only)
{
    return generate_sliding_moves(
        Piece::ROOK, moves, noisy_only);
}

int
Position::generate_queen_moves(uint32_t *moves, bool noisy_only)
{
    return generate_sliding_moves(
        Piece::QUEEN, moves, noisy_only);
}

int
Position::generate_sliding_moves(Piece piece, uint32_t *moves, bool noisy_only)
{
    Player player = player_to_move;
    Player opponent = static_cast<Player>(1 - player);

    const Bitboard occupied = get_occupied(player);
    const Bitboard opponent_occupied = get_occupied(opponent);
    const Bitboard occupancy = occupied | opponent_occupied;

    constexpr Piece attackable_pieces[] = {
        Piece::PAWN,
        Piece::KNIGHT,
        Piece::BISHOP,
        Piece::ROOK,
        Piece::QUEEN
    };

    Bitboard piece_sqs = bitboards[player][piece];

    // Number of moves generated
    int n = 0;

    for (const auto piece_sq : piece_sqs) {
        Bitboard attacks;
        switch (piece) {
            case Piece::BISHOP:
                attacks = get_bishop_attacks(piece_sq, occupancy);
                break;
            case Piece::ROOK:
                attacks = get_rook_attacks(piece_sq, occupancy);
                break;
            case Piece::QUEEN:
                attacks = get_queen_attacks(piece_sq, occupancy);
                break;
            default: // Only bishop/rook/queen use sliding move generation
                return n;
        }

        attacks &= ~occupied;

        for (const auto to_sq : attacks) {
            const Bitboard to_bb{1ULL << to_sq};
            if (opponent_occupied & to_bb) {
                // Find the captured piece to encode the capture correctly
                for (const Piece attackable_piece : attackable_pieces) { // TODO: experiment with having quicker access to what piece is on a given square
                    if (bitboards[opponent][attackable_piece] & to_bb) {
                        moves[n++] = encode_move(piece_sq, to_sq, piece, attackable_piece, 0, MoveFlags::CAPTURE);
                        break;
                    }
                }

                continue;
            }

            // Quiet move
            const uint32_t move = encode_move(
                piece_sq, to_sq, piece, 0, 0, 0
            );

            if (!noisy_only || move_gives_check(move)) {
                moves[n++] = move;
            }
        }
    }

    return n;
}


// Checks to see if playing the move puts the opposing king in check.
// Only assumes that move is pseudolegal.
// Assumes that the move has not been played on position p.
// NOTE: This function (and its helpers) *do not* take pawn promotions into consideration,
// since they are only intended to be used for noisy move filtering. If a pawn is being
// promoted, the move is already noisy! As such, the function is not 100% "correct".
bool
Position::move_gives_check(uint32_t move)
{
    Player player = player_to_move;
    Player opponent = (Player)(1 - player);

    // Find the opposing king square
    Bitboard king_bb = bitboards[opponent][Piece::KING];
    int king_sq = king_bb.pop_lsb();

    // Get the piece type
    Piece moved_piece = get_piece(move);

    // Get the destination square
    int to_sq = get_to_sq(move);
    int from_sq = get_from_sq(move);

    // Check direct attack by moved piece
    if (is_direct_attack(moved_piece, to_sq, king_sq)) {
        return true;
    }

    // Check discovered attack
    if (is_discovered_check(from_sq, king_sq)) {
        return true;
    }

    return false;
}


bool
Position::is_direct_attack(Piece moved_piece, int to_sq, int king_sq)
{
    Player player = player_to_move;
    Bitboard king_bb{1ULL << king_sq};

    switch (moved_piece) {
        case Piece::PAWN:
            return !(PAWN_ATTACK_MASKS[player][to_sq] & king_bb).empty();
        case Piece::KNIGHT:
            return !(KNIGHT_MOVE_MASKS[to_sq] & king_bb).empty();
        default:
            break;
    }

    Bitboard attack_mask{0};

    switch (moved_piece) {
        case Piece::BISHOP:
            attack_mask = BISHOP_MOVE_MASKS[to_sq];
            break;
        case Piece::ROOK:
            attack_mask = ROOK_MOVE_MASKS[to_sq];
            break;
        case Piece::QUEEN:
            attack_mask = BISHOP_MOVE_MASKS[to_sq] | ROOK_MOVE_MASKS[to_sq];
            break;
        default:
            break;
    }

    if (!(attack_mask & king_bb)) {
        return false;
    }

    int kr = king_sq / 8, kf = king_sq % 8;
    int tr = to_sq / 8, tf = to_sq % 8;

    int dr = kr - tr, df = kf - tf;

    int sr = (dr == 0) ? 0 : (dr > 0 ? 1 : -1);
    int sf = (df == 0) ? 0 : (df > 0 ? 1 : -1);

    int step = sr * 8 + sf;

    Bitboard occupied = (get_occupied(Player::WHITE) | get_occupied(Player::BLACK));
    occupied &= ~king_bb;

    for (int sq = to_sq + step; sq != king_sq; sq += step) {
        if (occupied & Bitboard{1ULL << sq}) {
            return false;
        }
    }

    return true;
}

bool
Position::is_discovered_check(int from_sq, int king_sq)
{
    Bitboard from_bb{1ULL << from_sq};
    Bitboard king_bb{1ULL << king_sq};
    Player player = player_to_move;

    // Isolate the direction
    int kr = king_sq / 8, kf = king_sq % 8;
    int fr = from_sq / 8, ff = from_sq % 8;

    int dr = kr - fr, df = kf - ff;

    // Check for geometrically impossible rays
    if (!(dr == 0 || df == 0 || abs(dr) == abs(df))) {
        return false;
    }

    // Normalize and invert the direction (we're going king square -> away from from_square)
    int sr = (dr == 0) ? 0 : (dr > 0 ? -1 : 1);
    int sf = (df == 0) ? 0 : (df > 0 ? -1 : 1);

    int step = sr * 8 + sf;

    Bitboard threats = bitboards[player][Piece::QUEEN];
    if (abs(sr) == abs(sf)) {
        threats |= bitboards[player][Piece::BISHOP];
    } else {
        threats |= bitboards[player][Piece::ROOK];
    }
    threats &= ~from_bb;

    Bitboard non_threats = ((get_occupied(Player::WHITE) | get_occupied(Player::BLACK)) & ~king_bb) & ~threats;

    for (int sq = king_sq + step; sq >= 0 && sq < 64; sq += step) {
        // Prevent wrapping across file boundaries
        int prev_file = (sq - step) % 8;
        int curr_file = sq % 8;

        // If files are too far apart, we've wrapped
        if (abs(curr_file - prev_file) > 1) {
            return false;
        }

        Bitboard sq_bb{1ULL << sq};

        if (threats & sq_bb) {
            return true;
        }

        if (non_threats & sq_bb) {
            return false;
        }
    }

    // Will this ever get reached?
    // LOG("discovered check: reached final condition")
    return false;

}

bool
Position::is_in_check(Player player)
{
    Player opponent = (Player)(1 - player);
    Bitboard king_bb = bitboards[player][Piece::KING];
    int king_sq = king_bb.pop_lsb();

    if (KNIGHT_MOVE_MASKS[king_sq] & bitboards[opponent][Piece::KNIGHT]) {
        return true;
    }
    if (PAWN_ATTACK_MASKS[player][king_sq] & bitboards[opponent][Piece::PAWN]) {
        return true;
    }
    if (KING_MOVE_MASKS[king_sq] & bitboards[opponent][Piece::KING]) {
        return true;
    }

    return is_in_sliding_check(player);
}

bool
Position::is_in_sliding_check(Player player)
{
    // Locate the king square
    Bitboard king_bb = bitboards[player][Piece::KING];
    int king_sq = king_bb.pop_lsb();
    int king_file = king_sq % 8;
    Player opponent = (Player)(1 - player);

    // Opponent's sliding piece bitboards
    Bitboard opp_bishops_and_queens = bitboards[opponent][Piece::BISHOP] | bitboards[opponent][Piece::QUEEN];
    Bitboard opp_rooks_and_queens = bitboards[opponent][Piece::ROOK] | bitboards[opponent][Piece::QUEEN];

    Bitboard all_occupied = (get_occupied(Player::WHITE) | get_occupied(Player::BLACK));

    // Sliding directions
    Bitboard sliding_bbs[] = {opp_bishops_and_queens, opp_rooks_and_queens};
    int deltas[2][4] = {{-9, -7, 7, 9}, {-8, -1, 1, 8}};

    // Loop over bishop- and rook-like attackers
    for (int i = 0; i < 2; i++) {
        Bitboard occupied_no_sliding = (all_occupied & ~(sliding_bbs[i]));
        for (int j = 0; j < 4; j++) {
            int curr_file = king_file;

            for (int step = 1; step < 8; step++) {
                int to_sq = king_sq + deltas[i][j] * step;
                if (to_sq < 0 || to_sq > 63) break;

                int to_file = to_sq % 8;
                if (abs(to_file - curr_file) > 1) break;

                Bitboard to_bb{1ULL << to_sq};

                if (occupied_no_sliding & to_bb) break;

                if (sliding_bbs[i] & to_bb) {
                    return true;
                }

                curr_file = to_file;
            }
        }
    }

    return false;
}

} // namespace mozzarella
