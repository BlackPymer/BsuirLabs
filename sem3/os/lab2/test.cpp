#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <random>
int main()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(10, 40);
    int random_number = dis(gen);
    std::cout << "*****The process " << getpid() << " is going to wait for " << random_number << " seconds before terminating" << std::endl;
    sleep(random_number);
    std::cout << "*****This process " << getpid() << " has terminated" << std::endl;
    return 0;
}