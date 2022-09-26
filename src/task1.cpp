#include "utils.hpp"

void TaskFilter1(std::ifstream &input, std::ofstream &output)
{
    std::string line;
    // Loop until EOF. output each line which passes filter into selected output file.
    std::cout << "Reading Input file" << std::endl;
    while (std::getline(input, line))
    {
        if (line.empty() || line.length() > MAX_LENGTH || line.length() < MIN_LENGTH || !isAlpha(line))
            continue;
        output << line << std::endl;
    }
    std::cout << "Output Complete" << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc < EXPECTED_ARGS)
    {
        std::cerr << "ERROR: expected input and output file name as arguments i.e: ./task1 dirty.txt clean.txt" << std::endl;
        return EXIT_FAILURE;
    }

    std::ifstream dirtyFile((std::string)argv[1]);    // Open input file. EXIT_FAILURE if failed.
    validateInput(&dirtyFile, (std::string)argv[1]);  // Check if file opened correctly.
    std::ofstream cleanFile((std::string)argv[2]);    // Open output. ``
    validateOutput(&cleanFile, (std::string)argv[2]); // Check if file opened correctly.

    TaskFilter1(dirtyFile, cleanFile);

    // Close files.
    cleanFile.close();
    dirtyFile.close();
    return EXIT_SUCCESS;
}
