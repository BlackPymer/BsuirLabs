/**
 * @file unit_tests.cpp
 * @brief Модульные тесты класса TicTacToe на базе UnitTest++.
 *
 * Тесты покрывают начальное состояние, победы X и O по всем линиям,
 * ничью, некорректные ходы (занятая клетка и выход за границы),
 * ходы после окончания игры, разные размеры поля и копирование.
 */
#include "tic_tac_toe.hpp"
#include <UnitTest++/UnitTest++.h>

TEST(InitialStatusIsPlaying)
{
    TicTacToe game(3);
    CHECK_EQUAL(IsPlaying, game.CheckGameStatus());
}

TEST(StatusAfterOneMoveStillPlaying)
{
    TicTacToe game(3);
    game.MakeMove(0, 0);
    CHECK_EQUAL(IsPlaying, game.CheckGameStatus());
}

TEST(AlmostFullIsStillPlaying)
{
    TicTacToe game(3);
    game.MakeMove(1, 1);
    game.MakeMove(0, 0);
    game.MakeMove(2, 2);
    game.MakeMove(0, 1);
    game.MakeMove(0, 2);
    game.MakeMove(1, 2);
    game.MakeMove(2, 1);
    game.MakeMove(2, 0);
    CHECK_EQUAL(IsPlaying, game.CheckGameStatus());
}

TEST(OneByOneXWins)
{
    TicTacToe game(1);
    game.MakeMove(0, 0);
    CHECK_EQUAL(XWins, game.CheckGameStatus());
}

TEST(TwoByTwoXWins)
{
    TicTacToe game(2);
    game.MakeMove(0, 0);
    game.MakeMove(1, 0);
    game.MakeMove(0, 1);
    CHECK_EQUAL(XWins, game.CheckGameStatus());
}

TEST(XWinsTopRow)
{
    TicTacToe game(3);
    game.MakeMove(0, 0);
    game.MakeMove(1, 0);
    game.MakeMove(0, 1);
    game.MakeMove(1, 1);
    game.MakeMove(0, 2);
    CHECK_EQUAL(XWins, game.CheckGameStatus());
}

TEST(XWinsMiddleRow)
{
    TicTacToe game(3);
    game.MakeMove(1, 0);
    game.MakeMove(0, 0);
    game.MakeMove(1, 1);
    game.MakeMove(2, 2);
    game.MakeMove(1, 2);
    CHECK_EQUAL(XWins, game.CheckGameStatus());
}

TEST(XWinsBottomRow)
{
    TicTacToe game(3);
    game.MakeMove(2, 0);
    game.MakeMove(0, 0);
    game.MakeMove(2, 1);
    game.MakeMove(0, 1);
    game.MakeMove(2, 2);
    CHECK_EQUAL(XWins, game.CheckGameStatus());
}

TEST(XWinsFirstColumn)
{
    TicTacToe game(3);
    game.MakeMove(0, 0);
    game.MakeMove(0, 1);
    game.MakeMove(1, 0);
    game.MakeMove(0, 2);
    game.MakeMove(2, 0);
    CHECK_EQUAL(XWins, game.CheckGameStatus());
}

TEST(XWinsMiddleColumn)
{
    TicTacToe game(3);
    game.MakeMove(0, 1);
    game.MakeMove(0, 0);
    game.MakeMove(1, 1);
    game.MakeMove(2, 0);
    game.MakeMove(2, 1);
    CHECK_EQUAL(XWins, game.CheckGameStatus());
}

TEST(XWinsLastColumn)
{
    TicTacToe game(3);
    game.MakeMove(0, 2);
    game.MakeMove(0, 0);
    game.MakeMove(1, 2);
    game.MakeMove(2, 0);
    game.MakeMove(2, 2);
    CHECK_EQUAL(XWins, game.CheckGameStatus());
}

TEST(XWinsMainDiagonal)
{
    TicTacToe game(3);
    game.MakeMove(0, 0);
    game.MakeMove(0, 1);
    game.MakeMove(1, 1);
    game.MakeMove(1, 0);
    game.MakeMove(2, 2);
    CHECK_EQUAL(XWins, game.CheckGameStatus());
}

TEST(XWinsAntiDiagonal)
{
    TicTacToe game(3);
    game.MakeMove(0, 2);
    game.MakeMove(0, 0);
    game.MakeMove(1, 1);
    game.MakeMove(1, 0);
    game.MakeMove(2, 0);
    CHECK_EQUAL(XWins, game.CheckGameStatus());
}

