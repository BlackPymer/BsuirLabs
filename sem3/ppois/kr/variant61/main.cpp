#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>
#include <exception>
class Creation
{
public:
    Creation(std::string title, std::string publishment_name, uint year_of_creation, std::string text)
    {
        title_ = title;
        publishment_name_ = publishment_name;
        year_of_creation_ = year_of_creation;
        text_ = text;
    }
    std::string getTitle() { return title_; }
    std::string getPublishmentName() { return publishment_name_; }
    uint getYear() { return year_of_creation_; }
    std::string getText() { return text_; }
    virtual ~Creation() {}

protected:
    std::string title_, publishment_name_, text_;
    uint year_of_creation_;
};

class Magazine : public Creation
{
public:
    Magazine(std::string title, std::string publishment_name, uint year_of_creation, std::string text, uint month,
             std::map<std::string, std::string> topics, std::string topic)
        : Creation(title, publishment_name, year_of_creation, text)
    {
        month_ = month;
        book_topics_ = topics;
        topic_ = topic;
    }

    uint getMonth() { return month_; }
    std::map<std::string, std::string> getTopics() { return book_topics_; }
    std::string getTopic() { return topic_; }

private:
    uint month_;
    std::map<std::string, std::string> book_topics_;
    std::string topic_;
};
struct Author
{
    std::string name;
    int birthYear;
    int deathYear;
};
class Book : public Creation
{
public:
    Book(std::string title, std::string publishment_name, uint year_of_creation, std::string text, std::vector<Author> authors)
        : Creation(title, publishment_name, year_of_creation, text)
    {
        authors_ = authors;
    }

    std::vector<Author> getAuthors()
    {
        return authors_;
    }

private:
    std::vector<Author> authors_;
};

class Polka
{
public:
    Polka(int max_creations)
    {
        max_creations_ = max_creations;
    }
    bool addCreation(std::shared_ptr<Creation> cr)
    {
        if (things_on_polka_.size() == max_creations_)
            return false;
        things_on_polka_.push_back(cr);
    }
    std::shared_ptr<Creation> takeCreation(std::string title, std::string text)
    {
        for (int i = 0; i < things_on_polka_.size(); ++i)
        {
            if (things_on_polka_[i]->getText() == text && things_on_polka_[i]->getTitle() == title)
            {
                auto chosen = things_on_polka_[i];
                things_on_polka_.erase(things_on_polka_.begin() + i);
                return chosen;
            }
        }
        return things_on_polka_[0];
    }
    std::vector<std::shared_ptr<Creation>> getByFilters(std::vector<Author> wantedAuthors, int year, int year_of_living)
    {
        std::vector<std::shared_ptr<Creation>> result;
        for (auto cr : things_on_polka_)
        {
            if (wantedAuthors.size() != 0)
            {
                std::shared_ptr<Book> book = std::dynamic_pointer_cast<Book>(cr);
                if (book == nullptr)
                    continue;
                if (year != -1 && book->getYear() != year)
                    continue;
                std::vector<Author> a = book->getAuthors();
                for (Author author : a)
                {

                    if ((year_of_living == -1 ||
                         (year_of_living > author.birthYear && year_of_living < author.deathYear)) &&
                        std::find(wantedAuthors.begin(), wantedAuthors.end(), author) != wantedAuthors.end())

                    {
                        result.push_back(cr);
                        break;
                    }
                }
            }
            else
            {
                if (year == -1)
                {
                    return getAllThings();
                }
                else
                {
                    if (cr->getYear() != year)
                    {
                        result.push_back(cr);
                    }
                }
            }
        }
        return result;
    }
    std::vector<std::shared_ptr<Creation>> getAllThings()
    {
        return things_on_polka_;
    }

private:
    std::vector<std::shared_ptr<Creation>> things_on_polka_;
    int max_creations_;
};

