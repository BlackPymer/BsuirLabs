// Вариант 61: библиотека — издания, журналы, книги, полки, шкафы,
// читатель, библиотекарь, администратор.
// Исправленная и дописанная реализация (отдельный файл, main.cpp не тронут).

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>
#include <stdexcept>

// ============================ Издания ============================

class Creation
{
public:
    Creation(std::string title, std::string publisher, int year, std::string text)
        : title_(std::move(title)), publisher_(std::move(publisher)),
          year_(year), text_(std::move(text)) {}
    virtual ~Creation() = default;

    const std::string &getTitle() const { return title_; }
    const std::string &getPublisher() const { return publisher_; }
    int getYear() const { return year_; }
    const std::string &getText() const { return text_; }

private:
    std::string title_;
    std::string publisher_;
    int year_;
    std::string text_;
};

// Книга: + авторы, у каждого известны годы жизни.
struct Author
{
    std::string name;
    int birthYear;
    int deathYear;

    friend bool operator==(const Author &a, const Author &b)
    {
        return a.name == b.name && a.birthYear == b.birthYear && a.deathYear == b.deathYear;
    }
};

class Book : public Creation
{
public:
    Book(std::string title, std::string publisher, int year, std::string text,
         std::vector<Author> authors)
        : Creation(std::move(title), std::move(publisher), year, std::move(text)),
          authors_(std::move(authors)) {}

    const std::vector<Author> &getAuthors() const { return authors_; }

private:
    std::vector<Author> authors_;
};

// Журнал: + месяц выпуска, разбивка на темы; две темы не могут совпадать
// для одного и того же журнала, поэтому темы хранятся как map<тема, текст>.
class Magazine : public Creation
{
public:
    Magazine(std::string title, std::string publisher, int year, int month,
             std::map<std::string, std::string> topics)
        : Creation(std::move(title), std::move(publisher), year, ""),
          month_(month), topics_(std::move(topics)) {}

    int getMonth() const { return month_; }
    const std::map<std::string, std::string> &getTopics() const { return topics_; }

    // Текст темы конкретного журнала (совпадает список тем — совпадает журнал).
    std::string getTopicText(const std::string &topic) const
    {
        auto it = topics_.find(topic);
        return it == topics_.end() ? std::string{} : it->second;
    }
    bool hasTopic(const std::string &topic) const { return topics_.count(topic) != 0; }

private:
    int month_;
    std::map<std::string, std::string> topics_;
};

// ============================ Полки и шкафы ============================

class Polka
{
public:
    explicit Polka(int maxCreations) : maxCreations_(maxCreations) {}

    bool addCreation(const std::shared_ptr<Creation> &cr)
    {
        if ((int)items_.size() >= maxCreations_ || !cr)
            return false;
        items_.push_back(cr);
        return true; // <-- в исходнике return true отсутствовал (UB)
    }

    bool empty() const { return items_.empty(); }
    bool full() const { return (int)items_.size() >= maxCreations_; }
    size_t size() const { return items_.size(); }

    std::shared_ptr<Creation> takeByTitleAndText(const std::string &title, const std::string &text)
    {
        for (auto it = items_.begin(); it != items_.end(); ++it)
        {
            if ((*it)->getTitle() == title && (*it)->getText() == text)
            {
                auto found = *it;
                items_.erase(it);
                return found;
            }
        }
        return nullptr; // <-- в исходнике возвращался items_[0] (выдача не той книги + UB на пустой полке)
    }

