#pragma once

#include <cstdint>
#include <string>

#include "position.h"

void uci_loop();
uint32_t parse_move(mozzarella::Position pos, std::string move_str);
