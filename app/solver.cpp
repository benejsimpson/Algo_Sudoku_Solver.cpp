#include "solver.h"
#include "types.h"
#include "utils.h"
#include <vector>

std::vector<int> get_set_bits(BitDigit bits)
{
    std::vector<int> result = {};
    while (bits)
    {
        int pos = __builtin_ctz(bits) + 1;
        result.push_back(pos);
        bits &= (bits - 1);
    }
    return result;
}

void __init__(Solver &s)
{
    s.__solver__();
    for (int i = 0; i < 81; i++)
    {
        s.board[i].__cell__(i);
    }
}

// Main solving algorithm
bool SudokuSolver(Solver &s)
{
    int filled = 0;
    int failed = 0;
    do
    {
        filled = 0;
        filled += s.naked_single();       // fill naked singles
        filled += s.hidden_single_fill(); // fill hidden singles
        s.filled += filled;
        filled += s.find_all_naked_pairs(); // remove candidates seeing dominos
        if (filled == 0)
            failed++;
    } while (failed < 2 && !s.check_solved());
    return s.check_solved();
}

// WIP - iterative solving algorithm
bool iterative_solve(Solver &s)
{
    SudokuSolver(s);
    if (s.has_contradiction())
    {
        std::cout << "s.has_contradiction returned true" << newl;
        return false;
    }
    if (s.check_solved())
    {
        std::cout << "s.check_solved returned true" << newl;
        return true;
    }

    // int pos = s.find_best_guess_cell();
    // if (pos == -1)
    // {
    //     cout << "find_best_guess_cell returned -1" << newl;
    //     return false;
    // }

    for (int pos = 0; pos < 81; pos++)
    {
        if (s.board[pos].is_solved())
            continue;

        std::vector<int> candidates = get_set_bits(s.board[pos].C_digits);
        for (int digit : candidates)
        {
            std::cout << "testing position " << pos << "\ncandidates : ";
            for (int d : candidates)
                std::cout << d << ", ";
            std::cout << newl << "testing digit: " << digit;

            Solver backup = s;
            backup.place_Z_digit(pos, digit);

            if (iterative_solve(backup))
            {
                s = backup;
                return true;
            }
        }
    }
    return false;
}

void SetupStartingBoard(Solver &s, std::string BoardHash)
{
    int pos = 0;
    int digit = 0;
    char prev_c = '#';
    for (char c : BoardHash)
    {
        if (c == '.')
        {
            pos += 3;
            continue;
        }
        if (c == '+')
        {
            pos += 2;
            continue;
        }
        if (c == '-')
        {
            pos++;
            prev_c = '-';
            continue;
        }
        digit = c - '0';
        if (digit != 0)
        {
            s.place_Z_digit(pos, digit);
        }

        if (prev_c == '-')
        {
            pos++;
            prev_c = '#';
        }

        pos++;
    }
}

std::vector<int> count_unsolved_candidates(Solver &s)
{
    std::vector<int> res = {0,0,0,0,0,0,0,0,0};
    for (int pos = 0; pos < 81; pos++)
    {
        if (s.board[pos].is_solved())
            continue;

        std::vector<int> digs = get_set_bits(s.board[pos].C_digits);

        for (int d : digs)
        {
            res.at(d - 1)++;
        } 
    }
    return res;
}


/*
    --------------
        SOLVER
    --------------

    contains functions used in main solving algorithm and brute force solving.
*/

// Initialise
void Solver::__solver__()
{
    for (int r = 0; r < 9; r++)
    {
        for (int c = 0; c < 9; c++)
        {
            int pos = get_pos_ind(r, c);
            int b = get_box_ind(r, c);

            rows[r].cells[c] = pos;
            cols[c].cells[r] = pos;

            int k = (r % 3) * 3 + (c % 3); // position inside box
            boxes[b].cells[k] = pos;

            board[pos].box_index = b;
        }
    }
}

void Solver::process_constraint(Constraint &con)
{
    con.solved_constraint =
        (con.missing_Z_digits() == 0);
}



/*
    --------------------------
        CELL MANIPULATIONS
    --------------------------
*/