    // Поиск: по темам (для журналов), по авторам, по году издания и по году жизни автора.
    std::vector<std::shared_ptr<Creation>> getByFilters(
        const std::vector<Author> &wantedAuthors, const std::vector<std::string> &wantedTopics,
        int year, int authorLivingYear) const
    {
        std::vector<std::shared_ptr<Creation>> result;
        bool anyAuthor = wantedAuthors.empty();
        bool anyTopic = wantedTopics.empty();
        bool anyYear = (year == -1);

        for (const auto &cr : items_)
        {
            bool authorOk = anyAuthor;
            bool topicOk = anyTopic;

            if (auto book = std::dynamic_pointer_cast<Book>(cr))
            {
                if (!anyAuthor)
                {
                    for (const auto &a : book->getAuthors())
                    {
                        bool wanted = std::find(wantedAuthors.begin(), wantedAuthors.end(), a) != wantedAuthors.end();
                        bool alive = authorLivingYear == -1 ||
                                     (authorLivingYear >= a.birthYear && authorLivingYear <= a.deathYear);
                        if (wanted && alive)
                        {
                            authorOk = true;
                            break;
                        }
                    }
                }
            }

            if (auto mag = std::dynamic_pointer_cast<Magazine>(cr))
            {
                if (!anyTopic)
                {
                    for (const auto &t : wantedTopics)
                        if (mag->hasTopic(t))
                        {
                            topicOk = true;
                            break;
                        }
                }
                else
                {
                    topicOk = true;
                }
            }

            bool yearOk = anyYear || cr->getYear() == year; // <-- было != (наоборот)

            if (authorOk && topicOk && yearOk)
                result.push_back(cr);
        }
        return result;
    }

    std::vector<std::shared_ptr<Creation>> getAll() const { return items_; }

private:
    std::vector<std::shared_ptr<Creation>> items_;
    int maxCreations_;
};

// Базовый шкаф. Тематический шкаф проверяет тип издания и тему/автора.
class Skaf
{
public:
    explicit Skaf(int polksAmount)
    {
        polks_.reserve(polksAmount);
        for (int i = 0; i < polksAmount; ++i)
            polks_.push_back(std::make_shared<Polka>(POLKA_CAPACITY));
    }
    virtual ~Skaf() = default;

    virtual bool accepts(const std::shared_ptr<Creation> &) const { return true; }

    // Приоритет — тематический шкаф, свободное место на его полках.
    bool addCreation(const std::shared_ptr<Creation> &cr)
    {
        if (!accepts(cr))
            return false;
        for (auto &p : polks_)
            if (p->addCreation(cr))
                return true;
        return false; // тематический шкаф есть, но мест нет
    }

    std::shared_ptr<Creation> takeCreation(int polkaIndex, const std::string &title, const std::string &text)
    {
        if (polkaIndex < 0 || polkaIndex >= (int)polks_.size())
            return nullptr;
        return polks_[polkaIndex]->takeByTitleAndText(title, text);
    }

    std::vector<std::vector<std::shared_ptr<Creation>>> getByFilters(
        const std::vector<Author> &authors, const std::vector<std::string> &topics,
        int year, int livingYear) const
    {
        std::vector<std::vector<std::shared_ptr<Creation>>> res;
        res.reserve(polks_.size());
        for (const auto &p : polks_)
            res.push_back(p->getByFilters(authors, topics, year, livingYear));
        return res;
    }

    std::vector<std::vector<std::shared_ptr<Creation>>> getAll() const
    {
        std::vector<std::vector<std::shared_ptr<Creation>>> res;
        for (const auto &p : polks_)
            res.push_back(p->getAll());
        return res;
    }

    bool hasFreeSpace() const
    {
        return std::any_of(polks_.begin(), polks_.end(), [](const auto &p)
                           { return !p->full(); });
    }

protected:
    static constexpr int POLKA_CAPACITY = 5; // количество полок фиксировано вместимостью
    std::vector<std::shared_ptr<Polka>> polks_;
};

// Шкаф для журналов с одним заголовком (одной темой).
class SkafMagazine : public Skaf
{
public:
    SkafMagazine(int polksAmount, std::string topic)
        : Skaf(polksAmount), topic_(std::move(topic)) {}

    const std::string &getTopic() const { return topic_; }

    bool accepts(const std::shared_ptr<Creation> &cr) const override
    {
        auto mag = std::dynamic_pointer_cast<Magazine>(cr);
        return mag && mag->getTitle() == topic_;
    }

private:
    std::string topic_; // <-- в исходнике не инициализировался никогда
};

