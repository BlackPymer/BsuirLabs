/**
 * @file unit_tests.cpp
 * @brief Модульные тесты словаря Vocabulary на базе UnitTest++.
 *
 * Тесты покрывают конструкторы (по умолчанию и из файла), добавление пар,
 * поиск переводов, удаление слов (лист, один ребёнок, два ребёнка),
 * очистку словаря, оператор присваивания и работу деструктора.
 */
#include "vocabulary.hpp"
#include <UnitTest++/UnitTest++.h>
#include <fstream>
#include <cstdio>

static const std::string APPLE = "apple";
static const std::string JABLOKO = "jabloko";
static const std::string BANANA = "banana";
static const std::string CHERRY = "cherry";

TEST(DefaultConstructorIsEmpty)
{
    Vocabulary voc;
    CHECK_EQUAL(0, voc.GetWordsCount());
}

TEST(FileConstructorThrowsOnMissingFile)
{
    CHECK_THROW(Vocabulary voc("nonexistent_file_xyz.txt"), Vocabulary::FileOpeningFailedException);
}

TEST(FileConstructorLoadsWords)
{
    const char *name = "vocab_test_file.txt";
    {
        std::ofstream out(name);
        out << "cat:kot\n"
            << "dog:sobaka\n"
            << "bird:ptica\n";
    }
    Vocabulary voc(name);
    CHECK_EQUAL(3, voc.GetWordsCount());
    CHECK_EQUAL(":kot", voc["cat"]);
    CHECK_EQUAL(":sobaka", voc["dog"]);
    CHECK_EQUAL(":ptica", voc["bird"]);
    std::remove(name);
}

TEST(FileConstructorEmptyFile)
{
    const char *name = "vocab_empty_file.txt";
    {
        std::ofstream out(name);
    }
    Vocabulary voc(name);
    CHECK_EQUAL(0, voc.GetWordsCount());
    std::remove(name);
}

TEST(AddSingleWordCount)
{
    Vocabulary voc;
    voc += std::make_pair(APPLE, JABLOKO);
    CHECK_EQUAL(1, voc.GetWordsCount());
}

TEST(AddThreeWordsCount)
{
    Vocabulary voc;
    voc += std::make_pair(BANANA, "banan");
    voc += std::make_pair(APPLE, JABLOKO);
    voc += std::make_pair(CHERRY, "vishnya");
    CHECK_EQUAL(3, voc.GetWordsCount());
}

TEST(AddTenWordsCount)
{
    Vocabulary voc;
    const char *words[] = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j"};
    for (int i = 0; i < 10; i++)
        voc += std::make_pair(words[i], std::string("t") + words[i]);
    CHECK_EQUAL(10, voc.GetWordsCount());
}

TEST(AddDuplicateWordKeepsBoth)
{
    Vocabulary voc;
    voc += std::make_pair(APPLE, JABLOKO);
    voc += std::make_pair(APPLE, "yabloko");
    CHECK_EQUAL(2, voc.GetWordsCount());
}

TEST(LookupExistingWord)
{
    Vocabulary voc;
    voc += std::make_pair(BANANA, "banan");
    CHECK_EQUAL("banan", voc[BANANA]);
}

TEST(LookupReturnsTranslation)
{
    Vocabulary voc;
    voc += std::make_pair(APPLE, JABLOKO);
    voc += std::make_pair(BANANA, "banan");
    voc += std::make_pair(CHERRY, "vishnya");
    CHECK_EQUAL(JABLOKO, voc[APPLE]);
    CHECK_EQUAL("banan", voc[BANANA]);
    CHECK_EQUAL("vishnya", voc[CHERRY]);
}

TEST(LookupMissingWordThrows)
{
    Vocabulary voc;
    voc += std::make_pair(APPLE, JABLOKO);
    CHECK_THROW(voc["pear"], Vocabulary::WordNotFoundException);
}

TEST(LookupOnEmptyThrows)
{
    Vocabulary voc;
    CHECK_THROW(voc[APPLE], Vocabulary::WordNotFoundException);
}

