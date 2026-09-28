#include "include/filesystem.hpp"
#include "include/exceptions.hpp"
#include <cmath>

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
            int blocks_count = (data.size() + BLOCK_SIZE - 1) / BLOCK_SIZE;
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

void FileSystem::AppendToFile(const std::string &name, const std::string &data)
{
    int i;
    for (i = 0; i < table_files_.size(); ++i)
    {
        if (table_files_[i].name == name)
            break;
    }
    if (i == table_files_.size())
    {
        throw FileNotFoundException();
    }

    int required_blocks = std::ceil((table_files_[i].size + data.size()) / (double)BLOCK_SIZE);
    int cur_blocks = std::ceil((table_files_[i].size / (double)BLOCK_SIZE));
    if (required_blocks == cur_blocks)
    {
        Block *current_block = &blocks_[table_files_[i].firstBlockIndex + cur_blocks];
        int last = current_block->data.find_last_not_of(' ');
        current_block->data.replace(last, data.size(), data);
        table_files_[i].size += data.size();
        return;
    }
    std::string all_data = ReadFromFile(name) + data;
    DeleteFile(name);
    WriteToFile(name, all_data);
}

void FileSystem::freeBlocks_(int block_index, int blocks_count)
{
    for (int i = block_index; i < block_index + blocks_count; ++i)
    {
        blocks_[i].isFree = true;
        blocks_[i].data = std::string(BLOCK_SIZE, ' ');
    }
}

int FileSystem::optimaze_()
{
    std::vector<int> idx_moves;
    int i = 0;
    for (int j = 0; j < BLOCK_COUNT; ++j)
    {
        blocks_[i] = blocks_[j];
        if (!blocks_[j].isFree)
        {
            ++i;
            continue;
        }
        idx_moves.push_back(j);
    }
    for (int k = i; k < BLOCK_COUNT; ++k)
        blocks_[k] = Block(BLOCK_SIZE);

    for (int j = 0; j < table_files_.size(); ++j)
    {
        int k = 0;
        for (; k < idx_moves.size(); ++k)
        {
            if (idx_moves[k] > table_files_[j].firstBlockIndex)
                break;
        }
        table_files_[j].firstBlockIndex = table_files_[j].firstBlockIndex - k;
    }

    return i + 1;
}

void FileSystem::CopyFile(std::string new_name, const std::string &old_name)
{
    if (!IsFileExists(old_name))
        throw FileNotFoundException();
    if (new_name == old_name)
        new_name += "(1)";

    WriteToFile(new_name, ReadFromFile(old_name));
}