// Шкаф для книг одного автора.
class SkafAuthors : public Skaf
{
public:
    SkafAuthors(int polksAmount, std::string authorName)
        : Skaf(polksAmount), authorName_(std::move(authorName)) {}

    const std::string &getAuthorName() const { return authorName_; }

    bool accepts(const std::shared_ptr<Creation> &cr) const override
    {
        auto book = std::dynamic_pointer_cast<Book>(cr);
        if (!book)
            return false;
        return std::any_of(book->getAuthors().begin(), book->getAuthors().end(),
                           [&](const Author &a)
                           { return a.name == authorName_; });
    }

private:
    std::string authorName_;
};

// ============================ Библиотека ============================

class Library
{
public:
    Library(std::string name, std::string address)
        : name_(std::move(name)), address_(std::move(address)) {}

    void addSkaf(const std::shared_ptr<Skaf> &sk) { skafs_.push_back(sk); }
    const std::vector<std::shared_ptr<Skaf>> &getSkafs() const { return skafs_; }

    const std::string &getName() const { return name_; }
    const std::string &getAddress() const { return address_; }

    // Тематический шкаф для журнала с данным заголовком.
    std::shared_ptr<SkafMagazine> findMagazineSkaf(const std::string &title) const
    {
        for (const auto &s : skafs_)
            if (auto sm = std::dynamic_pointer_cast<SkafMagazine>(s))
                if (sm->getTopic() == title)
                    return sm;
        return nullptr;
    }

    // Шкаф, посвящённый любому из указанных авторов.
    std::shared_ptr<SkafAuthors> findAuthorSkaf(const std::vector<Author> &authors) const
    {
        for (const auto &s : skafs_)
            if (auto sa = std::dynamic_pointer_cast<SkafAuthors>(s))
                for (const auto &a : authors)
                    if (a.name == sa->getAuthorName())
                        return sa;
        return nullptr;
    }

    // Любой шкаф, куда издание влезет (сначала тематические — приоритет).
    std::shared_ptr<Skaf> findAnySkafFor(const std::shared_ptr<Creation> &cr) const
    {
        for (const auto &s : skafs_)
            if (s->accepts(cr) && s->hasFreeSpace())
                return s;
        return nullptr;
    }

private:
    std::vector<std::shared_ptr<Skaf>> skafs_;
    std::string name_;
    std::string address_;
};

// ============================ Журнал выдач ============================

struct IssueRecord
{
    std::string readerName;
    std::string creationTitle;
    std::string creationText;
    bool returned = false;
};

// ============================ Библиотекарь ============================

class BookKeeper
{
public:
    BookKeeper(std::string name, Library *lib) : name_(std::move(name)), homeLib_(lib) {}

    void assignLibrary(Library *lib) { homeLib_ = lib; }
    void leaveLibrary() { homeLib_ = nullptr; }
    bool employed() const { return homeLib_ != nullptr; }
    const std::string &getName() const { return name_; }

    // Расстановка купленных изданий: сначала тематический шкаф, потом любой.
    void shelveCreation(const std::shared_ptr<Creation> &cr)
    {
        ensureEmployed();
        auto thematic = homeLib_->findAnySkafFor(cr);
        if (!thematic)
            throw std::runtime_error("NoFreeSkafsException: нет места ни в одном шкафу");
        thematic->addCreation(cr);
    }

    // Поиск издания по фильтрам: темы / авторы / год издания / год жизни автора.
    std::vector<std::shared_ptr<Creation>> search(
        const std::vector<Author> &authors, const std::vector<std::string> &topics,
        int year, int livingYear) const
    {
        ensureEmployed();
        std::vector<std::shared_ptr<Creation>> found;
        for (const auto &s : homeLib_->getSkafs())
            for (const auto &perPolka : s->getByFilters(authors, topics, year, livingYear))
                for (const auto &cr : perPolka)
                    found.push_back(cr);
        return found;
    }