TEST(LookupWordInLeftBranch)
{
    Vocabulary voc;
    voc += std::make_pair(BANANA, "banan");
    voc += std::make_pair(CHERRY, "vishnya");
    CHECK_EQUAL("vishnya", voc[CHERRY]);
}

TEST(LookupWordInRightBranch)
{
    Vocabulary voc;
    voc += std::make_pair(BANANA, "banan");
    voc += std::make_pair(APPLE, JABLOKO);
    CHECK_EQUAL(JABLOKO, voc[APPLE]);
}

TEST(LookupDeepChain)
{
    Vocabulary voc;
    const char *words[] = {"m", "f", "a", "j", "z", "t", "w"};
    for (int i = 0; i < 7; i++)
        voc += std::make_pair(words[i], std::string("tr_") + words[i]);
    for (int i = 0; i < 7; i++)
        CHECK_EQUAL(std::string("tr_") + words[i], voc[words[i]]);
}

TEST(LookupManyWordsAll)
{
    Vocabulary voc;
    const char *words[] = {"a", "b", "c", "d", "e", "f", "g", "h"};
    for (int i = 0; i < 8; i++)
        voc += std::make_pair(words[i], std::string("tr_") + words[i]);
    CHECK_EQUAL(8, voc.GetWordsCount());
    for (int i = 0; i < 8; i++)
        CHECK_EQUAL(std::string("tr_") + words[i], voc[words[i]]);
}

TEST(BracketReferenceIsMutable)
{
    Vocabulary voc;
    voc += std::make_pair("dog", "sobaka");
    voc["dog"] = "pes";
    CHECK_EQUAL("pes", voc["dog"]);
}

TEST(RemoveFromEmptyDoesNothing)
{
    Vocabulary voc;
    voc -= "apple";
    CHECK_EQUAL(0, voc.GetWordsCount());
}

TEST(RemoveOnlyElement)
{
    Vocabulary voc;
    voc += std::make_pair(APPLE, JABLOKO);
    voc -= APPLE;
    CHECK_EQUAL(0, voc.GetWordsCount());
    CHECK_THROW(voc[APPLE], Vocabulary::WordNotFoundException);
}

TEST(RemoveMissingWordDoesNothing)
{
    Vocabulary voc;
    voc += std::make_pair(APPLE, JABLOKO);
    voc -= "pear";
    CHECK_EQUAL(1, voc.GetWordsCount());
    CHECK_EQUAL(JABLOKO, voc[APPLE]);
}

TEST(RemoveLeafWord)
{
    Vocabulary voc;
    voc += std::make_pair(BANANA, "banan");
    voc += std::make_pair(APPLE, JABLOKO);
    voc += std::make_pair(CHERRY, "vishnya");
    voc -= CHERRY;
    CHECK_EQUAL(2, voc.GetWordsCount());
    CHECK_THROW(voc[CHERRY], Vocabulary::WordNotFoundException);
    CHECK_EQUAL(JABLOKO, voc[APPLE]);
    CHECK_EQUAL("banan", voc[BANANA]);
}

TEST(RemoveNodeWithOnlyLeftChild)
{
    Vocabulary voc;
    voc += std::make_pair(BANANA, "banan");
    voc += std::make_pair(CHERRY, "vishnya");
    voc += std::make_pair("date", "finik");
    voc -= CHERRY;
    CHECK_EQUAL(2, voc.GetWordsCount());
    CHECK_THROW(voc[CHERRY], Vocabulary::WordNotFoundException);
    CHECK_EQUAL("finik", voc["date"]);
    CHECK_EQUAL("banan", voc[BANANA]);
}

TEST(RemoveNodeWithOnlyRightChild)
{
    Vocabulary voc;
    voc += std::make_pair(CHERRY, "vishnya");
    voc += std::make_pair(BANANA, "banan");
    voc += std::make_pair(APPLE, JABLOKO);
    voc -= BANANA;
    CHECK_EQUAL(2, voc.GetWordsCount());
    CHECK_THROW(voc[BANANA], Vocabulary::WordNotFoundException);
    CHECK_EQUAL(JABLOKO, voc[APPLE]);
    CHECK_EQUAL("vishnya", voc[CHERRY]);
}

