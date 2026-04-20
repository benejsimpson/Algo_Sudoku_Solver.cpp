#pragma once

#include "types.h"

struct Solver
{
    int filled = 0;
    Cell board[81];
    Constraint rows[9];
    Constraint cols[9];
    Constraint boxes[9];

    // initialise constraints
    void __solver__();

    void process_constraint(Constraint &con);
    void place_Z_digit(int pos, int digit);
    void remove_C_in_constraint(int pos, int digit);
    // when a domino is found, remove the candidates from all other cells in constraint
    int remove_C_dominos(std::vector<int> digs, int pos1, int pos2, Constraint &con);
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
    std::vector<int> remaining_pos_to_fill();
    int recursive_backtrack_solve();
    bool has_contradiction() const;
    int find_best_guess_cell();
};

bool SudokuSolver(Solver &s);
bool iterative_solve(Solver &s);
void __init__(Solver &s);
std::vector<int> get_set_bits(BitDigit bits);
void SetupStartingBoard(Solver &s, std::string BoardHash);