    // Выдача: находит издание на полках, убирает его и делает пометку в журнале.
    std::shared_ptr<Creation> giveCreation(const std::string &readerName,
                                           const std::string &title, const std::string &text)
    {
        ensureEmployed();
        // Обходим каждую полку каждого шкафа.
        for (const auto &s : homeLib_->getSkafs())
        {
            auto perPolka = s->getAll();
            for (size_t i = 0; i < perPolka.size(); ++i)
            {
                auto cr = s->takeCreation((int)i, title, text);
                if (cr)
                {
                    issueLog_.push_back({readerName, title, text, false});
                    return cr;
                }
            }
        }
        return nullptr; // не нашлось
    }

    // Возврат издания читателем: ставим на место (по тем же правилам) и закрываем запись.
    bool acceptReturn(const std::shared_ptr<Creation> &cr, const std::string &readerName)
    {
        ensureEmployed();
        // закрываем запись в журнале
        for (auto it = issueLog_.rbegin(); it != issueLog_.rend(); ++it)
        {
            if (!it->returned && it->readerName == readerName &&
                it->creationTitle == cr->getTitle() && it->creationText == cr->getText())
            {
                it->returned = true;
                shelveCreation(cr);
                return true;
            }
        }
        return false; // такой выдачи не было
    }

    const std::vector<IssueRecord> &getIssueLog() const { return issueLog_; }

private:
    void ensureEmployed() const
    {
        if (!homeLib_)
            throw std::runtime_error("Библиотекарь не трудоустроен");
    }

    std::string name_;
    Library *homeLib_ = nullptr;
    std::vector<IssueRecord> issueLog_; // журнал выданных изданий
};

// ============================ Администратор ============================

class Admin
{
public:
    Admin(std::string name, std::shared_ptr<Library> lib)
        : name_(std::move(name)), library_(std::move(lib)) {}

    std::shared_ptr<Library> getLibrary() const { return library_; }
    std::shared_ptr<BookKeeper> getBookKeeper() const { return keeper_; }

    std::shared_ptr<Skaf> createSkaf(int polks)
    {
        auto sk = std::make_shared<Skaf>(polks);
        library_->addSkaf(sk);
        return sk;
    }

    std::shared_ptr<SkafMagazine> createMagazineSkaf(int polks, const std::string &topic)
    {
        auto sk = std::make_shared<SkafMagazine>(polks, topic);
        library_->addSkaf(sk);
        return sk;
    }

    std::shared_ptr<SkafAuthors> createAuthorSkaf(int polks, const std::string &author)
    {
        auto sk = std::make_shared<SkafAuthors>(polks, author);
        library_->addSkaf(sk);
        return sk;
    }

    // Назначить / уволить библиотекаря.
    void hireBookKeeper(const std::shared_ptr<BookKeeper> &keeper)
    {
        if (keeper_)
            fireBookKeeper();
        keeper_ = keeper;
        keeper_->assignLibrary(library_.get());
    }

    void fireBookKeeper()
    {
        if (keeper_)
        {
            keeper_->leaveLibrary();
            keeper_ = nullptr;
        }
    }

    // Покупка изданий: админ покупает и приносит в библиотеку,
    // расставляет hired библиотекарь.
    void buyCreation(const std::shared_ptr<Creation> &cr)
    {
        if (!keeper_)
            throw std::runtime_error("Нет библиотекаря — некому расставлять издания");
        keeper_->shelveCreation(cr);
    }

private:
    std::string name_;
    std::shared_ptr<Library> library_;
    std::shared_ptr<BookKeeper> keeper_;
};

// ============================ Читатель ============================

class Reader
{
public:
    Reader(std::string name, int maxCreations)
        : name_(std::move(name)), maxCreations_(maxCreations) {}

    const std::string &getName() const { return name_; }