TEST(RemoveRootWithTwoChildren)
{
    Vocabulary voc;
    voc += std::make_pair(BANANA, "banan");
    voc += std::make_pair(CHERRY, "vishnya");
    voc += std::make_pair(APPLE, JABLOKO);
    voc -= BANANA;
    CHECK_EQUAL(2, voc.GetWordsCount());
    CHECK_THROW(voc[BANANA], Vocabulary::WordNotFoundException);
    CHECK_EQUAL(JABLOKO, voc[APPLE]);
    CHECK_EQUAL("vishnya", voc[CHERRY]);
}

TEST(RemoveMiddleNodeWithTwoChildren)
{
    Vocabulary voc;
    voc += std::make_pair("zebra", "sebra");
    voc += std::make_pair("melon", "dynya");
    voc += std::make_pair("orange", "apelsin");
    voc += std::make_pair(BANANA, "banan");
    voc -= "melon";
    CHECK_EQUAL(3, voc.GetWordsCount());
    CHECK_THROW(voc["melon"], Vocabulary::WordNotFoundException);
    CHECK_EQUAL("apelsin", voc["orange"]);
    CHECK_EQUAL("banan", voc[BANANA]);
    CHECK_EQUAL("sebra", voc["zebra"]);
}

TEST(RemoveCountDecrements)
{
    Vocabulary voc;
    voc += std::make_pair(APPLE, JABLOKO);
    voc += std::make_pair(BANANA, "banan");
    voc += std::make_pair(CHERRY, "vishnya");
    voc += std::make_pair("date", "finik");
    voc -= APPLE;
    voc -= CHERRY;
    CHECK_EQUAL(2, voc.GetWordsCount());
}

TEST(RemoveThenLookupThrows)
{
    Vocabulary voc;
    voc += std::make_pair("dog", "sobaka");
    voc -= "dog";
    CHECK_THROW(voc["dog"], Vocabulary::WordNotFoundException);
}

TEST(RemoveThenAddAgain)
{
    Vocabulary voc;
    voc += std::make_pair(BANANA, "banan");
    voc -= BANANA;
    voc += std::make_pair(BANANA, "banan2");
    CHECK_EQUAL(1, voc.GetWordsCount());
    CHECK_EQUAL("banan2", voc[BANANA]);
}

TEST(RemoveAllOneByOne)
{
    Vocabulary voc;
    const char *words[] = {"c", "a", "e", "b", "d", "f"};
    for (int i = 0; i < 6; i++)
        voc += std::make_pair(words[i], "x");
    for (int i = 0; i < 6; i++)
    {
        voc -= words[i];
        CHECK_EQUAL(5 - i, voc.GetWordsCount());
    }
    CHECK_THROW(voc["a"], Vocabulary::WordNotFoundException);
}

TEST(ClearEmptySafe)
{
    Vocabulary voc;
    voc.Clear();
    CHECK_EQUAL(0, voc.GetWordsCount());
}

TEST(ClearNonEmptyResetsCount)
{
    Vocabulary voc;
    voc += std::make_pair(APPLE, JABLOKO);
    voc += std::make_pair(BANANA, "banan");
    voc += std::make_pair(CHERRY, "vishnya");
    voc.Clear();
    CHECK_EQUAL(0, voc.GetWordsCount());
}

TEST(ClearTwiceSafe)
{
    Vocabulary voc;
    voc += std::make_pair(APPLE, JABLOKO);
    voc.Clear();
    voc.Clear();
    CHECK_EQUAL(0, voc.GetWordsCount());
}

TEST(ClearThenAddWorks)
{
    Vocabulary voc;
    voc += std::make_pair(APPLE, JABLOKO);
    voc.Clear();
    voc += std::make_pair(BANANA, "banan");
    CHECK_EQUAL(1, voc.GetWordsCount());
    CHECK_EQUAL("banan", voc[BANANA]);
}

