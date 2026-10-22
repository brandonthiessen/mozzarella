#pragma once

#include <fstream>

extern std::ofstream debug_log;

#define LOG(x) do { debug_log << x << std::endl; } while(0)