TEST(OWinsTopRow)
{
    TicTacToe game(3);
    game.MakeMove(1, 1);
    game.MakeMove(0, 0);
    game.MakeMove(2, 2);
    game.MakeMove(0, 1);
    game.MakeMove(1, 2);
    game.MakeMove(0, 2);
    CHECK_EQUAL(OWins, game.CheckGameStatus());
}

TEST(OWinsMiddleColumn)
{
    TicTacToe game(3);
    game.MakeMove(0, 0);
    game.MakeMove(0, 1);
    game.MakeMove(2, 2);
    game.MakeMove(1, 1);
    game.MakeMove(2, 0);
    game.MakeMove(2, 1);
    CHECK_EQUAL(OWins, game.CheckGameStatus());
}

TEST(OWinsBottomRow)
{
    TicTacToe game(3);
    game.MakeMove(0, 1);
    game.MakeMove(2, 0);
    game.MakeMove(1, 1);
    game.MakeMove(2, 1);
    game.MakeMove(0, 0);
    game.MakeMove(2, 2);
    CHECK_EQUAL(OWins, game.CheckGameStatus());
}

TEST(OWinsMainDiagonal)
{
    TicTacToe game(3);
    game.MakeMove(0, 1);
    game.MakeMove(0, 0);
    game.MakeMove(0, 2);
    game.MakeMove(1, 1);
    game.MakeMove(2, 1);
    game.MakeMove(2, 2);
    CHECK_EQUAL(OWins, game.CheckGameStatus());
}

TEST(OWinsAntiDiagonal)
{
    TicTacToe game(3);
    game.MakeMove(0, 0);
    game.MakeMove(0, 2);
    game.MakeMove(1, 0);
    game.MakeMove(1, 1);
    game.MakeMove(2, 1);
    game.MakeMove(2, 0);
    CHECK_EQUAL(OWins, game.CheckGameStatus());
}

TEST(FourByFourXWinsDiagonal)
{
    TicTacToe game(4);
    game.MakeMove(0, 0);
    game.MakeMove(0, 1);
    game.MakeMove(1, 1);
    game.MakeMove(0, 2);
    game.MakeMove(2, 2);
    game.MakeMove(0, 3);
    game.MakeMove(3, 3);
    CHECK_EQUAL(XWins, game.CheckGameStatus());
}

TEST(FiveByFiveOWinsAntiDiagonal)
{
    TicTacToe game(5);
    game.MakeMove(0, 0);
    game.MakeMove(0, 4);
    game.MakeMove(0, 1);
    game.MakeMove(1, 3);
    game.MakeMove(0, 2);
    game.MakeMove(2, 2);
    game.MakeMove(1, 0);
    game.MakeMove(3, 1);
    game.MakeMove(1, 1);
    game.MakeMove(4, 0);
    CHECK_EQUAL(OWins, game.CheckGameStatus());
}

TEST(DrawOnFullBoard)
{
    TicTacToe game(3);
    game.MakeMove(1, 1);
    game.MakeMove(0, 0);
    game.MakeMove(2, 2);
    game.MakeMove(0, 1);
    game.MakeMove(0, 2);
    game.MakeMove(1, 2);
    game.MakeMove(2, 1);
    game.MakeMove(2, 0);
    game.MakeMove(1, 0);
    CHECK_EQUAL(Draw, game.CheckGameStatus());
}

TEST(InvalidMoveOccupiedCell)
{
    TicTacToe game(3);
    game.MakeMove(0, 0);
    CHECK_THROW(game.MakeMove(0, 0), TicTacToe::InvalidMoveException);
}

TEST(InvalidMoveSecondOccupiedCell)
{
    TicTacToe game(3);
    game.MakeMove(0, 0);
    game.MakeMove(1, 1);
    CHECK_THROW(game.MakeMove(1, 1), TicTacToe::InvalidMoveException);
}

TEST(InvalidMoveNegativeRow)
{
    TicTacToe game(3);
    CHECK_THROW(game.MakeMove(-1, 0), TicTacToe::InvalidMoveException);
}

TEST(InvalidMoveNegativeColumn)
{
    TicTacToe game(3);
    CHECK_THROW(game.MakeMove(0, -1), TicTacToe::InvalidMoveException);
}

TEST(InvalidMoveRowOutOfBounds)
{
    TicTacToe game(3);
    CHECK_THROW(game.MakeMove(3, 0), TicTacToe::InvalidMoveException);
}

TEST(InvalidMoveColumnOutOfBounds)
{
    TicTacToe game(3);
    CHECK_THROW(game.MakeMove(0, 3), TicTacToe::InvalidMoveException);
}

TEST(InvalidMoveBothOutOfBounds)
{
    TicTacToe game(3);
    CHECK_THROW(game.MakeMove(-1, 3), TicTacToe::InvalidMoveException);
}

