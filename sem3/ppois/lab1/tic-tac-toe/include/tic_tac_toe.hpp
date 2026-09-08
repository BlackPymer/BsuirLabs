#include <string>
#include <vector>

enum Cell
{
    Empty,
    X,
    O
};

class TicTacToe
{
public:
    TicTacToe(int fieldSize)
    {
        _fieldSize = fieldSize;
        _field = std::vector<std::vector<Cell>>(fieldSize, std::vector<Cell>(fieldSize, Empty));
    }

private:
    std::vector<std::vector<Cell>> _field;
    bool _playerTurn = 1;
    int _fieldSize;
};