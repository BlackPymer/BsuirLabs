#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string>
#include <cstring>
#include <vector>
#include <chrono>
#include <fcntl.h>
#include <signal.h>
struct Process
{
    pid_t pid;
    std::chrono::steady_clock::time_point start_time;
};
std::vector<pid_t> child_pids;
void signalHandler(int)
{
    for (pid_t pid : child_pids)
    {
        kill(pid, SIGTERM);
    }
    exit(0);
}
void createProcess(const std::vector<char *> &argv_args, Process &p)
{
    pid_t pid = fork();
    if (pid == 0)
    {
        nice(5);
        std::cout << "The process with PID " << getpid() << " has been created.\n";
        /*
        int devnull = open("/dev/null", O_RDWR);
        dup2(devnull, STDIN_FILENO);
        dup2(devnull, STDOUT_FILENO);
        */
        execvp(argv_args[0], argv_args.data());
        perror("execvp failed");
        exit(1);
    }
    else if (pid == -1)
    {
        p.pid = -1;
    }
    else
    {
        p.pid = pid;
        p.start_time = std::chrono::steady_clock::now();
        child_pids.push_back(pid);
    }
}
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

    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGHUP, signalHandler);

    std::vector<Process> processes;
    for (int i = 0; i < number; i++)
    {
        Process p;
        createProcess(argv_args, p);
        if (p.pid == -1)
        {
            return 1;
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
                createProcess(argv_args, p);
                if (p.pid == -1)
                {
                    return 1;
                }
            }
            if (std::chrono::steady_clock::now() - p.start_time > std::chrono::seconds(seconds))
            {
                kill(p.pid, SIGTERM);
                std::cout << "The process with PID " << p.pid << " has been terminated.\n";
                waitpid(p.pid, NULL, 0);
                createProcess(argv_args, p);
                if (p.pid == -1)
                {
                    return 1;
                }
            }
        }
    }
    return 0;
}
