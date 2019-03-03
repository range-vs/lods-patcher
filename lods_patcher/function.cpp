#include "function.h"

std::string read_binary_text(std::ifstream & file)
{
	std::string out;
	do
	{
		char current(' ');
		file.read(&current, sizeof(char));
		if (!file)
			break;
		if (current == 0x00)
			break;
		out += current;
	} while (true);
	if (!out.length())
		return "null";
	return out;
}

bool read_block_and_size(std::ifstream& file, uint & block, uint & size)
{
	file.read((char*)&block, sizeof(block)); //ID блока(сразу  с размером)
	if (!file)
		return false;
	file.read((char*)&size, sizeof(size)); //размер блока
	if (!file)
		return false;
	return true;
}