TEST(InvalidMoveOutOfBoundsOnTwoByTwo)
{
    TicTacToe game(2);
    CHECK_THROW(game.MakeMove(2, 0), TicTacToe::InvalidMoveException);
}

TEST(MoveAfterXWinThrows)
{
    TicTacToe game(3);
    game.MakeMove(0, 0);
    game.MakeMove(1, 0);
    game.MakeMove(0, 1);
    game.MakeMove(1, 1);
    game.MakeMove(0, 2);
    CHECK_THROW(game.MakeMove(2, 0), TicTacToe::GameNotStartedException);
}

TEST(MoveAfterOWinThrows)
{
    TicTacToe game(3);
    game.MakeMove(1, 1);
    game.MakeMove(0, 0);
    game.MakeMove(2, 2);
    game.MakeMove(0, 1);
    game.MakeMove(1, 2);
    game.MakeMove(0, 2);
    CHECK_THROW(game.MakeMove(1, 0), TicTacToe::GameNotStartedException);
}

TEST(MoveAfterDrawThrows)
{
    TicTacToe game(3);
    game.MakeMove(1, 1);
    game.MakeMove(0, 0);
    game.MakeMove(2, 2);
    game.MakeMove(0, 1);
    game.MakeMove(0, 2);
    game.MakeMove(1, 2);
    game.MakeMove(2, 1);
    game.MakeMove(2, 0);
    game.MakeMove(1, 0);
    CHECK_THROW(game.MakeMove(0, 0), TicTacToe::GameNotStartedException);
}

TEST(StatusRemainsAfterThrownMove)
{
    TicTacToe game(3);
    game.MakeMove(0, 0);
    game.MakeMove(1, 0);
    game.MakeMove(0, 1);
    game.MakeMove(1, 1);
    game.MakeMove(0, 2);
    try
    {
        game.MakeMove(2, 2);
    }
    catch (TicTacToe::GameNotStartedException &e)
    {
    }
    CHECK_EQUAL(XWins, game.CheckGameStatus());
}

TEST(CopyConstructorCopiesInitialState)
{
    TicTacToe original(3);
    TicTacToe copy(original);
    CHECK_EQUAL(IsPlaying, copy.CheckGameStatus());
}

TEST(CopyConstructorCopiesMidGameState)
{
    TicTacToe original(3);
    original.MakeMove(0, 0);
    original.MakeMove(1, 0);
    original.MakeMove(0, 1);
    TicTacToe copy(original);
    CHECK_EQUAL(IsPlaying, copy.CheckGameStatus());
    original.MakeMove(1, 1);
    original.MakeMove(0, 2);
    CHECK_EQUAL(XWins, original.CheckGameStatus());
    CHECK_EQUAL(IsPlaying, copy.CheckGameStatus());
}

TEST(CopyConstructorCopiesFinishedState)
{
    TicTacToe original(3);
    original.MakeMove(1, 1);
    original.MakeMove(0, 0);
    original.MakeMove(2, 2);
    original.MakeMove(0, 1);
    original.MakeMove(0, 2);
    original.MakeMove(1, 2);
    original.MakeMove(2, 1);
    original.MakeMove(2, 0);
    original.MakeMove(1, 0);
    TicTacToe copy(original);
    CHECK_EQUAL(Draw, copy.CheckGameStatus());
}

TEST(AssignmentCopiesState)
{
    TicTacToe source(3);
    source.MakeMove(0, 0);
    source.MakeMove(1, 0);
    source.MakeMove(0, 1);
    TicTacToe target(3);
    target = source;
    CHECK_EQUAL(IsPlaying, target.CheckGameStatus());
    source.MakeMove(1, 1);
    source.MakeMove(0, 2);
    CHECK_EQUAL(XWins, source.CheckGameStatus());
    CHECK_EQUAL(IsPlaying, target.CheckGameStatus());
}

TEST(AssignmentSelfAssignment)
{
    TicTacToe game(3);
    game.MakeMove(0, 0);
    game = game;
    CHECK_EQUAL(IsPlaying, game.CheckGameStatus());
}

TEST(CopyIsIndependent)
{
    TicTacToe original(3);
    original.MakeMove(0, 0);
    original.MakeMove(1, 0);
    original.MakeMove(0, 1);
    TicTacToe copy(original);
    copy.MakeMove(1, 1);
    copy.MakeMove(0, 2);
    CHECK_EQUAL(XWins, copy.CheckGameStatus());
    CHECK_EQUAL(IsPlaying, original.CheckGameStatus());
}

int main()
{
    return UnitTest::RunAllTests();
}