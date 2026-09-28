#include "include/filesystem.hpp"
#include "include/exceptions.hpp"

int FileSystem::blocksCount_(int size) const
{
    if (size <= 0)
        return 0;
    return (size + BLOCK_SIZE - 1) / BLOCK_SIZE;
}

FileSystem::~FileSystem() = default;

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
    file.firstBlockIndex = 0;
    table_files_.push_back(file);
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
                int old_blocks = blocksCount_(table_files_[i].size);
                freeBlocks_(table_files_[i].firstBlockIndex, old_blocks);
                table_files_[i].firstBlockIndex = 0;
                table_files_[i].size = 0;
                free_blocks_ += old_blocks;
            }
            int blocks_count = blocksCount_(static_cast<int>(data.size()));
            if (free_blocks_ < blocks_count)
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
                block_to_write = l - row;

            for (int j = 0; j < blocks_count; ++j)
            {
                std::string chunk = data.substr(j * BLOCK_SIZE, BLOCK_SIZE);
                blocks_[block_to_write + j].data = std::string(BLOCK_SIZE, ' ');
                blocks_[block_to_write + j].data.replace(0, chunk.size(), chunk);
                blocks_[block_to_write + j].isFree = false;
            }
            table_files_[i].firstBlockIndex = block_to_write;
            table_files_[i].size = static_cast<int>(data.size());
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

    int required_blocks = blocksCount_(table_files_[i].size + static_cast<int>(data.size()));
    int cur_blocks = blocksCount_(table_files_[i].size);
    if (required_blocks == cur_blocks)
    {
        if (data.empty())
            return;
        Block *current_block = &blocks_[table_files_[i].firstBlockIndex + cur_blocks - 1];
        size_t last = current_block->data.find_last_not_of(' ');
        if (last == std::string::npos)
            last = 0;
        else
            ++last;
        current_block->data.replace(last, data.size(), data);
        table_files_[i].size += static_cast<int>(data.size());
        return;
    }
    else
    {
        std::string all_data = ReadFromFile(name) + data;
        DeleteFile(name);
        WriteToFile(name, all_data);
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

    return i;
}

void FileSystem::CopyFile(std::string new_name, const std::string &old_name)
{
    if (!IsFileExists(old_name))
        throw FileNotFoundException();
    if (new_name == old_name)
        new_name += "(1)";

    WriteToFile(new_name, ReadFromFile(old_name));
}

std::string FileSystem::ReadFromFile(const std::string &name)
{
    int i;
    std::string data;
    for (i = 0; i < table_files_.size(); ++i)
    {
        if (table_files_[i].name == name)
        {
            int total_blocks = blocksCount_(table_files_[i].size);
            int last_block_size = table_files_[i].size % BLOCK_SIZE;
            for (int j = 0; j < total_blocks; ++j)
            {
                if (j == total_blocks - 1 && last_block_size != 0)
                    data.append(blocks_[table_files_[i].firstBlockIndex + j].data.substr(0, last_block_size));
                else
                    data.append(blocks_[table_files_[i].firstBlockIndex + j].data);
            }
            return data;
        }
    }
    if (i == table_files_.size())
        throw FileNotFoundException();
}

bool FileSystem::IsFileExists(const std::string &name)
{
    for (int i = 0; i < table_files_.size(); ++i)
    {
        if (table_files_[i].name == name)
            return true;
    }
    return false;
}

void FileSystem::DeleteFile(const std::string &name)
{
    int i;
    for (i = 0; i < table_files_.size(); ++i)
    {
        if (table_files_[i].name == name)
        {
            int freed = blocksCount_(table_files_[i].size);
            freeBlocks_(table_files_[i].firstBlockIndex, freed);
            free_blocks_ += freed;
            table_files_.erase(table_files_.begin() + i);
            break;
        }
    }
}