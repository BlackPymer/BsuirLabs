#include "block.hpp"
#include "file.hpp"
#include <vector>

class FileSystem
{
public:
    const int BLOCK_SIZE = 4096;
    const int BLOCK_COUNT = 100;

    FileSystem();
    ~FileSystem();

    void CreateFile(const std::string &name);
    void WriteToFile(const std::string &name, const std::string &data);
    void AppendToFile(const std::string &name, const std::string &data);
    std::string ReadFromFile(const std::string &name);
    bool IsFileExists(const std::string &name);
    void DeleteFile(const std::string &name);
    void Damp();

private:
    std::vector<Block> blocks_;
    std::vector<File> table_files_;
    int free_blocks_;

    void freeBlocks_(int block_index, int blocks_count);
    int optimaze_();
};