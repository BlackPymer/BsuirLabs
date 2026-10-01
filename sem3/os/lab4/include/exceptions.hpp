#pragma once
#include <exception>

class FileAlreadyExistsException : public std::exception
{
public:
    FileAlreadyExistsException() : std::exception() {}
    const char *what() const noexcept override { return "file already exists"; }
};

class NotEnoughtSpaceException : public std::exception
{
public:
    NotEnoughtSpaceException() : std::exception() {}
    const char *what() const noexcept override { return "not enough space on the filesystem"; }
};

class FileNotFoundException : public std::exception
{
public:
    FileNotFoundException() : std::exception() {}
    const char *what() const noexcept override { return "file not found"; }
};