class Skaf
{
public:
    Skaf(int amount_of_polks)
    {
        max_polks_amount_ = amount_of_polks;
    }
    std::vector<std::vector<std::shared_ptr<Creation>>> getAllCreationsByFilters(std::vector<Author> wantedAuthors, int year, int year_of_living)
    {
        std::vector<std::vector<std::shared_ptr<Creation>>> result;
        for (auto polk : polks_)
        {
            result.push_back(polk->getByFilters(wantedAuthors, year, year_of_living));
        }
        return result;
    }
    std::vector<std::vector<std::shared_ptr<Creation>>> getAllCreations()
    {
        std::vector<std::vector<std::shared_ptr<Creation>>> result;
        for (auto polk : polks_)
        {
            result.push_back(polk->getAllThings());
        }
        return result;
    }
    virtual bool addCreation(std::shared_ptr<Creation> cr)
    {
        for (auto &polk : polks_)
        {
            if (polk->addCreation(cr))
            {
                min_year_of_creation = std::min(min_year_of_creation, cr->getYear());
                max_year_of_creation = std::max(max_year_of_creation, cr->getYear());
                return true;
            }
        }
        return false;
    }
    std::shared_ptr<Creation> takeCreation(int polk_index, std::string text, std::string title)
    {
        if (polk_index < 0 || polk_index >= polks_.size())
            return nullptr;
        return polks_[polk_index]->takeCreation(title, text);
    }

protected:
    int max_polks_amount_;
    std::vector<std::shared_ptr<Polka>> polks_;
    uint min_year_of_creation;
    uint max_year_of_creation;

private:
    std::vector<Author> authors_;
};

class SkafMagazine : public Skaf
{
public:
    SkafMagazine(int amount_of_polks) : Skaf(amount_of_polks) {};
    std::string getMagazineTopic()
    {
        return magazine_topic_;
    }
    bool addCreation(std::shared_ptr<Creation> cr) override
    {
        std::shared_ptr<Magazine> m = std::dynamic_pointer_cast<Magazine>(cr);
        if (m == nullptr)
            return false;
        if (m->getTopic() != magazine_topic_)
        {
            return false;
        }
        return Skaf::addCreation(cr);
    }

private:
    std::string magazine_topic_;
};

class SkafAuthors : public Skaf
{
public:
    SkafAuthors(int amount_of_polks) : Skaf(amount_of_polks) {};

    Author getAuthor() { return author_; }
    bool addCreation(std::shared_ptr<Creation> cr) override
    {
        std::shared_ptr<Book> book = std::dynamic_pointer_cast<Book>(cr);
        if (book == nullptr)
            return false;
        return Skaf::addCreation(cr);
    }

private:
    Author author_;
};

class Library
{
public:
    Library(std::string name, std::string address)
    {
        name_ = name;
        address_ = address;
    }
    const std::vector<std::shared_ptr<Skaf>> &getSkafs() { return skafs_; }

    void addSkaf(std::shared_ptr<Skaf> sk) { skafs_.push_back(sk); }

    bool hasSkaf()
    {
        if (skafs_.size() == 0)
        {
            return false;
        }

        for (int i = 0; i < skafs_.size(); ++i)
        {
            std::shared_ptr<Skaf> sk = std::dynamic_pointer_cast<Skaf>(skafs_[i]);
            if (sk != nullptr)
            {
                return true;
            }
        }

        return false;
    }

    bool hasSkafAuthors(std::vector<Author> authors)
    {
        if (skafs_.size() == 0)
        {
            return false;
        }

        for (int i = 0; i < skafs_.size(); ++i)
        {
            std::shared_ptr<SkafAuthors> sk = std::dynamic_pointer_cast<SkafAuthors>(skafs_[i]);
            if (sk != nullptr)
            {
                for (int j = 0; j < authors.size(); ++j)
                {
                    if (authors[j].name == sk->getAuthor().name)
                    {
                        return true;
                    }
                }
                return false;
            }
        }

        return false;
    }

    bool hasSkafMagazin(std::string topic)
    {
        if (skafs_.size() == 0)
        {
            return false;
        }

        for (int i = 0; i < skafs_.size(); ++i)
        {
            std::shared_ptr<SkafMagazine> sk = std::dynamic_pointer_cast<SkafMagazine>(skafs_[i]);
            if (sk != nullptr && topic == sk->getMagazineTopic())
            {
                return true;
            }
        }

        return false;
    }

    const std::shared_ptr<SkafMagazine> getSkafMagazin(std::string topic)
    {
        for (int i = 0; i < skafs_.size(); ++i)
        {
            std::shared_ptr<SkafMagazine> sk = std::dynamic_pointer_cast<SkafMagazine>(skafs_[i]);
            if (sk != nullptr && topic == sk->getMagazineTopic())
            {
                return sk;
            }
        }
    }

