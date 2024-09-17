#include <unistd.h>
#include <iostream>
#include <cstring>
#include <strings.h>
#include <string>

#include <pthread.h>
#include <unordered_map>
#include <sstream>
#include <vector>
#include <fstream>
#include <cmath>
#include <iomanip>
#include <algorithm>

struct SharedData
{
    int threadIndex;
    std::string input;
    std::string msgBack;
    std::vector<double> entrophyVector;
    std::vector<std::string> vectorOfStringVectors;


    int *turn;
    pthread_mutex_t *bsem;
    pthread_mutex_t *copy;
    pthread_cond_t *wait;
};

std::vector<std::string> lineToVector(const std::string& line) {
    std::vector<std::string> result;
    std::istringstream iss(line);
    std::string token;
    while (iss >> token) {
        result.push_back(token);
    }

    return result;
}

std::vector<std::string> initializeVectorFunction(std::string &stringReceived)
{
    std::string input = stringReceived; 
    std::vector<std::string> stringVec;

    size_t sizeInput =input.size(); 
    for(int i=0; i<sizeInput; i++)
        stringVec.push_back(std::string(1, input[i]));
    return stringVec;
}

std::unordered_map<char, int> initializeCharIntHashMap(std::vector<std::string>& stringVec) 
{
    std::unordered_map<char, int> charIntMap;

    for (size_t i = 0; i < stringVec.size(); i += 2) {
        const std::string& str = stringVec[i];  // Get every other string
        for (char ch : str) {
            charIntMap[ch] = 0; // initialize to 0
        }
    }
    return charIntMap;
}

std::vector<double> mainAlgoFunc(std::string &buffer)
{
    std::string stringNoSpace = buffer; // string from server
    std::unordered_map<char, int> vectorOfHashMaps;
    std::vector<std::string> vectorOfStringVectors;
    std::vector<double> vectorOfDoubleVectors;

    vectorOfStringVectors = lineToVector(stringNoSpace);
    vectorOfHashMaps = initializeCharIntHashMap(vectorOfStringVectors);

    int currFreq; 
    double currH; 
    char selectedTask;
    int extraFreq;
    int sizeVec = vectorOfStringVectors.size();
    int intValue;

    // Loop to calculate entrophy 
    for (int i = 0; i < sizeVec; i += 2)
    {   
        intValue = 0;
        currFreq = 0;

        // Checking for current frequency out of hash map
        for (const auto& pair : vectorOfHashMaps)
        {
            intValue = pair.second; // Extract the integer part from the value
            currFreq += intValue;   // Add the integer part to the total sum
        }

        // Checking for current entrophy from entrophy Vector
        if (vectorOfDoubleVectors.empty())
            currH = 0;
        else
            currH = vectorOfDoubleVectors.back();

        // Assign selected task and extra frequency from string vector
        selectedTask = vectorOfStringVectors[i][0];
        extraFreq = std::stoi(vectorOfStringVectors[i+1]);
        
        // Remaining Variables for entrophy calculations
        double H;
        double currentTerm;
        double newTerm;
        int freqOfSelectedTask = vectorOfHashMaps.at(selectedTask);
        int nFreq = currFreq + extraFreq;

        // Entrophy determination 
        if (nFreq == extraFreq) 
            H = 0;
        else
        {
            // Current Term determination/calculation a
            if (freqOfSelectedTask == 0)
                currentTerm = 0;
            else 
                currentTerm = freqOfSelectedTask * std::log2(freqOfSelectedTask);

            // New Term and Entrophy Caluculations
            newTerm = (freqOfSelectedTask + extraFreq) * std::log2(freqOfSelectedTask + extraFreq);
            H = (std::log2(nFreq)) -((std::log2(currFreq)- currH)*(currFreq)- currentTerm + newTerm)/(nFreq);
        }

        // Entrophy added to vector and update hash map
        vectorOfDoubleVectors.push_back(H); 
        int newHashValue = extraFreq + vectorOfHashMaps.at(selectedTask);
        vectorOfHashMaps[selectedTask]=newHashValue;
    }
    // Returning Entrophy Vector
    return vectorOfDoubleVectors;
}

void removeSpacesFromVector(std::vector<std::vector<std::string>> &inputLines)
{
    for (std::vector<std::string> &lineGroup : inputLines)
    {
        for (std::string &line : lineGroup)
        {
            // Remove spaces from the current string
            line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
        }
    }
}

void displayFinalVector(const std::vector<std::string>& vectorOfStringToPrint)
{
    for (int i = 0; i < vectorOfStringToPrint.size(); ++i) 
    {
        if (i % 2 == 1) // Check if it's an odd-indexed string
        {  
            if(i != vectorOfStringToPrint.size() - 1)
                std::cout << "(" << vectorOfStringToPrint[i] << "), ";
            else
                std::cout << "(" << vectorOfStringToPrint[i] << ")";
        } 
        else 
        {
            std::cout << vectorOfStringToPrint[i];
        }
    }
}


