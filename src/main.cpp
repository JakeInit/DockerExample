#include <iostream>

#if defined(__x86_64__) && defined(__linux__)
std::string helloString = "linux/amd64";
#elif defined(__arm__) && defined(__linux__)
std::string helloString = "linux/arm64";
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#ifdef _WIN64
std::string helloString = "windows64";
#else
std::string helloString = "windows32";
#endif
#else
std::string helloString = "unknown CPU architecture"
#endif

int main(int argc, char** argv)
{
    std::cout << "Hello From Docker " << helloString << std::endl;
    return 0;
}