// Place Z_digit - set cell C_digits to 0
void Solver::place_Z_digit(int pos, int digit)
{
    if (board[pos].is_solved())
        return;

    int row = board[pos].row_index;
    int col = board[pos].col_index;
    int box = board[pos].box_index;
    int digit_bit = bit_for_digit(digit);

    board[pos].place_Z_digit(digit);
    rows[row].placed_Z_digits |= digit_bit;
    cols[col].placed_Z_digits |= digit_bit;
    boxes[box].placed_Z_digits |= digit_bit;

    std::set<int> inds = {};
    for (int i = 0; i < 9; i++)
    {
        inds.insert(rows[row].cells[i]);
        inds.insert(cols[col].cells[i]);
        inds.insert(boxes[box].cells[i]);
    }

    for (int i : inds)
    {
        board[i].remove_C_digit(digit);
    }
}

void Solver::remove_C_in_constraint(int pos, int digit)
{
    int row = board[pos].row_index;
    int col = board[pos].col_index;
    int box = board[pos].box_index;

    for (int i = 0; i < 9; i++)
    {

        board[rows[row].cells[i]].remove_C_digit(digit);
        board[cols[col].cells[i]].remove_C_digit(digit);
        board[boxes[box].cells[i]].remove_C_digit(digit);
    }
}

// when a domino is found, remove the candidates from all other cells in constraint
int Solver::remove_C_dominos(std::vector<int> digs, int pos1, int pos2, Constraint &con)
{
    int removed = 0;
    for (int i = 0; i < 9; i++)
    {
        int pos = con.cells[i];
        if ((board[pos].is_solved()) || (pos == pos1) || (pos == pos2))
            continue;
        for (int d : digs)
        {
            if (board[pos].has_C_digit(d))
            {
                board[pos].remove_C_digit(d);
                removed++;
            }
        }
    }
    return removed;
}

// returns vector of int positions that dont have Z_digits
std::vector<int> Solver::remaining_pos_to_fill()
{
    int n = 0; // number of unoslved cells
    std::vector<int> pos_to_fill = {};
    for (int pos = 0; pos < 81; pos++)
    {
        if (!board[pos].is_solved())
            pos_to_fill.push_back(pos);
    }
    return pos_to_fill;
}

// returns true if all rows are solved
bool Solver::check_solved()
{
    for (int i = 0; i < 9; i++)
    {
        process_constraint(rows[i]);
        process_constraint(cols[i]);
        process_constraint(boxes[i]);
    }
    int solved = 0;
    for (int i = 0; i < 9; i++)
        solved += rows[i].solved_constraint;
    return solved == 9;
}

/*
    ---------------------
        NAKED SINGLES
    ---------------------
*/

// fills all naked singles in grid -> number of digits filled
int Solver::naked_single()
{
    int filled = 0;
    int total_filled = 0;
    do
    {
        filled = 0;

        for (int pos = 0; pos < 81; pos++)
        {
            if (board[pos].count_C_digits() == 1)
            {
                int digit = std::bitset<9>(board[pos].C_digits)._Find_first() + 1;
                place_Z_digit(pos, digit);
                std::cout << "Naked single (" << digit << ") at pos " << pos << newl;
                filled++;
            }
        }
        total_filled += filled;
    } while (filled > 0);
    return total_filled;
}

/*
    ----------------------
        HIDDEN SINGLES
    ----------------------
*/

// fills hidden singles in a constraint -> num singles filled
int Solver::hidden_single_in_constraint(Constraint &con)
{

    /*
        TO DO:

        does this belong inside Constraint struct?

        build a digit-position BitDigit for the constraint
        - for each digit, create a BitDigit of candidate positions
        - if BitDigit has only one bit set, place digit
    */

    BitDigit missing = con.missing_Z_digits();
    if (con.solved_constraint)
        return 0;
    int filled = 0;

    for (int digit = 1; digit <= 9; digit++)
    {
        if (!(missing & bit_for_digit(digit)))
            continue;
        int count = 0;
        int last_pos = -1;
        for (int i = 0; i < 9; i++)
        {
            int pos = con.cells[i];

            if (board[pos].has_C_digit(digit))
            {
                count++;
                last_pos = pos;
            }
        }
        if (count == 1)
        {
            place_Z_digit(last_pos, digit);
            std::cout << "Hidden single (" << digit << ") at pos " << last_pos << newl;
            filled++;
        }
    }
    return filled;
}

