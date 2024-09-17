#include <iostream>
#include <unordered_map>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <cmath>
#include <iomanip>

struct SharedData {
    std::unordered_map<char, int> vectorOfHashMaps;
    std::vector<double> vectorOfDoubleVectors;
    std::vector<std::string> vectorOfStringVectors;
    int threadIndex;
};

void* threadFunction(void* arg) 
{   
    SharedData* sharedData = static_cast<SharedData*>(arg);
    //SharedData* sharedData = (struct ShareData)*arg;
    int threadIndex = sharedData->threadIndex;
    std::unordered_map<char, int>& vectorOfHashMaps = sharedData->vectorOfHashMaps;
    std::vector<double>& vectorOfDoubleVectors = sharedData->vectorOfDoubleVectors;
    std::vector<std::string>& vectorOfStringVectors = sharedData->vectorOfStringVectors;
    
    /*
    std::cout << std::endl;
    std::cout << "Tread Index: INSIDE THREAD FUNCTION: " << threadIndex << std::endl;
    std::cout << std::endl;*/

    int currFreq = 0;
    double currH = 0;
    char selectedTask;
    int extraFreq=0;
    int sizeVec = vectorOfStringVectors.size();
    /*
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<< std::endl;
    std::cout << "Size of Vec: " << sizeVec << std::endl;
    std::cout << std::endl;
    std::cout << "_________________INTO HUGE LOOP_________________" << std::endl;
    std::cout << std::endl;*/

    int k = 1;
    for (int i = 0; i < sizeVec; i+=2)
    {   
        //std::cout << "-----------------------------------------LOOP NUMBER [" << k << "]" << std::endl;
        int intValue = 0;
        currFreq = 0;
        k+=1;

        
        for (const auto& pair : vectorOfHashMaps)//[threadIndex]) 
        {
        // Extract the integer part from the value
        intValue = pair.second;
        // Add the integer part to the total sum
        currFreq += intValue;
        }
        /*
        std::cout << "currFreq after Looking at Map: "<< currFreq << std::endl;
        std::cout << "Entering currH ,selectedtask and extraFreq" << std::endl;
        std::cout << std::endl;
        std::cout << "currH look up" << std::endl;
        std::cout << std::endl;*/

        if (vectorOfDoubleVectors.empty())//vectorOfDoubleVectors[threadIndex].empty())
        {
            //std::cout << "Inside H Vector is empty" << std::endl;
            //std::cout << "Size of vectorOfDoubleVectors: " << vectorOfDoubleVectors.size() << std::endl;
            currH = 0;
            //std::cout << "currH: " << currH << std::endl;

        }
        else
        {
            //std::cout << "Inside H Vector is not empty" << std::endl;
            currH = vectorOfDoubleVectors.back();
            //std::cout << "currH: " << currH << std::endl;
        }

        /*
        std::cout << "Outside currH assignment" << std::endl;
        std::cout << std::endl;

        std::cout << "MY MAYBE USELESS IF i=0" << std::endl;
        std::cout << std::endl;*/

        if (i == 0) 
        {
            //std::cout << "In selected Task Loop if i=0" << std::endl;

            selectedTask = vectorOfStringVectors[i][0];
            //std::cout << "Outside selected Task" << std::endl;

            extraFreq = std::stoi(vectorOfStringVectors[i+1]);
            //std::cout << "Outside extraFreq" << std::endl;
            //std::cout << std::endl;

        }
        
        else
        {
            //std::cout << "In selected Task Loop if i!=0" << std::endl;
            //std::cout << "In selected Task" << std::endl;
            selectedTask = vectorOfStringVectors[i][0];
            //std::cout << "selectedTask: " << selectedTask << std::endl;



            //std::cout << "Entering extraFreq" << std::endl;
            extraFreq = std::stoi(vectorOfStringVectors[i+1]);
            //std::cout << std::endl;

        }
        /*
        std::cout << "Outside  MAYBE USELESS IF i=0" << std::endl;
        std::cout << std::endl;
        //std::cout << "Outside selectedTask,extraFreq" << std::endl;
        std::cout << std::endl;

        //#############################################
        std::cout << "currH: "<< currH << std::endl;
        std::cout << "selectedTask: "<< selectedTask << std::endl;
        std::cout << "extraFreq: "<< extraFreq << std::endl;*/
    
        
        
        double H;
        double currentTerm;
        double newTerm;
        int freqOfSelectedTask = vectorOfHashMaps.at(selectedTask);

        /*
        std::cout << "~~~~~~~~~~~~~~~~~~" << std::endl;
        std::cout << " ENTROPHY FUNCTION " << std::endl;
        std::cout << "~~~~~~~~~~~~~~~~~~" << std::endl;*/

        int nFreq = currFreq + extraFreq;
        /*
        std::cout << "Nfreq: " << nFreq << std::endl;

        std::cout << "currFreq: " << currFreq << std::endl;
        std::cout << "currH: " << currH << std::endl;
        std::cout << "selectedTask: " << selectedTask << std::endl;
        std::cout << "extraFreq: " << extraFreq << std::endl;
        std::cout << std::endl;*/

        if (nFreq == extraFreq) 
        {
            H = 0;
        }
        else
        {
            if (freqOfSelectedTask == 0)
            {
                currentTerm = 0;
            }
            else 
            {
                currentTerm = freqOfSelectedTask * std::log2(freqOfSelectedTask);
            }
            //std::cout << "THE currentTerm of; " << selectedTask << " : " << currentTerm << std::endl;

            newTerm = (freqOfSelectedTask + extraFreq) * std::log2(freqOfSelectedTask + extraFreq);
            //std::cout << "The new Term: " << newTerm << std::endl;
            
            H = (std::log2(nFreq)) -((std::log2(currFreq)- currH)*(currFreq)- currentTerm + newTerm)/(nFreq);
            //std::cout << "The H calculated: " << H << std::endl;
            //std::cout << std::endl;
        }
        //std::cout << "Outside H Caluculations" << std::endl;
        //std::cout << std::endl;

        //
        vectorOfDoubleVectors.push_back(H);
        //std::cout << "The entrophy value pushed into vector: " << H << std::endl;
        //std::cout << std::endl;
        //
        
        int newHashValue = extraFreq + vectorOfHashMaps.at(selectedTask);
        //std::cout << "Frequency pushed into Hash: " << newHashValue << std::endl;
        //std::cout << "Outside newHashValue" << std::endl;
        //std::cout << std::endl;

        vectorOfHashMaps[selectedTask]=newHashValue;
        //std::cout << "Outside updating hash map" << std::endl;
    
        /*
        const std::unordered_map<char, int>& hashMap = vectorOfHashMaps;
        for (const auto& pair : hashMap) {
            std::cout << "Key: " << pair.first << ", Value: " << pair.second << std::endl;
        }
        std::cout << std::endl;

        std::cout << "NEW H Vector" << std::endl;
        const std::vector<double>& innerVector = vectorOfDoubleVectors;
        for (double value : innerVector) {
            std::cout << value << " ";
        }
        std::cout << std::endl;

        std::cout<<"THE END OF THE THREAD" << std::endl;*/

    }

    return nullptr;
}

