#include "include/filesystem.hpp"
#include "include/exceptions.hpp"
#include <iostream>

int main()
{
    int get;
    FileSystem filesystem;
    while (true)
    {
        std::cout << "Выберите операцию: \n";
        std::cout << "1. Создать файл\n";
        std::cout << "2. Записать данные в файл\n";
        std::cout << "3. Добавить данные в файл\n";
        std::cout << "4. Считать данные из файла\n";
        std::cout << "5. Проверить наличие файла\n";
        std::cout << "6. Удалить файл\n";
        std::cout << "7. Копировать файл\n";
        std::cout << "8. Вывести информацию о файлах\n";
        std::cout << "9. Выход\n";
        std::cin >> get;

        if (get == 1)
        {
            std::string name;
            std::cout << "Введите имя файла: \n";
            std::cin >> name;
            try
            {
                filesystem.CreateFile(name);
                std::cout << "Файл создан\n";
            }
            catch (const FileAlreadyExistsException &e)
            {
                std::cout << "Файл с таким именем уже существует\n";
            }
            catch (const std::exception &e)
            {
                std::cout << "Ошибка: " << e.what() << "\n";
            }
        }
        else if (get == 2)
        {
            std::string name;
            std::string data;
            std::cout << "Введите имя файла: \n";
            std::cin >> name;
            std::cout << "Введите информацию для записи: \n";
            std::cin >> data;
            try
            {
                filesystem.WriteToFile(name, data);
                std::cout << "Информация записана\n";
            }
            catch (const NotEnoughtSpaceException &e)
            {
                std::cout << "Недостаточно свободного места\n";
            }
            catch (const std::exception &e)
            {
                std::cout << "Ошибка: " << e.what() << "\n";
            }
        }
        else if (get == 3)
        {
            std::string name;
            std::string data;
            std::cout << "Введите имя файла: \n";
            std::cin >> name;
            std::cout << "Введите информацию для добавления: \n";
            std::cin >> data;
            try
            {
                filesystem.AppendToFile(name, data);
                std::cout << "Информация добавлена\n";
            }
            catch (const FileNotFoundException &e)
            {
                std::cout << "Такого файла не существует\n";
            }
            catch (const NotEnoughtSpaceException &e)
            {
                std::cout << "Недостаточно свободного места\n";
            }
            catch (const std::exception &e)
            {
                std::cout << "Ошибка: " << e.what() << "\n";
            }
        }
        else if (get == 4)
        {
            std::string name;
            std::cout << "Введите имя файла: \n";
            std::cin >> name;
            try
            {
                std::cout << filesystem.ReadFromFile(name) << '\n';
            }
            catch (const FileNotFoundException &e)
            {
                std::cout << "Такого файла не существует\n";
            }
            catch (const std::exception &e)
            {
                std::cout << "Ошибка: " << e.what() << "\n";
            }
        }
        else if (get == 5)
        {
            std::string name;
            std::cout << "Введите имя файла: \n";
            std::cin >> name;
            if (filesystem.IsFileExists(name))
                std::cout << "Файл существует\n";
            else
                std::cout << "Файл не существует\n";
        }
        else if (get == 6)
        {
            std::string name;
            std::cout << "Введите имя файла: \n";
            std::cin >> name;
            try
            {
                filesystem.DeleteFile(name);
                std::cout << "Файл удален\n";
            }
            catch (const std::exception &e)
            {
                std::cout << "Ошибка: " << e.what() << "\n";
            }
        }
        else if (get == 7)
        {
            std::string new_name;
            std::string old_name;
            std::cout << "Введите имя нового файла: \n";
            std::cin >> new_name;
            std::cout << "Введите имя исходного файла: \n";
            std::cin >> old_name;
            try
            {
                filesystem.CopyFile(new_name, old_name);
                std::cout << "Файл успешно скопирован\n";
            }
            catch (const FileNotFoundException &e)
            {
                std::cout << "Такого файла не существует\n";
            }
            catch (const NotEnoughtSpaceException &e)
            {
                std::cout << "Недостаточно свободного места\n";
            }
            catch (const std::exception &e)
            {
                std::cout << "Ошибка: " << e.what() << "\n";
            }
        }
        else if (get == 8)
        {
            std::cout << filesystem.Dump() << "\n";
        }
        else if (get == 9)
        {
            break;
        }
        std::cout << std::endl;
    }
    return 0;
}