// fills hidden singles in every constraint -> number of digits filled
int Solver::hidden_single_fill()
{
    int filled = 0;
    int total_filled = 0;
    do
    {
        filled = 0;

        for (int i = 0; i < 9; i++)
        {
            process_constraint(rows[i]);
            process_constraint(cols[i]);
            process_constraint(boxes[i]);

            filled += hidden_single_in_constraint(rows[i]);
            filled += hidden_single_in_constraint(cols[i]);
            filled += hidden_single_in_constraint(boxes[i]);
        }
        total_filled += filled;
    } while (filled > 0);
    return total_filled;
}

/*
    -------------------
        NAKED PAIRS
    -------------------
*/

// find dominos in constraint
int Solver::find_naked_pairs_in_constraint(Constraint &con)
{
    if (con.solved_constraint)
        return 0; // if constraint is solved -> continue
    int changes = 0;

    // loop through cells in constraint to get first cell
    for (int i = 0; i < 9; i++)
    {
        int pos0 = con.cells[i]; // position of fist cell in domino test

        // ignore solved cells and cells with >2 candidates
        if (board[pos0].is_solved() || board[pos0].count_C_digits() != 2)
            continue;

        BitDigit cdig0 = board[pos0].C_digits; // bitdig of first cell

        for (int j = i + 1; j < 9; j++)
        {
            int pos1 = con.cells[j]; // position of second cell in domino test

            if ((pos0 == pos1) || (board[pos1].is_solved()))
                continue;

            BitDigit cdig1 = board[pos1].C_digits; // bitdig of second cell

            if (cdig0 == cdig1)
            { // both cells have same vals
                std::cout << "Naked pair (" << cdig0 << ',' << cdig1 << ") at pos (" << pos0 << ',' << pos1 << ')' << newl;
                std::vector<int> digs = get_set_bits(cdig0);
                // remove digits from other cells in shared constraints
                if (board[pos0].row_index == board[pos1].row_index)
                    changes += remove_C_dominos(digs, pos0, pos1, rows[board[pos0].row_index]);
                if (board[pos0].col_index == board[pos1].col_index)
                    changes += remove_C_dominos(digs, pos0, pos1, cols[board[pos0].col_index]);
                if (board[pos0].box_index == board[pos1].box_index)
                    changes += remove_C_dominos(digs, pos0, pos1, boxes[board[pos0].box_index]);
            }
        }
    }
    return changes;
}

// pass in unsolved constraints
int Solver::find_all_naked_pairs()
{
    int filled = 0;
    int total_filled = 0;

    for (int i = 0; i < 9; i++)
    {
        process_constraint(rows[i]);
        process_constraint(cols[i]);
        process_constraint(boxes[i]);

        filled += find_naked_pairs_in_constraint(rows[i]);
        filled += find_naked_pairs_in_constraint(cols[i]);
        filled += find_naked_pairs_in_constraint(boxes[i]);
    }
    total_filled += filled;
    return filled;
}

/*
    --------------------
        HIDDEN PAIRS
    --------------------
*/

int Solver::find_hidden_pair_in_constraint(Constraint &con)
{
    std::map<int, int> digit_pos = {};
    for (int i = 0; i < 9; i++)
    {
        int pos = con.cells[i];
        if (board[pos].is_solved())
            continue;

        std::vector<int> cand;
    }
    return 0;
}

/*
    -------------------------
        BRUTE FORCE SOLVE
    -------------------------
*/

int Solver::recursive_backtrack_solve()
{

    std::vector<int> remaining_pos = remaining_pos_to_fill();
    int remaining_num = remaining_pos.size();
    std::map<int, int> pos_dig = {};
    for (int i = 0; i < remaining_num; i++)
    {
    }

    return 0;
}



bool Solver::has_contradiction() const
{
    for (int pos = 0; pos < 81; pos++)
    {
        if (board[pos].is_solved())
            continue;

        if (board[pos].C_digits == 0)
        {
            std::cout << "has_contradiction!\nPos " << pos << " : " << board[pos].C_digits << newl;
            return true;
        }
    }
    return false;
}

int Solver::find_best_guess_cell()
{
    // might change this to find candidates that appear least number of times in grid
    // need to play about with this

    int best_pos = -1;
    int best_count = 10;

    for (int pos = 0; pos < 81; pos++)
    {
        if (board[pos].is_solved())
            continue;
        if (board[pos].count_C_digits() < best_count)
        {
            best_count = board[pos].count_C_digits();
            best_pos = pos;
        }
        if (best_count == 2)
            break;
    }
    return best_pos;
}
