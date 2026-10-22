#pragma once

#include "position.h"
#include "move_generation.h"

#include <cstdint>
#include <iostream>
#include <cassert>
#include <string>

uint64_t perft(Position *p, int depth);
