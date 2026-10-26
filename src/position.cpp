#include "position.h"

#include "bitboard.h"
#include "castling_rights.h"
#include "evaluation.h"
#include "move.h"
#include "piece.h"
#include "utils.h"

namespace mozzarella {

/*
 * Constructor for a Position struct. Sets up the initial board state and data parameters.
 * If given a FEN string 
 */
Position::Position(std::string fen)
{
    // FENs are structured like
    // "[board] [side] [castling rights] [ep target] [halfmoves] [fullmoves]"

    // Split the FEN by spaces to get the individual components
    std::vector<std::string> fen_vec = split_str(fen, ' ');

    // Split the board by rank
    std::vector<std::string> board_vec = split_str(fen_vec[0], '/');

    // Iterate over the board vector
    int j = 8;
    int i = 56;

    for (int r = 0; r < 8; r++) {
        for (char c: board_vec[r]) {
            if ('1' <= c && c <= '8') {
                i += (c - '1' + 1);
            } else {
                bitboards[chtopl(c)][chtopc(c)] |= Bitboard{1ULL << i};
                i++;
            }
        }

        j--;
        i = (j - 1) * 8;
    }

    // Get the current player to move
    if (fen_vec[1] == "w") {
        player_to_move = Player::WHITE;
    } else if (fen_vec[1] == "b") {
        player_to_move = Player::BLACK;
    }

    // Zero out the castling rights
    for (int i = 0; i < 2; i++) {
        castling_rights[i] = (CastlingRights) 0;
    }

    for (char c: fen_vec[2]) {
        switch (c) {
            case 'k':
                castling_rights[Player::BLACK] |= CastlingRights::KING_UNMOVED;
                castling_rights[Player::BLACK] |= CastlingRights::KROOK;
                break;
            case 'q':
                castling_rights[Player::BLACK] |= CastlingRights::KING_UNMOVED;
                castling_rights[Player::BLACK] |= CastlingRights::QROOK;
                break;
            case 'K':
                castling_rights[Player::WHITE] |= CastlingRights::KING_UNMOVED;
                castling_rights[Player::WHITE] |= CastlingRights::KROOK;
                break;
            case 'Q':
                castling_rights[Player::WHITE] |= CastlingRights::KING_UNMOVED;
                castling_rights[Player::WHITE] |= CastlingRights::QROOK;
                break;
        }
    }

    castling_rights_stack.push_back(castling_rights[Player::WHITE]);
    castling_rights_stack.push_back(castling_rights[Player::BLACK]);

    // Get the en passant target square (not used yet)
    if (fen_vec[3] == "-") {
        ep_target = 0ULL;
    } else {
        int file = (char) fen_vec[3][0] - (char) 'a';
        int rank = (char) fen_vec[3][1] - (char) '1';

        ep_target = rank * 8 + file;
    }

    halfmoves = std::stoi(fen_vec[4]);
    fullmoves = std::stoi(fen_vec[5]);

    material_value[0] = 0;
    material_value[1] = 0;
    pst_eval = 0;

    // Calculate starting material value
    for (int pl = 0; pl < 2; pl++) {
        for (int pc = 0; pc < 5; pc++) {
            Bitboard tmp = bitboards[pl][pc];
            for (const auto sq : tmp) {
                material_value[pl] += piece_value((Piece) pc);
                pst_eval += PST[pl][pc][sq];
            }
        }
    }

    pst_eval_stack.push_back(pst_eval);
}


/*
 * Make the requested move on the board. A move is encoded in a 32-bit integer.
 * Updates applicable board state.
 *
 * Note: this method assumes that the move itself is legal.
 * Legality checking is performed elsewhere.
 */
void Position::make_move(uint32_t move) {
    // MOVE DECODING
    uint32_t move_flags = get_flags(move);
    Bitboard from_bb{1ULL << get_from_sq(move)};
    Bitboard to_bb{1ULL << get_to_sq(move)};

    if (from_bb & to_bb) return;

    Piece piece_moved = get_piece(move);


    // Handle move stack and castling rights stack
    move_stack.push_back(move);
    castling_rights_stack.push_back(castling_rights[Player::WHITE]);
    castling_rights_stack.push_back(castling_rights[Player::BLACK]);

    // Handle eval stack
    pst_eval_stack.push_back(pst_eval);

    // Update PST, removing the value from the starting square
    if (piece_moved != Piece::KING) {
        pst_eval -= PST[player_to_move][piece_moved][get_from_sq(move)];
    }

    // Handle forfeiting castling rights due to movement of pieces
    if (piece_moved == Piece::KING) {
        castling_rights[player_to_move] &= ~(CastlingRights::KING_UNMOVED);
    } else if (piece_moved == Piece::ROOK) {
        if (from_bb == Bitboard{1ULL << (56 * player_to_move)}) {
            castling_rights[player_to_move] &= ~CastlingRights::QROOK;
        } else if (from_bb == Bitboard{1ULL << (7 + 56 * player_to_move)}) {
            castling_rights[player_to_move] &= ~CastlingRights::KROOK;
        }
    }

    // CASTLES
    if (move_flags & (MoveFlags::CASTLE)) {
        // int king_from = 4 + 56 * player_to_move;
        // int king_to;
        int rook_from = 0;
        int rook_to = 0;

        if (move_flags & MoveFlags::KING_CASTLE) {
            // king_to = king_from + 2;
            rook_from = 7 + 56 * player_to_move;
            rook_to = 5 + 56 * player_to_move;

            castling_rights[player_to_move] &= ~(CastlingRights::KING_UNMOVED | CastlingRights::KROOK);
        } else if (move_flags & MoveFlags::QUEEN_CASTLE) {
            // king_to = king_from - 2;
            rook_from = 0 + 56 * player_to_move;
            rook_to = 3 + 56 * player_to_move;

            castling_rights[player_to_move] &= ~(CastlingRights::KING_UNMOVED | CastlingRights::QROOK);
        }

        // Move king
        // pst_eval -= PST[player_to_move][Piece::KING][king_from];
        // pst_eval += PST[player_to_move][Piece::KING][king_to];

        // Move rook
        bitboards[player_to_move][Piece::ROOK] &= Bitboard{~(1ULL << rook_from)};
        bitboards[player_to_move][Piece::ROOK] |=  Bitboard{1ULL << rook_to};
        pst_eval -= PST[player_to_move][Piece::ROOK][rook_from];
        pst_eval += PST[player_to_move][Piece::ROOK][rook_to];

    } else if (move_flags & MoveFlags::DOUBLE_PAWN_PUSH) {
        if (player_to_move == Player::WHITE) {
            // TODO: do we want to handle en passant this way?
        } else if (player_to_move == Player::BLACK) {
            // see above
        }
    }

    // CAPTURES
    if (move_flags & MoveFlags::CAPTURE) {
        Piece piece_captured = get_captured(move);
        material_value[1 - player_to_move] -= piece_value(piece_captured);

        if (!(move_flags & MoveFlags::EN_PASSANT)) {
            bitboards[1 - player_to_move][piece_captured] &= ~to_bb;

            pst_eval -= PST[1 - player_to_move][piece_captured][get_to_sq(move)];

            // If the captured piece was a rook in the corner, then opponent can no longer castle on that side
            if (piece_captured == Piece::ROOK) {
                if (to_bb == Bitboard{1ULL << (56 * (1 - player_to_move))}) {
                    castling_rights[1 - player_to_move] &= ~(CastlingRights::QROOK);
                } else if (to_bb == Bitboard{1ULL << (7 + 56 * (1 - player_to_move))}) {
                    castling_rights[1 - player_to_move] &= ~(CastlingRights::KROOK);
                }
            }
        } else {
            int captured_sq = (player_to_move == Player::WHITE) ? get_to_sq(move) - 8 : get_to_sq(move) + 8;
            pst_eval -= PST[1 - player_to_move][Piece::PAWN][captured_sq];
            if (player_to_move == Player::WHITE) {
                bitboards[1 - player_to_move][Piece::PAWN] &= ~(to_bb >> 8);
            } else if (player_to_move == Player::BLACK) {
                bitboards[1 - player_to_move][Piece::PAWN] &= ~(to_bb << 8);
            }
        }
    }

    // PROMOTIONS
    if (move_flags & (MoveFlags::PROMO)) {
        Piece promo_piece = get_promotion(move);

        bitboards[player_to_move][promo_piece] |= to_bb;

        // Material update
        material_value[player_to_move] += piece_value(promo_piece) -  piece_value(Piece::PAWN);

        // PST update
        pst_eval += PST[player_to_move][promo_piece][get_to_sq(move)];
        pst_eval -= PST[player_to_move][Piece::PAWN][get_from_sq(move)];

    } else {
        bitboards[player_to_move][piece_moved] |= to_bb;

        if (piece_moved != Piece::KING) {
            pst_eval += PST[player_to_move][piece_moved][get_to_sq(move)];
        }
    }

    // Remove the moved piece from the original square
    bitboards[player_to_move][piece_moved] &= ~from_bb;

    // Switch the turn
    player_to_move = Player(1 - player_to_move);
}


/*
 * Un-makes the requested move on the board. A move is encoded in a 32-bit integer.
 * Updates applicable board state.
 *
 * Note: this method assumes that the un-move itself is legal and was the most recent move.
 * Legality checking is performed elsewhere.
 */
void Position::unmake_move(uint32_t move) {
    player_to_move = (Player) (1 - player_to_move);

    // MOVE DECODING
    uint32_t move_flags = get_flags(move);
    Bitboard from_bb{1ULL << get_from_sq(move)};
    Bitboard to_bb{1ULL << get_to_sq(move)};
    if (from_bb & to_bb) return;
    Piece piece_moved = get_piece(move);

    // CASTLES
    if (move_flags & (MoveFlags::CASTLE)) {
        int rook_from = 0;
        int rook_to = 0;
        if (move_flags & MoveFlags::KING_CASTLE) {
            // Special adjustment of the rook bitboard in the event of a castle
            rook_from = 7 + 56 * player_to_move;
            rook_to = 5 + 56 * player_to_move;
        } else if (move_flags & MoveFlags::QUEEN_CASTLE) {
            rook_from = 0 + 56 * player_to_move;
            rook_to = 3 + 56 * player_to_move;
        }
        bitboards[player_to_move][Piece::ROOK] |= Bitboard{1ULL << rook_from};
        bitboards[player_to_move][Piece::ROOK] &= Bitboard{~(1ULL << rook_to)};
    } else if (move_flags & MoveFlags::DOUBLE_PAWN_PUSH) {
        if (player_to_move == Player::WHITE) {
            // TODO: do we want to handle en passant this way?
        } else if (player_to_move == Player::BLACK) {
            // see above
        }
    }

    // CAPTURES
    if (move_flags & MoveFlags::CAPTURE) {
        Piece piece_captured = get_captured(move);

        // Material restore
        material_value[1 - player_to_move] += piece_value(piece_captured);

        if (!(move_flags & MoveFlags::EN_PASSANT)) {
            bitboards[1 - player_to_move][piece_captured] |= to_bb;
        } else {
            if (player_to_move == Player::WHITE) {
                bitboards[1 - player_to_move][Piece::PAWN] |= to_bb >> 8;
            } else if (player_to_move == Player::BLACK) {
                bitboards[1 - player_to_move][Piece::PAWN] |= to_bb << 8;
            }
        }
    }

    // PROMOTIONS
    if (move_flags & (MoveFlags::PROMO)) {
        Piece promo_piece = get_promotion(move);
        bitboards[player_to_move][promo_piece] &= ~to_bb;

        // Material restore
        material_value[player_to_move] -= piece_value(promo_piece) - piece_value(Piece::PAWN);

    } else {
        bitboards[player_to_move][piece_moved] &= ~to_bb;
    }

    // Add the moved piece back to the original square
    bitboards[player_to_move][piece_moved] |= from_bb;

    // Handle move stack and castling rights stack
    move_stack.pop_back();

    // Pop the last eval
    pst_eval = pst_eval_stack.back();
    pst_eval_stack.pop_back();

    // Yes, I know this is kind of a weird way of handling the castling stack
    uint8_t b_last_rights = castling_rights_stack.back();
    castling_rights_stack.pop_back();
    uint8_t w_last_rights = castling_rights_stack.back();
    castling_rights_stack.pop_back();

    castling_rights[Player::BLACK] = b_last_rights;
    castling_rights[Player::WHITE] = w_last_rights;
}


/*
 * Gets the occupied squares of a given player in the form of a bitboard.
 */
Bitboard Position::get_occupied(int player) {
    Bitboard occ;
    for (int i = 0; i < 6; i++) {
        occ = occ | bitboards[player][i];
    }
    return occ;
}

} // namespace mozzarella
