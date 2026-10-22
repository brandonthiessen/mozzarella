#pragma once

#include <cstdint>
#include <string>

#include "position.h"

uint32_t parse_move(Position pos, std::string move_str);
