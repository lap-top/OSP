#include "task2.hpp"
#include "utils.hpp"
#include <chrono>
// TYPE DEFS
namespace bip = boost::interprocess;
typedef bip::allocator<std::string, bip::managed_shared_memory::segment_manager> ShmemAllocator;
typedef std::vector<std::string, ShmemAllocator> ShVector;
typedef std::map<int, ShVector *> mVec;

// GLOBALS
bip::managed_shared_memory segment;
mVec lengthMap;

// HELPER FUNCTIONS
ShVector *createVector(bip::managed_shared_memory *segment, int vecId)
{
    const ShmemAllocator alloc_inst(segment->get_segment_manager());
    return segment->construct<ShVector>(std::to_string(vecId).c_str())(alloc_inst);
}

void reduce2(std::ofstream &output)
{

    std::map<int, int> index;
    for (auto const &x : lengthMap)
        index[x.first] = 0;

    // merge sort based of lexical compare
    while (lengthMap.size() > 0)
    {
        int smallestMapIndex = -1;
        for (auto const &x : lengthMap)
        {

            // set to the first vector in map
            if (smallestMapIndex < 0)
                smallestMapIndex = x.first;

            if (lexicalCompare(lengthMap[x.first]->at(index[x.first]), lengthMap[smallestMapIndex]->at(index[smallestMapIndex])))
                smallestMapIndex = x.first;
        }

        output << lengthMap[smallestMapIndex]->at(index[smallestMapIndex]) + "\n";
        index[smallestMapIndex]++;

        if (lengthMap[smallestMapIndex]->size() == index[smallestMapIndex])
            lengthMap.erase(smallestMapIndex);
    }
}

void map2(std::ifstream &input, std::ofstream &output, mVec &lengthMap)
{
    std::string line;
    int *mapIndex = (int *)mmap(NULL, PAGESIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    // go through each line and match string to the respective vector (based off length).
    while (std::getline(input, line))
    {
        // fetch all strings and store in shared mem
        if (line.empty() || line.length() > MAX_LENGTH || line.length() < MIN_LENGTH || !isAlpha(line))
            continue;
        // if the vector hasn't been created, create vector in boost in boost
        if (lengthMap.count(line.length()) == 0)
        {
            ShVector *newVector = createVector(&segment, line.length());
            newVector->push_back(line);
            lengthMap[line.length()] = newVector;
        }
        else
        {
            lengthMap[line.length()]->push_back(line);
        }
    }
    // fork and sort
    *mapIndex = MIN_LENGTH;
    int vectorIndex = 0;
    for (const auto &x : lengthMap)
    {
        vectorIndex = x.first;
        if (fork() == 0)
        {
            auto start = std::chrono::high_resolution_clock::now();
            bip::managed_shared_memory segment(bip::open_only, SHARED_NAME);
            ShVector *myvector = segment.find<ShVector>(std::to_string(vectorIndex).c_str()).first;
            ShVector mv = *myvector; // set to local mem (much faster than directly accessing)

            std::sort(mv.begin(), mv.end(), cmpOnThirdChar);
            *myvector = mv; // set the shared mem

            auto finish = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = finish - start;
            std::cout << "Process elapsed time (length " << std::to_string(vectorIndex).c_str() << "): " << elapsed.count() << " s\n";
            exit(EXIT_SUCCESS);
        }
    }

    // Wait until all processes  have finish
    while (wait(NULL) > 0)
        ;

    /// merge sort
    reduce2(output);
}

int main(int argc, char *argv[])
{
    if (argc < EXPECTED_ARGS)
    {
        std::cerr << "ERROR: expected input and output file name as arguments: dirty.txt clean.txt" << std::endl;
        return EXIT_FAILURE;
    }

    std::ifstream dirtyFile((std::string)argv[1]);    // Open input file. EXIT_FAILURE if failed.
    validateInput(&dirtyFile, (std::string)argv[1]);  // Check if file opened correctly.
    std::ofstream cleanFile((std::string)argv[2]);    // Open output. ``
    validateOutput(&cleanFile, (std::string)argv[2]); // Check if file opened correctly.

    // Ensure memory doesn't exsist, from another program execution.
    struct shm_remove
    {
        shm_remove() { bip::shared_memory_object::remove(SHARED_NAME); }
        ~shm_remove() { bip::shared_memory_object::remove(SHARED_NAME); }
    } remove;

    // allocate virtual mem segment
    segment = bip::managed_shared_memory(bip::create_only, SHARED_NAME, SHAREDMEM_SIZE);

    map2(dirtyFile, cleanFile, lengthMap);

    // Clean Up
    cleanFile.close();
    dirtyFile.close();
    bip::shared_memory_object::remove(SHARED_NAME);
}