// Function to fill a vector of strings from a line of text
std::vector<std::string> lineToVector(const std::string& line) {
    std::vector<std::string> result;
    std::istringstream iss(line);
    std::string token;

    while (iss >> token) {
        result.push_back(token);
    }

    return result;
}
// Fill Vector with the Chars
std::vector<char> stringToCharVector(const std::string& str) 
{
    std::vector<char> charVector;
    for (char ch : str) 
    {
        // Exclude spaces from the vector
        if (ch != ' ') 
        {
            charVector.push_back(ch);
        }
    }
    return charVector;
}

//Display hashMap old code
template <typename KeyType, typename ValueType>
void displayHashMap(const std::unordered_map<KeyType, ValueType>& hashMap) 
{
    for (const auto& pair : hashMap) {
        std::cout << "Key: " << pair.first << ", Value: " << pair.second << std::endl;
    }
}




std::unordered_map<char, int> initializeCharIntHashMap(const std::vector<std::string>& stringVec) 
{
    std::unordered_map<char, int> charIntMap;

    for (size_t i = 0; i < stringVec.size(); i += 2) {
        const std::string& str = stringVec[i];  // Get every other string
        for (char ch : str) {
            charIntMap[ch] = 0;
        }
    }
    return charIntMap;
}

void displayFinalVector(const std::vector<std::string>& vectorOfStringToPrint)
{
    for (int i = 0; i < vectorOfStringToPrint.size(); ++i) 
    {
        if (i % 2 == 1) // Check if it's an odd-indexed string
        {  
            if(i != vectorOfStringToPrint.size()-1)
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
int main() 
{

    std::vector<std::vector<std::string>> inputLines; // Vector to store groups of lines
    std::string inputN;
    while (getline(std::cin, inputN)) {
        if (inputN.empty()) {
            // Blank line encountered, stop reading input for the current group
            break;
        } else {
            // Add the line to the current group

            inputLines.push_back({inputN});
        }
    }
    /*
    while (true) {
        std::getline(std::cin, line);

        if (line.empty()) {
            // Blank line encountered, stop reading input
            break;
        } else {
            // Add the line to the current group
            inputLines.push_back({line});
        }
    }*/

    /* Display the vector of vectors
    for (const auto& group : inputLines) {
        for (const auto& line : group) {
            std::cout << line << std::endl;
        }
        std::cout << "----" << std::endl; // Separate groups
    }*/

    ///std::string inputLine;
    ///std::getline(inputFile, inputLine); // Read a line from the file
    //A 2 B 4 C 3 A 7
    //B 3 A 3 C 3 A 1 B 1 C 1
    // Fill a vector of strings using the function
    std::vector<std::vector<std::string>> indStrings;
    //std::cout << "The sizeOfinputLines: " << inputLines.size() << std::endl;

    indStrings.resize(inputLines.size());


    int sizeOfIndStrings = indStrings.size();

    //std::cout << "The sizeOfIndStrings: " << sizeOfIndStrings << std::endl;

    for(int i =0; i<sizeOfIndStrings; i++)
    {
        indStrings[i] = lineToVector(inputLines[i][0]);
    }

    //std::cout <<std::endl;
    ///std::cout << "Size of Vector: " << charVector.size() << std::endl;
    //std::cout <<std::endl;

    ///inputFile.close(); // Close the file

    ///////////////////////////////////////////////////////////////////////////////////////
    //I have a vector of strings: charVector
    //const int NTHREADS = 1;
    
    
    const int NTHREADS = sizeOfIndStrings;
    //std::cout << "NTHREAD: " << NTHREADS << std::endl;


    std::vector<pthread_t> tid(NTHREADS);


    std::vector<SharedData> sharedData(NTHREADS);

    //std::cout << "Entering vector loops" << std::endl;
    //this is going to be a loop for the size of the vector holding the input strings
    //sharedData[0].vectorOfStringVectors = charVector;
    for(int i =0; i<NTHREADS; i++)
    {
        sharedData[i].vectorOfStringVectors = indStrings[i];
    }
    ///sharedData[0].vectorOfStringVectors = charVector;
    ///sharedData[1].vectorOfStringVectors = {"B","3","A","3","C","3","A","1","B","1","C","1"};
    ///sharedData[2].vectorOfStringVectors = {"I","3","J","10","I","100"};
   
    for (int i = 0; i < NTHREADS; i++) 
    {
        sharedData[i].threadIndex = i;
        sharedData[i].vectorOfHashMaps = initializeCharIntHashMap(sharedData[i].vectorOfStringVectors);
        pthread_create(&tid[i], nullptr, threadFunction, &sharedData[i]);
    }

    for (int i = 0; i < NTHREADS; i++) 
    {
        pthread_join(tid[i], nullptr);
    }
    
    ///////////////////////////////////////////////////////////////////////////////////////
    /*CPU 1
    Task scheduling information: A(2), B(4), C(3), A(7)
    Entropy for CPU 1
    0.00 0.92 1.53 1.42*/
    
    //std::cout << std::endl;
    //std::cout << std::endl;

    for (int i = 0; i < NTHREADS; i++) 
    {
        //sharedData[i].vectorOfDoubleVectors;
        //std::cout << "Vector in Position: " << i << std::endl;
        //std::cout << std::endl;
        std::cout << "CPU " << i+1 << std::endl;
        std::cout << "Task scheduling information: ";
        displayFinalVector(sharedData[i].vectorOfStringVectors);
        //displayHashMap(sharedData[1].vectorOfHashMaps); 
        std::cout << std::endl;
        std::cout << "Entrophy for CPU " << i+1 << std::endl;
        for(int j = 0; j < sharedData[i].vectorOfDoubleVectors.size(); j++)
        {
            std::cout << std::fixed << std::setprecision(2) << sharedData[i].vectorOfDoubleVectors[j]<<  " ";
        }
        std::cout << std::endl;
    
    }

    return 0;
}
