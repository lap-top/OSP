#define SERVER_DELAY 10000
#define EXPECTED_ARGS_5S 2
#include "utils.hpp"
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (argc < EXPECTED_ARGS_5S)
    {
        std::cerr << "ERROR: expected input file name as argument. i.e: ./task5_server input.txt" << std::endl;
        return EXIT_FAILURE;
    }
    std::ifstream dirtyFile((std::string)argv[1]);   // Open input file. EXIT_FAILURE if failed.
    validateInput(&dirtyFile, (std::string)argv[1]); // Check if file opened correctly.

    std::string line;
    std::vector<std::string> strings;

    while (std::getline(dirtyFile, line))
    {
        strings.push_back(line);
    }
    srand(time(NULL));

    // Executes until no more strings in vector
    while (strings.size() > 1)
    {
        usleep(SERVER_DELAY);
        int random = rand() % strings.size();
        std::cout << strings[random] << std::endl;
        std::swap(strings[random], strings.back());
        strings.pop_back();
    }
    // Indicates to client to stop reading.
    std::cout << "\0";
    std::cout << "Finished Writing Strings." << std::endl;
    return EXIT_SUCCESS;
}