void *threadFunction(void *arg)
{ 
    //SharedData *sharedData = static_cast<SharedData *>(arg);
    SharedData sharedData = *static_cast<SharedData *>(arg);
    //int index = sharedData->threadIndex;
    int index = sharedData.threadIndex;
    std::string input = sharedData.input;

    pthread_mutex_unlock(sharedData.copy);
    
    pthread_mutex_lock(sharedData.bsem); 

    while (*sharedData.turn != index)
        pthread_cond_wait(sharedData.wait, sharedData.bsem);

    pthread_mutex_unlock(sharedData.bsem);


    std::vector<double> entrophyVector = mainAlgoFunc(input);

    // From here is all new code
    std::vector<std::string>vectorOfStringVectors;

    vectorOfStringVectors = lineToVector(sharedData.input);
    /*
    for (int i = 0; i < NTHREADS; i++)
        vectorOfStringVectors = lineToVector(sharedData.input);
    */    

    
    std::cout << "CPU " << index+1 << std::endl;
    std::cout << "Task scheduling information: ";
    displayFinalVector(vectorOfStringVectors);
    std::cout << std::endl;
    std::cout << "Entropy for CPU " << index+1 << std::endl;

    for(int j = 0; j < entrophyVector.size(); j++)
            std::cout << std::fixed << std::setprecision(2) << entrophyVector[j]<<  " ";
        std::cout << std::endl;
        std::cout << std::endl;
    /*
    if (i != NTHREADS - 1)
    {
        for(int j = 0; j < entrophyVector.size(); j++)
            std::cout << std::fixed << std::setprecision(2) << entrophyVector[j]<<  " ";
        std::cout << std::endl;
        std::cout << std::endl;
    }
    else
    {
        for(int j = 0; j < entrophyVector.size(); j++)
        std::cout << std::fixed << std::setprecision(2) << entrophyVector[j]<<  " ";
    }
    */





/*
    for (int i = 0; i < NTHREADS; i++) 
    {
        std::cout << "CPU " << i+1 << std::endl;
        std::cout << "Task scheduling information: ";
        displayFinalVector(sharedData[i].vectorOfStringVectors);
        std::cout << std::endl;
        std::cout << "Entropy for CPU " << i+1 << std::endl;
        if (i != NTHREADS - 1)
        {
            for(int j = 0; j < entrophyVector.size(); j++)
                std::cout << std::fixed << std::setprecision(2) << entrophyVector[j]<<  " ";
            std::cout << std::endl;
            std::cout << std::endl;
        }
        else
        {
            for(int j = 0; j < entrophyVector.size(); j++)
            std::cout << std::fixed << std::setprecision(2) << entrophyVector[j]<<  " ";
        }

    }
*/
    
    
    /*
    // This works
    for(int j = 0; j < entrophyVector.size(); j++)
        std::cout << std::fixed << std::setprecision(2) << entrophyVector[j]<<  " ";
    std::cout << std::endl;
    */

    pthread_mutex_lock(sharedData.bsem);
    (*sharedData.turn)++;
    pthread_cond_broadcast(sharedData.wait);
    pthread_mutex_unlock(sharedData.bsem);

    
    return nullptr;
}

int main(int argc, char *argv[])
{
    //const char *hostName = argv[1];
    //const char *portNum = argv[2];

    pthread_mutex_t bsem;
    pthread_mutex_t copy;
    pthread_cond_t wait = PTHREAD_COND_INITIALIZER;

    pthread_mutex_init(&bsem, nullptr);
    pthread_mutex_init(&copy, nullptr);
    static int turn;
    turn = 0;


    // Managing input
    
    std::vector<std::vector<std::string>> inputLines; // Vector to store groups of lines
    //Should be able to chage this to just one vector of strings
    
    std::string inputN;                               // variable used for loops
    while (getline(std::cin, inputN))
    {
        if (inputN.empty())
            break; // Blank line encountered, stop reading input for the current group
        else
            inputLines.push_back({inputN}); // Add the line to the current group
    }

    // Handeling the input strings
    int numLines = inputLines.size();
    const int NTHREADS = numLines; // Based on number of input lines

    //pthread_mutex_t bsem;
    //pthread_mutex_init(&bsem,nullptr)

    std::vector<pthread_t> tid(NTHREADS);
    SharedData structToSend;
    structToSend.turn = &turn;
    structToSend.bsem = &bsem;
    structToSend.wait = &wait;
    structToSend.copy = &copy;

    // Pthread Create loop
    for (int i = 0; i < NTHREADS; i++)
    {
        pthread_mutex_lock(&copy);
        /*
        structToSend[i].input = inputLines[i][0];
        structToSend[i].threadIndex = i; // Set the thread index
        */

        structToSend.input = inputLines[i][0];
        //structToSend.input = inputLines[i];
        structToSend.threadIndex = i;
        pthread_create(&tid[i], nullptr, threadFunction, &structToSend);// pthread_create
    }

    // pthread_join loop
    for (int i = 0; i < NTHREADS; i++)
        pthread_join(tid[i], nullptr);

/*
    // Displaying the Information
    for (int i = 0; i < NTHREADS; i++)
        sharedData[i].vectorOfStringVectors = lineToVector(sharedData[i].input);
        
    for (int i = 0; i < NTHREADS; i++) 
    {
        std::cout << "CPU " << i+1 << std::endl;
        std::cout << "Task scheduling information: ";
        displayFinalVector(sharedData[i].vectorOfStringVectors);
        std::cout << std::endl;
        std::cout << "Entropy for CPU " << i+1 << std::endl;
        if (i != NTHREADS - 1)
        {
            for(int j = 0; j < sharedData[i].entrophyVector.size(); j++)
                std::cout << std::fixed << std::setprecision(2) << sharedData[i].entrophyVector[j]<<  " ";
            std::cout << std::endl;
            std::cout << std::endl;
        }
        else
        {
            for(int j = 0; j < sharedData[i].entrophyVector.size(); j++)
            std::cout << std::fixed << std::setprecision(2) << sharedData[i].entrophyVector[j]<<  " ";
        }

    }
*/
    return 0;
}