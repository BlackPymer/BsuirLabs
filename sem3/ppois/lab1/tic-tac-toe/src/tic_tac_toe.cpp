#include "include/tic_tac_toe.hpp"

TicTacToe::TicTacToe(int fieldSize)
{
    _fieldSize = fieldSize;
    _field = std::vector<std::vector<Cell>>(fieldSize, std::vector<Cell>(fieldSize, Empty));
}
void TicTacToe::MakeMove(int row, int col)
{
    if (_gameStatus != IsPlaying)
    {
        throw GameNotStartedException();
    }
    if (_field[row][col] != Empty || row < 0 || row >= _fieldSize || col < 0 || col >= _fieldSize)
        throw InvalidMoveException();
    _field[row][col] = _playerTurn ? X : O;
    _playerTurn = !_playerTurn;
}

GameStatus TicTacToe::CheckGameStatus()
{
}