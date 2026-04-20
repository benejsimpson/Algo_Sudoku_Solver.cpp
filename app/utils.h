#pragma once
#include <cstdint>
#include <vector>
#include <bitset>
#include <bit>

using BitDigit = uint16_t;
using bit9 = std::bitset<9>;

constexpr char newl = '\n';

constexpr int get_pos_ind(int row, int col)
{
    return row * 9 + col;
}

constexpr int get_row_ind(int pos)
{
    return pos / 9;
}

constexpr int get_col_ind(int pos)
{
    return pos % 9;
}

constexpr int get_box_ind(int row, int col)
{
    return (row / 3) * 3 + (col / 3);
}


constexpr BitDigit ALL_DIGITS = 0x1FF;

constexpr BitDigit bit_for_digit(int digit)
{
    return BitDigit(1 << (digit - 1));
}