#include "utils.h"
#include "types.h"
#include "solver.h"
#include "io.h"

/*
    ------------------------------------
                TEST PUZZLES
    ------------------------------------
    PASSED:
        easy = "..5++7.+9+9350017010560082.4+070.030+6040.+3106.9+070+6040";
        medium = "0504+.+41+.+2053010608030.2+.+3.6+9++5040+2190+6..7++5";
        hard = "+20900743+8+2+4+.090040+3710.7+020053.....+5930.+6170+8";
        extreme = ".013..680+2+6..2+470+54++8.+5-6-030.305026+3.801..4+";
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

        std::string puzzle_start;
        std::cout << "Enter puzzle hash: ";
        std::cin >> puzzle_start;

        SetupStartingBoard(s, puzzle_start);

        std::cout << "STARTING BOARD\n";
        print_board(s, 'Z');
        std::cout << newl << newl;

        bool solved = SudokuSolver(s);

        std::cout << "\nFINAL BOARD\n";
        print_board(s, 'Z');

        if (solved)
        {
            std::cout << "\nSOLVED!\n";
        }
        else
        {
            std::cout << "\nNOT SOLVED!\n\nREMAINING CANDIDATES:\n\n";
            print_board(s, 'C');
            // iterative_solve(s);
        }

        std::cout << "\nType:\n";
        std::cout << "hash -> get final hash\n";
        std::cout << "end  -> exit program\n";
        std::cout << "any other input -> play again\n";
        std::cout << "> ";

        std::string decision;
        std::cin >> decision;

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