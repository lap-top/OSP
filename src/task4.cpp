#include <iterator>
#include "utils.hpp"
#include <chrono>
#include <unistd.h>

// used for setting sched_priority based off performance values from task3
std::map<int, int> threadPriority{
    {3, 99}, {4, 92}, {5, 95}, {6, 97}, {7, 99}, {8, 98}, {9, 96}, {10, 94}, {11, 93}, {12, 91}, {13, 90}, {14, 89}, {15, 88}};

// executes threads in this order.
int threadOrder[] = {7, 8, 9, 6, 5, 10, 11, 12, 4, 13, 14, 15};

std::vector<std::string> globalVec;       // Contains all strings from file
std::map<int, std::vector<int>> indexMap; // Contains the indexes of all strings in globalVec

// Looks up index on global vector and finds
bool cmpOnThirdChar3(int str1, int str2)
{
    return globalVec[str1].substr(2, globalVec[str1].length()) < globalVec[str2].substr(2, globalVec[str2].length());
}

// Thread runner program

void *runner(void *arg)
{

    auto start = std::chrono::high_resolution_clock::now();
    std::vector<int> *vec = (std::vector<int> *)arg;

    std::sort(vec->begin(), vec->end(), cmpOnThirdChar3); // Sort based off third character onwards
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "Thread elapsed time (length " << globalVec[vec->front()].length() << "): " << elapsed.count() << std::endl;
    pthread_exit(vec);
}

void reduce4(std::ofstream &output)
{
    std::map<int, int> index;

    // INITIALIZE INDEX MAPPING TO MOVE THROUGH VECTORS (FILES)
    for (auto const &x : indexMap)
        index[x.first] = 0;

    // LEXICAL SORT
    while (indexMap.size() > 0)
    {
        int lowestIndex = -1;
        // GO THROUGH EACH LINE ON EACH FILE FIND LOWEST LEXICAL
        for (auto const &x : indexMap)
        {
            if (lowestIndex < 0)
                lowestIndex = x.first;

            if (lexicalCompare(globalVec[indexMap[x.first].at(index[x.first])], globalVec[indexMap[lowestIndex].at(index[lowestIndex])]))
                lowestIndex = x.first;
        }

        output << globalVec[indexMap[lowestIndex].at(index[lowestIndex])] + "\n";

        index[lowestIndex]++; // Increment index on vector (file) which was lowest lex
        if (index[lowestIndex] == indexMap[lowestIndex].size())
        { // if the index == length we can erase the vector (file)
            indexMap.erase(lowestIndex);
        }
    }
    std::cout << "Reduce Complete, Thanks :)" << std::endl; 
}
void map4(std::ifstream &input, std::ofstream &output)
{
    // fill vector of strings and index mapping to those strings.
    std::string line;
    int idx = 0;
    while (std::getline(input, line))
    {
        if (line.empty() || line.length() > MAX_LENGTH || line.length() < MIN_LENGTH || !isAlpha(line))
            continue;
        indexMap[line.length()].push_back(idx++);
        globalVec.push_back(line);
    }
    sched_param param[NUM_THREADS];
    pthread_t tid[NUM_THREADS];
    pthread_attr_t attr[NUM_THREADS];

    // LAUNCH THREADS
    auto start = std::chrono::high_resolution_clock::now();
    for (int x : threadOrder)
    {
        // Set up thread priorities
        param[x].sched_priority = threadPriority[x];
        pthread_attr_init(&attr[x]);
        pthread_attr_setschedpolicy(&attr[x], SCHED_RR);
        pthread_attr_setinheritsched(&attr[x], PTHREAD_EXPLICIT_SCHED);
        pthread_attr_setschedparam(&attr[x], &param[x]);
        pthread_attr_setdetachstate(&attr[x], PTHREAD_CREATE_JOINABLE);

        // LAUNCH RUNNER PROGRAM WITH VECTOR OF INDEXES AS REFERENCE
        pthread_create(&tid[x], &attr[x], runner, (void *)&indexMap[x]);
    }

    // WAIT UNTIL ALL THREADS FINISH
    for (auto const &x : indexMap)
        pthread_join(tid[x.first], NULL);

    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "Total running time of threads time: " << elapsed.count() << std::endl;
    // CALL REDUCE FUNCTION
    std::cout << "Executing reduce function" << std::endl; 
    reduce4(output);
}

int main(int argc, char *argv[])
{
    if (argc < EXPECTED_ARGS)
    {
        std::cerr << "ERROR: expected input and output file name as arguments i.e: ./task4 dirty.txt clean.txt" << std::endl;
        return EXIT_FAILURE;
    }

    std::ifstream dirtyFile((std::string)argv[1]);    // Open input file. EXIT_FAILURE if failed.
    validateInput(&dirtyFile, (std::string)argv[1]);  // Check if file opened correctly.
    std::ofstream cleanFile((std::string)argv[2]);    // Open output. ``
    validateOutput(&cleanFile, (std::string)argv[2]); // Check if file opened correctly.

    map4(dirtyFile, cleanFile);

    cleanFile.close();
    dirtyFile.close();
    return EXIT_SUCCESS;
}
