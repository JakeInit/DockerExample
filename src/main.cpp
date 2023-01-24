#include <chrono>
#include <csignal>
#include <iostream>
#include <thread>

#include <boost/filesystem/operations.hpp>

void signalHandler(int signal);

bool running = true;

int main(int argc, char** argv)
{
    signal(SIGINT, signalHandler);

    std::cout << "The size of " << boost::filesystem::absolute(argv[0]) << " is "
              << boost::filesystem::file_size(argv[0]) << '\n';

    std::cout << "Press 'ctrl + c' to exit\n"
              << std::endl;

    while (running)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    return 0;
}

void signalHandler(int signal)
{
    std::cout << "Signal " << std::to_string(signal)
              << " called. Setting running to false" << std::endl;
    running = false;
}
