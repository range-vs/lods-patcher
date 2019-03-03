#pragma once

#include <string>
#include <fstream>

#include "types.h"

std::string read_binary_text(std::ifstream& file);
bool read_block_and_size(std::ifstream& file, uint& block, uint& size);