#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string>
#include <cstring>
#include <vector>
#include <chrono>
struct Process
{
    pid_t pid;
    std::chrono::steady_clock::time_point start_time;
};
int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        std::cout << "Usage: " << argv[0] << " number seconds process\n";
        return 1;
    }
    int number = std::atoi(argv[1]);
    int seconds = std::atoi(argv[2]);
    std::string process = argv[3];

    std::vector<char *> argv_args;
    std::vector<char> process_copy(process.begin(), process.end());
    process_copy.push_back('\0');
    char *token = std::strtok(process_copy.data(), " ");
    while (token != NULL)
    {
        argv_args.push_back(token);
        token = std::strtok(NULL, " ");
    }
    argv_args.push_back(NULL);

    std::vector<Process> processes;
    for (int i = 0; i < number; i++)
    {
        Process p;
        pid_t pid = fork();
        if (pid == 0)
        {
            std::cout << "The process with PID " << getpid() << " has been created.\n";
            execvp(argv_args[0], argv_args.data());
            perror("execvp failed");
            exit(1);
        }
        else if (pid == -1)
        {
            return 1;
        }
        else
        {
            p.pid = pid;
            p.start_time = std::chrono::steady_clock::now();
        }
        processes.push_back(p);
    }
    while (true)
    {
        for (Process &p : processes)
        {
            auto res = waitpid(p.pid, NULL, WNOHANG);
            if (res != 0)
            {
                std::cout << "The process with PID " << p.pid << " has been terminated by itself.\n";
                pid_t pid = fork();
                if (pid == 0)
                {
                    std::cout << "The process with PID " << getpid() << " has been created.\n";
                    execvp(argv_args[0], argv_args.data());
                    perror("execvp failed");
                    exit(1);
                }
                else if (pid == -1)
                {
                    return 1;
                }
                else
                {
                    p.pid = pid;
                    p.start_time = std::chrono::steady_clock::now();
                }
            }
            if (std::chrono::steady_clock::now() - p.start_time > std::chrono::seconds(seconds))
            {
                kill(p.pid, SIGTERM);
                std::cout << "The process with PID " << p.pid << " has been terminated.\n";
                waitpid(p.pid, NULL, 0);
                pid_t pid = fork();
                if (pid == 0)
                {
                    std::cout << "The process with PID " << getpid() << " has been created.\n";
                    execvp(argv_args[0], argv_args.data());
                    perror("execvp failed");
                    exit(1);
                }
                else if (pid == -1)
                {
                    return 1;
                }
                else
                {
                    p.pid = pid;
                    p.start_time = std::chrono::steady_clock::now();
                }
            }
        }
    }
    return 0;
}