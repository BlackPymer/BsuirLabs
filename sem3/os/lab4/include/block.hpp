#pragma once
#include <string>

struct Block
{
    bool isFree = true;
    std::string data;
    Block(int block_size)
    {
        data = std::string(block_size, ' ');
    }
};