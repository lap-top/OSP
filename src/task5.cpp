#include <iterator>
#include "utils.hpp"
#include <queue>
#include <unistd.h>
#define MESSAGE_DELAY 1000000
std::vector<std::string> globalVec;
bool serverComplete = false; // flag for if the server has exhausted all strings.
std::map<int, int> counter;

// This is a runner function to read cin and store them into a map of queues.
void *inputReader(void *arg)
{
    std::map<int, std::queue<std::string *>> *buffer = (std::map<int, std::queue<std::string *>> *)arg;
    while (true)
    {

        std::string line;
        std::getline(std::cin, line);

        if (line[0] == '\0')
        {
            break;
        }

        if (line.length() >= MIN_LENGTH && line.length() <= MAX_LENGTH)
        {
            std::string *temp = new std::string(line);
            buffer->at(temp->length()).push(temp);
        }
    }
    serverComplete = true;
    std::cout << "Server has no more strings" << std::endl;
    return NULL;
}

void *reduce5(void *arg)
{
    std::queue<std::string *> *que = (std::queue<std::string *> *)arg;
    while (true)
    {
        if (!que->empty())
        {
            counter[que->front()->length()]++;
            que->pop();
        }
    }
    return NULL;
}

// Checks if the all queues in the map are empty, used as final condition for program.
bool isMapEmpty(std::map<int, std::queue<std::string *>> *buffer)
{
    for (auto const &x : *buffer)
    {
        if (!buffer->at(x.first).empty())
        {
            return false;
        }
    }
    return true;
}

void *counterOutput(void *arg)
{
    while (true)
    {
        usleep(MESSAGE_DELAY);
        std::cout << "Counter Summary" << std::endl;
        for (auto const &x : counter)
        {
            std::cout << "Length[" << x.first << "] count:" << x.second << std::endl;
        }
        std::cout << "________________________" << std::endl;
    }
    return NULL;
}

void map5()
{
    // Create Buffer
    std::map<int, std::queue<std::string *>> buffer;
    std::map<int, int> counter;
    for (int i = MIN_LENGTH; i <= MAX_LENGTH; i++)
    { // allocate dynamically
        std::queue<std::string *> temp;
        buffer[i] = temp;
        counter[i] = 0;
    }
    // Create client listener thread to fill buffer created above
    pthread_t inputThread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&inputThread, &attr, inputReader, (void *)&buffer);

    pthread_t summaryThread;
    pthread_create(&summaryThread, &attr, counterOutput, NULL);
    // launch threads
    pthread_t tid[NUM_THREADS];
    for (auto const &x : buffer)
    {
        pthread_attr_init(&attr); /* attr = Thread Attributes */
        pthread_create(&tid[x.first], &attr, reduce5, (void *)&buffer[x.first]);
    }

    while (true)
    {
        usleep(10000);
        if (isMapEmpty(&buffer) && serverComplete)
        {
            break;
        }
    }
    std::cout << "No more strings to compute, Server exhausted all strings" << std::endl;
}

int main(int argc, char *argv[])
{
    map5();
    return EXIT_SUCCESS;
}