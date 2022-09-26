
#ifndef HEADER_FILE_NAME
#define HEADER_FILE_NAME
#include <sys/wait.h>
#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <fstream>

// CONSTANTS
#define MAX_LENGTH 15
#define MIN_LENGTH 3
#define NUM_THREADS 15
#define EXPECTED_ARGS 3
bool isAlpha(const std::string &str);

bool cmpOnThirdChar(std::string str1, std::string str2);

bool lexicalCompare(std::string str1, std::string str2);

void validateOutput(std::ofstream* file, std::string filename);

void validateInput(std::ifstream* file, std::string filename);

#endif