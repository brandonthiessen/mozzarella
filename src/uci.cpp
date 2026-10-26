#include "uci.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "log.h"
#include "move.h"
#include "perft.h"
#include "piece.h"
#include "player.h"
#include "position.h"
#include "search.h"
#include "tt.h"
#include "utils.h"

int main() {
    debug_log.open("logs/debug.log", std::ios::trunc);
    LOG("ENGINE START");

    std::ios::sync_with_stdio(false);

    uci_loop();

    return 0;
}

void uci_loop() {
    std::string line;
    mozzarella::Position pos;
    mozzarella::MovePicker mp{64};

    while (std::getline(std::cin, line)) {
        if (line == "uci") {
            std::cout << "id name Mozzarella" << std::endl;
            std::cout << "id author Brandon Thiessen" << std::endl;
            std::cout << "uciok" << std::endl;
        }
        else if (line == "ucinewgame") {
            mp.tt.clear();
        }
        else if (line == "isready") {
            std::cout << "readyok" << std::endl;
        }
        else if (line.rfind("position", 0) == 0) {
            std::istringstream iss(line);
            std::string token;
            iss >> token;  // read "position"
            iss >> token;  // read "startpos" or "fen"

            if (token == "startpos") {
                pos = mozzarella::Position("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
            }
            else if (token == "fen") {
                // Read the FEN string (6 space-separated parts)
                std::string fen_str;
                for (int i = 0; i < 6; i++) {
                    if (iss >> token) {
                        if (i > 0) fen_str += " ";
                        fen_str += token;
                    }
                }
                pos = mozzarella::Position(fen_str);
            }

            // Parse and apply moves if present
            if (iss >> token && token == "moves") {
                std::string move_str;
                while (iss >> move_str) {
                    uint32_t m = parse_move(pos, move_str);
                    pos.make_move(m);
                }
            }
        }
        else if (line.rfind("go", 0) == 0) {

            int wtime = -1, btime = -1;
            int winc = 0, binc = 0; // currently unused
            int movetime = -1;
            int depth = -1;

            std::istringstream iss(line);
            std::string token;
            iss >> token; // read "go"

            // Perft mode options via "go perft N"
            bool perft_mode = false;
            int perft_depth = -1;

            // Read in all values from "go" command
            while (iss >> token) {
                if (token == "perft") {
                    perft_mode = true;
                    iss >> perft_depth;
                }
                if (token == "wtime") iss >> wtime;
                else if (token == "btime") iss >> btime;
                else if (token == "winc") iss >> winc;
                else if (token == "binc") iss >> binc;
                else if (token == "movetime") iss >> movetime;
                else if (token == "depth") iss >> depth;
            }

            if (perft_mode) {
                std::array<uint32_t, 256> moves;
                int n = pos.generate_legal_moves(moves.data(), false);

                long total_nodes = 0;

                for (int i = 0; i < n; i++) {
                    uint32_t m = moves[i];

                    pos.make_move(m);
                    long nodes = perft(pos, perft_depth - 1);
                    total_nodes += nodes;
                    pos.unmake_move(m);

                    std::cout << mozzarella::move_to_string(m) << ": " << nodes << std::endl;
                }

                std::cout << "\nNodes searched: " << total_nodes << std::endl;
                continue;
            }

            int remaining = (pos.player_to_move == Player::WHITE) ? wtime : btime;

            int time_budget = (movetime != -1)
                ? movetime
                : std::max(100, remaining > 0 ? remaining / 50 : 100);

            uint32_t best_move;

            // Search either to a fixed depth depending on "go" request
            if (depth != -1) {
                best_move = mp.find_best_move_fixed_depth(pos, depth);
            } else {
                best_move = mp.find_best_move(pos, time_budget);
            }

            // Output the move to CLI
            std::cout << "bestmove " << mozzarella::move_to_string(best_move) << std::endl;

        }
        else if (line == "quit") {
            break;
        }
    }
}

uint32_t parse_move(mozzarella::Position pos, std::string move_str) {
    int from_sq = mozzarella::coord_to_sq(move_str.substr(0, 2));
    int to_sq = mozzarella::coord_to_sq(move_str.substr(2, 4));

    uint32_t flags = 0;

    // Get the promotion piece if it exists
    Piece promo_piece = Piece::PAWN;
    if (move_str.size() == 5) {
        char c = move_str[4];
        switch (c) {
            case 'q': 
                promo_piece = Piece::QUEEN;
                flags |= (MoveFlags::QUEEN_PROMO);
                break;
            case 'n': 
                promo_piece = Piece::KNIGHT;
                flags |= (MoveFlags::KNIGHT_PROMO);
                break;
            case 'b': 
                promo_piece = Piece::BISHOP;
                flags |= (MoveFlags::BISHOP_PROMO);
                break;
            case 'r': 
                promo_piece = Piece::ROOK;
                flags |= (MoveFlags::ROOK_PROMO);
                break;
        }
    }

    // Get the piece that was moved
    Piece piece_moved = Piece::PAWN;
    for (int i = 0; i < 6; i++) {
        if (pos.bitboards[pos.player_to_move][i] & mozzarella::Bitboard{1ULL << from_sq}) {
            piece_moved = (Piece) i;
            break;
        }
    }

    // Check if the move was a castle (king moved two squares)
    if (piece_moved == Piece::KING) {
        if (from_sq + 2 == to_sq) {
            flags |= MoveFlags::KING_CASTLE;
        } else if (from_sq - 2 == to_sq) {
            flags |= MoveFlags::QUEEN_CASTLE;
        }
    }

    // Figure out if a piece has been captured
    int piece_captured = Piece::PAWN;
    for (int i = 0; i < 5; i++) {
        if (pos.bitboards[1 - pos.player_to_move][i] & mozzarella::Bitboard{1ULL << to_sq}) {
            piece_captured = (Piece) i;
            flags |= MoveFlags::CAPTURE;
            break;
        }
    }

    if (!(flags & MoveFlags::CAPTURE) && piece_moved == Piece::PAWN) {
        for (auto res : {to_sq - 9, to_sq - 7, to_sq + 7, to_sq + 9}) {
            if (from_sq == res) {
                flags |= (MoveFlags::CAPTURE | MoveFlags::EN_PASSANT);
            }
        }
    }

    return encode_move(from_sq, to_sq, piece_moved, piece_captured, promo_piece, flags);
}