    // Приход в библиотеку через администратора, поиск через библиотекаря.
    std::vector<std::shared_ptr<Creation>> searchIn(const std::shared_ptr<Library> & /*lib*/,
                                                    const std::shared_ptr<BookKeeper> &keeper,
                                                    const std::vector<Author> &authors,
                                                    const std::vector<std::string> &topics,
                                                    int year, int livingYear) const
    {
        if (!keeper || !keeper->employed())
            throw std::runtime_error("В библиотеке нет библиотекаря");
        return keeper->search(authors, topics, year, livingYear);
    }

    // Просит библиотекаря выдать издание; не может держать больше maxCreations со всех библиотек.
    std::shared_ptr<Creation> requestCreation(const std::shared_ptr<BookKeeper> &keeper,
                                              const std::string &title, const std::string &text)
    {
        if (held_.size() >= (size_t)maxCreations_)
            throw std::runtime_error("Превышен лимит изданий у читателя: " + name_);
        auto cr = keeper->giveCreation(name_, title, text);
        if (cr)
            held_.push_back(cr);
        return cr;
    }

    // Вернуть издание своему библиотекарю.
    bool returnCreation(const std::shared_ptr<BookKeeper> &keeper, const std::shared_ptr<Creation> &cr)
    {
        auto it = std::find(held_.begin(), held_.end(), cr);
        if (it == held_.end())
            return false;
        if (!keeper->acceptReturn(cr, name_))
            return false;
        held_.erase(it);
        return true;
    }

    size_t heldCount() const { return held_.size(); }

private:
    std::string name_;
    int maxCreations_;
    std::vector<std::shared_ptr<Creation>> held_;
};

// ============================ Демонстрация ============================

int main()
{
    // Админ владеет библиотекой, создаёт шкафы.
    auto library = std::make_shared<Library>("Центральная", "ул. Пушкина, 1");
    Admin admin("Анна", library);

    auto magSkaf = admin.createMagazineSkaf(2, "Наука и жизнь");
    auto authSkaf = admin.createAuthorSkaf(2, "А. Чехов");
    admin.createSkaf(2); // общий шкаф

    // Нанимаем библиотекаря.
    auto keeper = std::make_shared<BookKeeper>("Мария", library.get());
    admin.hireBookKeeper(keeper);

    // Покупаем издания и расставляем.
    admin.buyCreation(std::make_shared<Magazine>("Наука и жизнь", "Наука", 2020, 5,
                                                 std::map<std::string, std::string>{{"Физика", "текст про физику"},
                                                                                    {"Биология", "текст про биологию"}}));
    admin.buyCreation(std::make_shared<Book>("Палата №6", "Дрофа", 1892, "текст палаты",
                                             std::vector<Author>{{"А. Чехов", 1860, 1904}}));
    admin.buyCreation(std::make_shared<Book>("Война и мир", "Дрофа", 1869, "текст войны",
                                             std::vector<Author>{{"Л. Толстой", 1828, 1910}}));

    // Читатель.
    Reader reader("Иван", 2); // может держать максимум 2 издания со всех библиотек

    // Ищем книги Чехова, изданные в 1892, когда автор был жив.
    auto found = reader.searchIn(library, keeper, {{"А. Чехов", 1860, 1904}}, {}, 1892, 1892);
    std::cout << "Найдено по фильтрам: " << found.size() << "\n";

    // Просим выдать.
    auto book = reader.requestCreation(keeper, "Палата №6", "текст палаты");
    std::cout << "Выдано: " << (book ? book->getTitle() : "не нашлось") << "\n";
    std::cout << "У читателя изданий: " << reader.heldCount() << "\n";

    // Возврат.
    if (book)
    {
        reader.returnCreation(keeper, book);
        std::cout << "Возврат выполнен, у читателя изданий: " << reader.heldCount() << "\n";
    }

    // Журнал выдач библиотекаря.
    std::cout << "Журнал выдач:\n";
    for (const auto &r : keeper->getIssueLog())
        std::cout << "  " << r.readerName << " -> " << r.creationTitle
                  << (r.returned ? " (возвращено)" : " (на руках)") << "\n";

    return 0;
}
