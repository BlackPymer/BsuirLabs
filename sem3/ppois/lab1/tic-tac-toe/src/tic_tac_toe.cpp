/**
 * @file tic_tac_toe.cpp
 * @brief Реализация класса TicTacToe.
 */
#include "tic_tac_toe.hpp"

TicTacToe::TicTacToe(int fieldSize)
{
    _fieldSize = fieldSize;
    _field = std::vector<std::vector<Cell>>(fieldSize, std::vector<Cell>(fieldSize, Empty));
    _emptyCellsLeft = fieldSize * fieldSize;
}

TicTacToe::TicTacToe(const TicTacToe &other)
{
    _fieldSize = other._fieldSize;
    _field = other._field;
    _emptyCellsLeft = other._emptyCellsLeft;
    _playerTurn = other._playerTurn;
    _gameStatus = other._gameStatus;
}

TicTacToe &TicTacToe::operator=(const TicTacToe &other)
{
    if (this != &other)
    {
        _fieldSize = other._fieldSize;
        _field = other._field;
        _emptyCellsLeft = other._emptyCellsLeft;
        _playerTurn = other._playerTurn;
        _gameStatus = other._gameStatus;
    }
    return *this;
}

void TicTacToe::MakeMove(int row, int col)
{
    if (_gameStatus != IsPlaying)
    {
        throw GameNotStartedException();
    }
    if (row < 0 || row >= _fieldSize || col < 0 || col >= _fieldSize || _field[row][col] != Empty)
        throw InvalidMoveException();
    _field[row][col] = _playerTurn ? X : O;
    _emptyCellsLeft--;
    CheckWin(row, col);
    _playerTurn = !_playerTurn;
}

void TicTacToe::CheckWin(int row, int col)
{
    Cell potentialWinner = _field[row][col];
    bool win = true;
    for (Cell cell : _field[row])
    {
        if (cell != potentialWinner)
        {
            win = false;
            break;
        }
    }
    if (win)
    {
        _gameStatus = potentialWinner == X ? XWins : OWins;
        return;
    }
    win = true;
    for (int i = 0; i < _fieldSize; i++)
    {
        if (_field[i][col] != potentialWinner)
        {
            win = false;
            break;
        }
    }
    if (win)
    {
        _gameStatus = potentialWinner == X ? XWins : OWins;
        return;
    }
    win = true;
    if (row == col)
    {
        for (int i = 0; i < _fieldSize; i++)
        {
            if (_field[i][i] != potentialWinner)
            {
                win = false;
                break;
            }
        }
        if (win)
        {
            _gameStatus = potentialWinner == X ? XWins : OWins;
            return;
        }
    }

    win = true;
    if (row == _fieldSize - 1 - col)
    {
        for (int i = 0; i < _fieldSize; i++)
        {
            if (_field[i][_fieldSize - 1 - i] != potentialWinner)
            {
                win = false;
                break;
            }
        }
        if (win)
        {
            _gameStatus = potentialWinner == X ? XWins : OWins;
            return;
        }
    }
    if (_emptyCellsLeft == 0)
        _gameStatus = Draw;
}

GameStatus TicTacToe::CheckGameStatus()
{
    return _gameStatus;
}

std::string TicTacToe::GetFieldFormatted() const
{
    std::string fieldFormatted = "";
    for (int i = 0; i < _fieldSize + 2; i++)
        fieldFormatted += "-";
    fieldFormatted += "\n";
    for (auto row : _field)
    {
        fieldFormatted += "|";
        for (auto cell : row)
        {
            if (cell == Empty)
                fieldFormatted += " |";
            else if (cell == X)
                fieldFormatted += "X|";
            else
                fieldFormatted += "O|";
        }
        fieldFormatted += "\n";
    }
    return fieldFormatted;
}

bool TicTacToe::isKrestikMove()
{
    return _playerTurn == 1;
}
