#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <cstdint>
#include <bitset>
#include <bit>
#include <limits>
#include <map>

/*
    ------------------------------------
                TEST PUZZLES
    ------------------------------------
    PASSED:
        easy = "..500007.009009350017010560082.400070.030006040.003106.900070006040";
        medium = "050400.004100.002053010608030.200.003.600900005040002190006..700005";
        hard = "002090074300800200400.090040003710.700020053.....005930.006170008";
        extreme = ".013..680+2+6..2+470+54++8.+5-6-030.305026+3.801..4+"; // (new hash system)
*/

using namespace std;
using BitDigit = uint16_t;
using bit9 = bitset<9>;

constexpr BitDigit ALL_DIGITS = 0x1FF;

constexpr BitDigit bit_for_digit(int digit) { return BitDigit(1 << (digit - 1)); }

constexpr int get_pos_ind(int row, int col) { return row * 9 + col; }

constexpr int get_row_ind(int pos) { return pos / 9; }

constexpr int get_col_ind(int pos) { return pos % 9; }

constexpr int get_box_ind(int row, int col) { return (row / 3) * 3 + (col / 3); }

const char newl = '\n';

struct Constraint
{
    int cells[9]; // position of cell 0-80
    BitDigit placed_Z_digits = 0;
    bool solved_constraint = false;

    BitDigit missing_Z_digits() const { return ALL_DIGITS & ~placed_Z_digits; }
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
    void print_Z_digit() const { cout << Z_digit; }

    // places digit in cell, sets candidates to 0
    void place_Z_digit(int digit)
    {
        Z_digit = digit;
        C_digits = 0;
    }

    // returns true if [digit] in candidates
    bool has_C_digit(int digit) const { return C_digits & bit_for_digit(digit); }

    // removes digit from candidates
    void remove_C_digit(int digit) { C_digits &= ~bit_for_digit(digit); }

    // returns number of candidates
    int count_C_digits()
    {
        bit9 candidates = C_digits;
        return candidates.count();
    }
};

struct Solver
{
    int filled = 0;
    Cell board[81];
    Constraint rows[9];
    Constraint cols[9];
    Constraint boxes[9];

    // initialise constraints
    void __solver__()
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
    void process_constraint(Constraint &con) { con.solved_constraint = (con.missing_Z_digits() == 0); }
    /*
    Place Z_digit
    set cell C_digits to 0
    remove digit from C_digits in constraint
    */
    void place_Z_digit(int pos, int digit);
    void remove_C_in_constraint(int pos, int digit);
    // when a domino is found, remove the candidates from all other cells in constraint
    int remove_C_dominos(vector<int> digs, int pos1, int pos2, Constraint &con);
    // fills all naked singles in grid -> number of digits filled
    int naked_single();
    // fills hidden singles in a constraint -> num singles filled
    int hidden_single_in_constraint(Constraint &con);
    // fills hidden singles in every constraint -> number of digits filled
    int hidden_single_fill();
    // find dominos in constraint
    int find_naked_pairs_in_constraint(Constraint &con);
    // pass in unsolved constraints
    int find_all_naked_pairs();
    bool check_solved();
    // TO DO: find where n digits can only appear in n shared cells in a constraint
    int find_hidden_pair_in_constraint(Constraint &con);

    vector<int> remaining_pos_to_fill();
    int recursive_backtrack_solve();
    bool has_contradiction() const;
    int find_best_guess_cell();
};

bool iterative_solve(Solver &s);

/*
    -----------
        AUX
    -----------
*/

void __init__(Solver &s)
{
    s.__solver__();
    for (int i = 0; i < 81; i++)
    {
        s.board[i].__cell__(i);
    }
}
vector<int> get_set_bits(BitDigit bits)
{
    vector<int> result = {};
    while (bits)
    {
        int pos = __builtin_ctz(bits) + 1;
        result.push_back(pos);
        bits &= (bits - 1);
    }
    return result;
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
                cout << '.';
                continue;
            }

            if (dig1 == 0)
            {
                if (dig0 == 0)
                {
                    cout << '+' << dig2;
                    continue;
                }

                if (dig2 == 0)
                {
                    cout << dig0 << '+';
                    continue;
                }
            }
            cout << dig0 << dig1 << dig2;
        }
    }
    cout << newl;
}

void print_board(const Solver &s, char style = 'Z');
void SetupStartingBoard(Solver &s, string BoardHash);
bool SudokuSolver(Solver &s);

/*
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
                MAIN PROGRAM
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
*/

