#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "piece.h"
#include "player.h"

namespace mozzarella {

const std::unordered_map<char, Piece> _chtopc = {
    {'p', Piece::PAWN},
    {'n', Piece::KNIGHT},
    {'b', Piece::BISHOP},
    {'r', Piece::ROOK},
    {'q', Piece::QUEEN},
    {'k', Piece::KING},
};

// Translate a standard coordinate string into a square number (0 - 63).
int coord_to_sq(std::string str);

// Translate a square number to a standard coordinate string.
std::string square_to_coord(int sq);

// Return the string form of an encoded move.
std::string move_to_string(uint32_t move);

// Return the player corresponding to the character from the FEN
Player chtopl(char c);

// Return the piece corresponding to the character from the FEN
Piece chtopc(char c);

// Splits a string based on delimiter and returns the pieces as a vector of strings (GPT CODE)
std::vector<std::string> split_str(const std:: string& s, char delimiter);

// Output the chess board to the console.
void render_board(void *v);

}
