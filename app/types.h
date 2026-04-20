#pragma once
#include <iostream>
#include <set>
#include <map>
#include "utils.h"


struct Constraint
{
    int cells[9]; // points to Cell:: [position of cell 0-80]
    BitDigit placed_Z_digits = 0;
    bool solved_constraint = false;

    BitDigit missing_Z_digits() const
    {
        return ALL_DIGITS & ~placed_Z_digits;
    }
};

struct Cell
{
    int Z_digit = 0;                // (int) placed digit -> default 0 (not placed)
    BitDigit C_digits = ALL_DIGITS; // (BitDigit) cell candidates
    int row_index, col_index, box_index, pos;

    void __cell__(int cell_pos)
    {
        pos = cell_pos;
        row_index = get_row_ind(pos);
        col_index = get_col_ind(pos);
        box_index = get_box_ind(row_index, col_index);
    }

    // returns true if digit placed in cell
    bool is_solved() const { return Z_digit != 0; }

    // prints the digit in the centre of cell
    // of not placed -> 0
    void print_Z_digit() const { std::cout << Z_digit; }

    // places digit in cell, sets candidates to 0
    void place_Z_digit(int digit)
    {
        Z_digit = digit;
        C_digits = 0;
    }

    // returns true if [digit] in candidates
    bool has_C_digit(int digit)
    {
        return C_digits & bit_for_digit(digit);
    }

    // removes digit from candidates
    void remove_C_digit(int digit)
    {
        C_digits &= ~bit_for_digit(digit);
    }

    // returns number of candidates
    int count_C_digits()
    {
        bit9 candidates = C_digits;
        return candidates.count();
    }
};