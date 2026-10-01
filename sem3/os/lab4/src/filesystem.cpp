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
    int file_index = -1;
    for (size_t i = 0; i < table_files_.size(); ++i)
    {
        if (table_files_[i].name == name)
        {
            file_index = static_cast<int>(i);
            break;
        }
    }

    int old_blocks = 0;
    if (file_index != -1)
        old_blocks = blocksCount_(table_files_[file_index].size);
    int blocks_count = blocksCount_(static_cast<int>(data.size()));

    if (free_blocks_ + old_blocks < blocks_count)
        throw NotEnoughtSpaceException();

    if (old_blocks != 0)
    {
        freeBlocks_(table_files_[file_index].firstBlockIndex, old_blocks);
        free_blocks_ += old_blocks;
    }

    int block_to_write = findFreeBlocks_(blocks_count);

    for (int j = 0; j < blocks_count; ++j)
    {
        std::string chunk = data.substr(j * BLOCK_SIZE, BLOCK_SIZE);
        blocks_[block_to_write + j].data = std::string(BLOCK_SIZE, ' ');
        blocks_[block_to_write + j].data.replace(0, chunk.size(), chunk);
        blocks_[block_to_write + j].isFree = false;
    }
    free_blocks_ -= blocks_count;

    if (file_index == -1)
    {
        File file;
        file.name = name;
        file.size = static_cast<int>(data.size());
        file.firstBlockIndex = block_to_write;
        table_files_.push_back(file);
        return;
    }

    table_files_[file_index].firstBlockIndex = block_to_write;
    table_files_[file_index].size = static_cast<int>(data.size());
}

void FileSystem::AppendToFile(const std::string &name, const std::string &data)
{
    int i;
    for (i = 0; static_cast<size_t>(i) < table_files_.size(); ++i)
    {
        if (table_files_[i].name == name)
            break;
    }
    if (static_cast<size_t>(i) == table_files_.size())
    {
        throw FileNotFoundException();
    }
    if (data.empty())
        return;

    int required_blocks = blocksCount_(table_files_[i].size + static_cast<int>(data.size()));
    int cur_blocks = blocksCount_(table_files_[i].size);
    if (required_blocks == cur_blocks)
    {
        Block &current_block = blocks_[table_files_[i].firstBlockIndex + cur_blocks - 1];
        int offset = table_files_[i].size % BLOCK_SIZE;
        current_block.data.replace(offset, data.size(), data);
        table_files_[i].size += static_cast<int>(data.size());
        return;
    }
    else
    {
        std::string all_data = ReadFromFile(name) + data;
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

int FileSystem::findFreeBlocks_(int blocks_count)
{
    if (blocks_count == 0)
        return 0;

    int run = 0;
    for (int l = 0; l < BLOCK_COUNT; ++l)
    {
        if (!blocks_[l].isFree)
        {
            run = 0;
            continue;
        }
        ++run;
        if (run == blocks_count)
            return l - blocks_count + 1;
    }
    return optimaze_();
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

    for (size_t j = 0; j < table_files_.size(); ++j)
    {
        size_t k = 0;
        for (; k < idx_moves.size(); ++k)
        {
            if (idx_moves[k] > table_files_[j].firstBlockIndex)
                break;
        }
        table_files_[j].firstBlockIndex -= static_cast<int>(k);
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
    for (size_t i = 0; i < table_files_.size(); ++i)
    {
        if (table_files_[i].name != name)
            continue;

        std::string data;
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
    throw FileNotFoundException();
}

bool FileSystem::IsFileExists(const std::string &name)
{
    for (size_t i = 0; i < table_files_.size(); ++i)
    {
        if (table_files_[i].name == name)
            return true;
    }
    return false;
}

void FileSystem::DeleteFile(const std::string &name)
{
    for (size_t i = 0; i < table_files_.size(); ++i)
    {
        if (table_files_[i].name != name)
            continue;

        int freed = blocksCount_(table_files_[i].size);
        freeBlocks_(table_files_[i].firstBlockIndex, freed);
        free_blocks_ += freed;
        table_files_.erase(table_files_.begin() + i);
        return;
    }
}

std::string FileSystem::Dump()
{
    std::string result;
    for (size_t i = 0; i < table_files_.size(); ++i)
    {
        result += "Name:" + table_files_[i].name + "\tsize: " + std::to_string(table_files_[i].size) + "\n";
        result += "Content:\n";
        int total_blocks = blocksCount_(table_files_[i].size);
        int last_block_size = table_files_[i].size % BLOCK_SIZE;
        for (int j = 0; j < total_blocks; ++j)
        {
            if (j == total_blocks - 1 && last_block_size != 0)
                result += blocks_[table_files_[i].firstBlockIndex + j].data.substr(0, last_block_size);
            else
                result += blocks_[table_files_[i].firstBlockIndex + j].data;
        }
        result += "\n";
    }
    return result;
}