int main()
{
    bool play_again = true;
    bool is_solved;

    do
    {
        is_solved = false;
        Solver s{};
        __init__(s);

        string puzzle_start;
        cout << "Enter puzzle hash: ";
        cin >> puzzle_start;

        SetupStartingBoard(s, puzzle_start);

        cout << "STARTING BOARD\n";
        print_board(s, 'Z');
        cout << newl << newl;

        bool solved = SudokuSolver(s);

        cout << "\nFINAL BOARD\n";
        print_board(s, 'Z');

        if (solved)
        {
            cout << "\nSOLVED!\n";
        }
        else
        {
            cout << "\nNOT SOLVED!\n\nREMAINING CANDIDATES:\n\n";
            print_board(s, 'C');
            // iterative_solve(s);
        }

        cout << "\nType:\n";
        cout << "hash -> get final hash\n";
        cout << "end  -> exit program\n";
        cout << "any other input -> play again\n";
        cout << "> ";

        string decision;
        cin >> decision;

        if (decision == "hash")
        {
            puzzle_hash_out(s);
        }
        if (decision == "end")
        {
            play_again = false;
        }

        is_solved = s.check_solved();

    } while (play_again);
    return is_solved;
}

// MAIN SOLVING ALGORITHM
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

/*
    ------------------------
            Terminal
    ------------------------
*/

// user input board hash into solver -> places digits
void SetupStartingBoard(Solver &s, string BoardHash)
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

// prints board, style = ['Z','C']
void print_board(const Solver &s, char style)
{

    if (style == 'Z')
    {

        for (int i = 0; i < 81; i++)
        {

            i % 3 == 0 ? cout << "  " : cout << ' ';
            if (i % 9 == 0)
            {
                cout << newl;
            }
            if (i % 27 == 0)
            {
                cout << newl;
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
            cout << "pos " << pos << " (" << row << " , " << col << ") : ";

            // get digits from candidate cells to print
            for (int val = 1; val <= 9; val++)
            {
                if (s.board[pos].has_C_digit(val))
                    cout << val << ' ';
            }
            cout << newl;
        }
        cout << "\nFilled Cells: " << s.filled << "\nUnfilled Cells: " << unfilled << newl;
    }
}

/*
    ---------------------------------
            Solver Algorithms
    ---------------------------------
*/

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

    set<int> inds = {};
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
                int digit = bitset<9>(board[pos].C_digits)._Find_first() + 1;
                place_Z_digit(pos, digit);
                cout << "Naked single (" << digit << ") at pos " << pos << newl;
                filled++;
            }
        }
        total_filled += filled;
    } while (filled > 0);
    return total_filled;
}

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
            cout << "Hidden single (" << digit << ") at pos " << last_pos << newl;
            filled++;
        }
    }
    return filled;
}

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
                cout << "Naked pair (" << cdig0 << ',' << cdig1 << ") at pos (" << pos0 << ',' << pos1 << ')' << newl;
                vector<int> digs = get_set_bits(cdig0);
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

int Solver::remove_C_dominos(vector<int> digs, int pos1, int pos2, Constraint &con)
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
    ------------------------------
        RECURSIVE BACKTRACKING
    ------------------------------
*/

int Solver::recursive_backtrack_solve()
{

    vector<int> remaining_pos = remaining_pos_to_fill();
    int remaining_num = remaining_pos.size();
    map<int, int> pos_dig = {};
    for (int i = 0; i < remaining_num; i++)
    {
    }

    return 0;
}

vector<int> Solver::remaining_pos_to_fill()
{
    int n = 0; // number of unoslved cells
    vector<int> pos_to_fill = {};
    for (int pos = 0; pos < 81; pos++)
    {
        if (!board[pos].is_solved())
            pos_to_fill.push_back(pos);
    }
    return pos_to_fill;
}

bool Solver::has_contradiction() const
{
    for (int pos = 0; pos < 81; pos++)
    {
        if (board[pos].is_solved())
            continue;

        if (board[pos].C_digits == 0)
        {
            cout << "has_contradiction!\nPos " << pos << " : " << board[pos].C_digits << newl;
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

bool iterative_solve(Solver &s)
{
    SudokuSolver(s);
    if (s.has_contradiction())
    {
        cout << "s.has_contradiction returned true" << newl;
        return false;
    }
    if (s.check_solved())
    {
        cout << "s.check_solved returned true" << newl;
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

        vector<int> candidates = get_set_bits(s.board[pos].C_digits);
        for (int digit : candidates)
        {
            cout << "testing position " << pos << "\ncandidates : ";
            for (int d : candidates)
                cout << d << ", ";
            cout << newl << "testing digit: " << digit;

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

vector<int> count_unsolved_candidates(Solver &s)
{
    vector<int> res = {0,0,0,0,0,0,0,0,0};
    for (int pos = 0; pos < 81; pos++)
    {
        if (s.board[pos].is_solved())
            continue;

        vector<int> digs = get_set_bits(s.board[pos].C_digits);

        for (int d : digs)
        {
            res.at(d - 1)++;
        } 
    }
    return res;
}