    const std::shared_ptr<Skaf> getSkafAuthors(std::vector<Author> authors)
    {
        for (int i = 0; i < skafs_.size(); ++i)
        {
            std::shared_ptr<SkafAuthors> sk = std::dynamic_pointer_cast<SkafAuthors>(skafs_[i]);
            if (sk != nullptr)
            {
                for (int j = 0; j < authors.size(); ++j)
                {
                    if (authors[j].name == sk->getAuthor().name)
                    {
                        return skafs_[i];
                    }
                }
            }
        }
    }
    std::string getName() { return name_; }
    std::string getAddress() { return address_; }

private:
    std::vector<std::shared_ptr<Skaf>> skafs_;
    std::string address_;
    std::string name_;
};
class NoFreeSkafsException : std::exception
{
};
class BookKeeper
{
public:
    BookKeeper(Library *lib) : home_lib_(lib) {};

    void addLib(Library *lib) { home_lib_ = lib; }

    void deleteLib() { home_lib_ = nullptr; }

    bool isLibness()
    {
        if (home_lib_ == nullptr)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    void addCreation(std::shared_ptr<Creation> cr)
    {
        std::shared_ptr<Book> book = std::dynamic_pointer_cast<Book>(cr);
        std::shared_ptr<Magazine> magaz = std::dynamic_pointer_cast<Magazine>(cr);

        if (book != nullptr)
        {
            if (home_lib_->hasSkafAuthors(book->getAuthors()))
            {
                std::shared_ptr<Skaf> skf = home_lib_->getSkafAuthors(book->getAuthors());
                bool a = skf->addCreation(cr);
                if (a)
                    return;
            }
        }

        if (magaz != nullptr)
        {
            if (home_lib_->hasSkafMagazin(magaz->getTopic()))
            {
                std::shared_ptr<Skaf> skf = home_lib_->getSkafMagazin(magaz->getTopic());
                bool a = skf->addCreation(cr);
                if (a)
                    return;
            }
        }

        for (auto skf : home_lib_->getSkafs())
        {
            if (skf->addCreation(cr))
                return;
        }
        throw NoFreeSkafsException();
    }

    std::shared_ptr<Creation> giveBook() {}

private:
    Library *home_lib_;
};

class Admin
{
public:
    Admin(Library *lib) : home_lib_(lib) {};

    Library *tellLibrary()
    {
        return home_lib_;
    }

    BookKeeper *tellBookKeeper()
    {
        return keeper_;
    }

    bool makeSkaf(int polks_amount)
    {
        auto skf = std::make_shared<Skaf>(polks_amount);
        home_lib_->addSkaf(skf);
    }

    bool makeSkafAuthor(int polks_amount)
    {
        auto skf = std::make_shared<SkafAuthors>(polks_amount);
        home_lib_->addSkaf(skf);
    }

    bool makeSkafMagazine(int polks_amount)
    {
        auto skf = std::make_shared<SkafMagazine>(polks_amount);
        home_lib_->addSkaf(skf);
    }

    void hireBookKeaper(BookKeeper *ded)
    {
        ded->addLib(home_lib_);
        if (keeper_ != nullptr)
            fireBookKeaper();
        keeper_ = ded;
    }
    void fireBookKeaper()
    {
        keeper_->deleteLib();
        keeper_ = nullptr;
    }

    void addCreation(std::shared_ptr<Creation> cr)
    {
        if (keeper_ != nullptr)
        {
            keeper_->addCreation(cr);
        }
    }

private:
    Library *home_lib_;
    BookKeeper *keeper_;
};

class Reader
{
public:
    Reader(Admin *adm, int max_creations)
    {
        adm_ = adm;
        max_creations_ = max_creations;
        keeper_ = adm->tellBookKeeper();
        sks_ = adm_->tellLibrary()->getSkafs();
    }

    void takeCreation(){
        
    }
    

private:
    Admin *adm_;
    BookKeeper *keeper_;
    std::vector<std::shared_ptr<Skaf>> sks_;
    std::vector<std::shared_ptr<Creation>> have_creations_;
    int max_creations_;
};
