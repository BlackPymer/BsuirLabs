#include <string>
#include <vector>

enum Cell
{
    Empty,
    X,
    O
};
enum GameStatus
{
    IsPlaying,
    XWins,
    OWins,
    Draw
};

class TicTacToe
{
public:
    TicTacToe(int fieldSize);
    void MakeMove(int row, int col);
    GameStatus CheckGameStatus();
    class InvalidMoveException : public std::exception
    {
    };
    class GameNotStartedException : public std::exception
    {
    };

private:
    std::vector<std::vector<Cell>> _field;
    bool _playerTurn = 1;
    int _fieldSize;
    GameStatus _gameStatus = IsPlaying;
};