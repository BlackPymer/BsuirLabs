/**
 * @file vocabulary.hpp
 * @brief Заголовочный файл словаря на базе бинарного дерева поиска.
 *
 * @mainpage Словарь (Vocabulary)
 *
 * Реализация словаря «слово → перевод» на базе бинарного дерева поиска.
 *
 * Словарь поддерживает добавление пар «слово-перевод» (operator+=),
 * поиск перевода по слову (operator[]), удаление слов (operator-=),
 * загрузку пар из файла, копирование через оператор присваивания
 * и очистку (Clear). Все узлы дерева удаляются в деструкторе.
 *
 * Сборка и запуск тестов:
 * @code
 * make tests
 * ./unit_tests
 * @endcode
 *
 * Генерация документации:
 * @code
 * make docs
 * @endcode
 */

#include <string>
#include <vector>
#include <memory>
#include <utility>

/**
 * @brief Узел бинарного дерева поиска.
 *
 * Хранит слово и его перевод, а также указатели на правое и левое
 * поддеревья. Правое поддерево содержит слова, идущие раньше по
 * алфавиту (или равные), левое — позже.
 */
struct WordPair
{
    std::string Word;      ///< Слово (ключ).
    std::string Translation; ///< Перевод слова.
    WordPair *right = NULL;  ///< Указатель на правое поддерево.
    WordPair *left = NULL;   ///< Указатель на левое поддерево.

    /**
     * @brief Конструктор по слову и переводу.
     * @param word Слово (ключ).
     * @param translation Перевод слова.
     */
    WordPair(std::string word, std::string translation)
    {
        Word = word;
        Translation = translation;
    }

    /**
     * @brief Конструктор из пары «слово-перевод».
     * @param other Пара, из которой берутся слово и перевод.
     */
    WordPair(const std::pair<std::string, std::string> &other)
    {
        Word = other.first;
        Translation = other.second;
    }
};

/**
 * @brief Класс словаря на базе бинарного дерева поиска.
 *
 * Хранит указатель на корень дерева (@ref _words) и количество
 * пар (@ref _wordsCount).
 */
class Vocabulary
{
public:
    /**
     * @brief Конструктор, загружающий словарь из файла.
     *
     * Каждая строка файла должна иметь формат @c слово:перевод.
     *
     * @param filename Путь к файлу со словарём.
     * @throw FileOpeningFailedException Если файл не удалось открыть.
     */
    Vocabulary(std::string filename);

    /// @brief Конструктор по умолчанию создаёт пустой словарь.
    Vocabulary();

    /**
     * @brief Копирующий конструктор.
     * @param other Копируемый словарь.
     */
    Vocabulary(const Vocabulary &other);

    /**
     * @brief Возвращает перевод слова по ключу.
     *
     * @param word Искомое слово.
     * @return Ссылка на перевод.
     * @throw WordNotFoundException Если слово не найдено или словарь пуст.
     */
    std::string &operator[](std::string word);

    /**
     * @brief Добавляет пару «слово-перевод» в словарь.
     * @param word Пара слов.
     */
    void operator+=(const std::pair<std::string, std::string> &word);

    /**
     * @brief Удаляет слово из словаря.
     * @param word Удаляемое слово.
     */
    void operator-=(const std::string &word);

    /**
     * @brief Оператор присваивания (глубокое копирование).
     * @param other Копируемый словарь.
     */
    void operator=(const Vocabulary &other);

    /**
     * @brief Возвращает количество пар в словаре.
     * @return Количество пар слово-перевод.
     */
    int GetWordsCount();

    /**
     * @brief Исключение: не удалось открыть файл словаря.
     */
    class FileOpeningFailedException : public std::exception
    {
    };

    /**
     * @brief Исключение: слово не найдено в словаре.
     */
    class WordNotFoundException : public std::exception
    {
    };

    /**
     * @brief Полностью очищает словарь.
     *
     * Удаляет все узлы дерева и сбрасывает счётчик пар в ноль.
     */
    void Clear();

    /**
     * @brief Деструктор. Удаляет все узлы дерева через @ref Clear.
     */
    ~Vocabulary();

private:
    WordPair *_words;  ///< Корень бинарного дерева поиска.
    int _wordsCount;   ///< Количество пар слово-перевод.
};