TEST(AssignEmptyToEmpty)
{
    Vocabulary source;
    Vocabulary target;
    target = source;
    CHECK_EQUAL(0, target.GetWordsCount());
}

TEST(AssignNonEmptyToEmpty)
{
    Vocabulary source;
    source += std::make_pair(APPLE, JABLOKO);
    source += std::make_pair(BANANA, "banan");
    Vocabulary target;
    target = source;
    CHECK_EQUAL(2, target.GetWordsCount());
    CHECK_EQUAL(JABLOKO, target[APPLE]);
    CHECK_EQUAL("banan", target[BANANA]);
}

TEST(AssignOverNonEmptyReplacesData)
{
    Vocabulary source;
    source += std::make_pair(APPLE, JABLOKO);
    Vocabulary target;
    target += std::make_pair("dog", "sobaka");
    target += std::make_pair("cat", "kot");
    target = source;
    CHECK_EQUAL(1, target.GetWordsCount());
    CHECK_EQUAL(JABLOKO, target[APPLE]);
    CHECK_THROW(target["dog"], Vocabulary::WordNotFoundException);
}

TEST(SelfAssignmentKeepsData)
{
    Vocabulary voc;
    voc += std::make_pair(APPLE, JABLOKO);
    voc += std::make_pair(BANANA, "banan");
    voc = voc;
    CHECK_EQUAL(2, voc.GetWordsCount());
    CHECK_EQUAL(JABLOKO, voc[APPLE]);
    CHECK_EQUAL("banan", voc[BANANA]);
}

TEST(AssignmentIsDeepCopy)
{
    Vocabulary source;
    source += std::make_pair(APPLE, JABLOKO);
    source += std::make_pair(BANANA, "banan");
    Vocabulary target;
    target = source;
    source[APPLE] = "izmeneno";
    CHECK_EQUAL("izmeneno", source[APPLE]);
    CHECK_EQUAL(JABLOKO, target[APPLE]);
    source -= BANANA;
    CHECK_EQUAL(1, source.GetWordsCount());
    CHECK_EQUAL("banan", target[BANANA]);
}

TEST(AssignmentFromEmptyClears)
{
    Vocabulary source;
    Vocabulary target;
    target += std::make_pair(APPLE, JABLOKO);
    target = source;
    CHECK_EQUAL(0, target.GetWordsCount());
    CHECK_THROW(target[APPLE], Vocabulary::WordNotFoundException);
}

TEST(AssignmentThenClear)
{
    Vocabulary source;
    source += std::make_pair(BANANA, "banan");
    Vocabulary target;
    target = source;
    target.Clear();
    CHECK_EQUAL(0, target.GetWordsCount());
    CHECK_EQUAL("banan", source[BANANA]);
}

TEST(CopyConstructorCopiesEmptyCount)
{
    Vocabulary original;
    Vocabulary copy(original);
    CHECK_EQUAL(0, copy.GetWordsCount());
}

TEST(DestructorNoCrashManyObjects)
{
    for (int i = 0; i < 100; i++)
    {
        Vocabulary voc;
        voc += std::make_pair(APPLE, JABLOKO);
        voc += std::make_pair(BANANA, "banan");
        voc += std::make_pair(CHERRY, "vishnya");
    }
    CHECK(true);
}

TEST(DestructorAfterRemovals)
{
    for (int i = 0; i < 50; i++)
    {
        Vocabulary voc;
        voc += std::make_pair("a", "1");
        voc += std::make_pair("b", "2");
        voc += std::make_pair("c", "3");
        voc -= "b";
        voc -= "a";
        voc.Clear();
    }
    CHECK(true);
}

TEST(DestructorAfterAssignment)
{
    Vocabulary source;
    source += std::make_pair(APPLE, JABLOKO);
    for (int i = 0; i < 50; i++)
    {
        Vocabulary target;
        target = source;
    }
    CHECK(true);
}

int main()
{
    return UnitTest::RunAllTests();
}