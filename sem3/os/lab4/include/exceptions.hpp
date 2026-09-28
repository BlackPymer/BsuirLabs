#include <string>
class FileAlreadyExistsException : public std::exception
{
public:
    FileAlreadyExistsException() : std::exception() {}
};

class NotEnoughtSpaceException : public std::exception
{
public:
    NotEnoughtSpaceException() : std::exception() {}
};

class FileNotFoundException : public std::exception
{
public:
    FileNotFoundException() : std::exception() {}
};
