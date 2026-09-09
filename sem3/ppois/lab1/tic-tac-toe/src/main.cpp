#include <iostream>
#include "tic_tac_toe.hpp"

int main()
{
    std::cout << "TIC TAC TOE GAME\n";
    while (true)
    {

        std::cout << "Enter filed size (3+):\n-> ";
        int fieldSize = 3;
        std::cin >> fieldSize;
        if (fieldSize < 3)
        {
            std::cout << "Make sure field size is greater than 3 ";
            continue;
        }
        TicTacToe game(fieldSize);

        while (game.CheckGameStatus() == IsPlaying)
        {
            std::cout << "Player " << (game.isKrestikMove() ? "krestik" : "zero") << " turn. Write 2 numbers from 0 to 2:\n-> ";
            int row, col;
            std::cout << "Enter row and column:\n-> ";
            std::cin >> row >> col;
            try
            {
                game.MakeMove(row, col);
            }
            catch (TicTacToe::InvalidMoveException &e)
            {
                std::cout << "Illegal move! Try again.\n";
                continue;
            }
        }
        std::cout << "Game over.\n";
        if (game.CheckGameStatus() == Draw)
        {
            std::cout << "Draw!\n\n\n\n\n";
        }
        else
            std::cout << "Player " << (game.CheckGameStatus() == XWins ? " krestik " : " zero ") << " wins !\n\n\n\n\n";
    }
}