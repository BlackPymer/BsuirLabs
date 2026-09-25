#include "include/filesystem.hpp"
#include "include/exceptions.hpp"

FileSystem::FileSystem()
{
    blocks_ = std::vector<Block>(BLOCK_COUNT, Block(BLOCK_SIZE));
    table_files_ = std::vector<File>();
    free_blocks_ = BLOCK_COUNT;
}

void FileSystem::CreateFile(const std::string &name)
{
    if (IsFileExists(name))
        throw FileAlreadyExistsException();

    File file;
    file.name = name;
    file.size = 0;
}

void FileSystem::WriteToFile(const std::string &name, const std::string &data)
{
    if (!IsFileExists(name))
        CreateFile(name);

    for (int i = 0; i < table_files_.size(); i++)
    {
        if (table_files_[i].name == name)
        {
            if (table_files_[i].size != 0)
            {
                freeBlocks_(table_files_[i].firstBlockIndex, table_files_[i].size / BLOCK_SIZE);
                table_files_[i].firstBlockIndex = 0;
                table_files_[i].size = 0;
                free_blocks_ += table_files_[i].size / BLOCK_SIZE;
            }
            int blocks_count = data.size() / BLOCK_SIZE;
            if (free_blocks_ > blocks_count)
                throw NotEnoughtSpaceException();
            int row = 0, l = 0;
            for (l = 0; l < BLOCK_COUNT; ++l)
            {
                if (row == blocks_count)
                    break;
                if (blocks_[l].isFree)
                {
                    row++;
                }
                else
                    row = 0;
            }
            int block_to_write = 0;
            if (row != blocks_count)
            {
                block_to_write = optimaze_();
            }
            else
                block_to_write = l - row + 1;

            for (int j = 0; j < blocks_count; ++j)
            {
                blocks_[block_to_write + j].data = data.substr(j * BLOCK_SIZE, BLOCK_SIZE);
                blocks_[block_to_write + j].isFree = false;
            }
            table_files_[i].firstBlockIndex = block_to_write;
            table_files_[i].size = data.size();
            free_blocks_ -= blocks_count;
        }
    }
}

void FileSystem::freeBlocks_(int block_index, int blocks_count)
{
    for (int i = block_index; i < block_index + blocks_count; ++i)
    {
        blocks_[i].isFree = true;
        blocks_[i].data = std::string(BLOCK_SIZE, ' ');
    }
}

int FileSystem::optimaze_(){
    //через массив с парами индексов

}