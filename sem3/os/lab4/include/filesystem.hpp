#pragma once

#include "block.hpp"
#include "file.hpp"
#include <string>
#include <vector>

class FileSystem
{
public:
    static constexpr int BLOCK_SIZE = 4096;
    static constexpr int BLOCK_COUNT = 100;

    FileSystem();
    ~FileSystem();

    void CreateFile(const std::string &name);
    void WriteToFile(const std::string &name, const std::string &data);
    void AppendToFile(const std::string &name, const std::string &data);
    std::string ReadFromFile(const std::string &name);
    bool IsFileExists(const std::string &name);
    void DeleteFile(const std::string &name);
    void CopyFile(std::string new_name, const std::string &old_name);
    std::string Dump();

private:
    std::vector<Block> blocks_;
    std::vector<File> table_files_;
    int free_blocks_;

    void freeBlocks_(int block_index, int blocks_count);
    int optimaze_();
    int blocksCount_(int size) const;
    int findFreeBlocks_(int blocks_count);
};
