#include "io.h"

void print_board(Solver &s, char style)
{

    if (style == 'Z')
    {

        for (int i = 0; i < 81; i++)
        {

            i % 3 == 0 ? std::cout << "  " : std::cout << ' ';
            if (i % 9 == 0)
            {
                std::cout << newl;
            }
            if (i % 27 == 0)
            {
                std::cout << newl;
            }
            s.board[i].print_Z_digit();
        }
    }

    if (style == 'C')
    {
        int unfilled = 0;
        for (int pos = 0; pos < 81; pos++)
        {
            if (s.board[pos].is_solved())
                continue;

            unfilled++;
            int row = s.board[pos].row_index + 1;
            int col = s.board[pos].col_index + 1;
            std::cout << "pos " << pos << " (" << row << " , " << col << ") : ";

            // get digits from candidate cells to print
            for (int val = 1; val <= 9; val++)
            {
                if (s.board[pos].has_C_digit(val))
                    std::cout << val << ' ';
            }
            std::cout << newl;
        }
        std::cout << "\nFilled Cells: " << s.filled << "\nUnfilled Cells: " << unfilled << newl;
    }
}

void puzzle_hash_out(Solver s)
{
    for (int row = 0; row < 9; row++)
    {
        for (int boxrow = 0; boxrow < 3; boxrow++)
        {
            int pos0 = (9 * row) + (3 * boxrow);
            int pos1 = pos0 + 1;
            int pos2 = pos0 + 2;
            int dig0 = s.board[pos0].Z_digit;
            int dig1 = s.board[pos1].Z_digit;
            int dig2 = s.board[pos2].Z_digit;

            if (dig0 + dig1 + dig2 == 0)
            {
                std::cout << '.';
                continue;
            }

            if (dig1 == 0)
            {
                if (dig0 == 0)
                {
                    std::cout << '+' << dig2;
                    continue;
                }

                if (dig2 == 0)
                {
                    std::cout << dig0 << '+';
                    continue;
                }
            }
            std::cout << dig0 << dig1 << dig2;
        }
    }
    std::cout << newl;
}