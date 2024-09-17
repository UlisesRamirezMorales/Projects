#include <unistd.h>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <strings.h>

#include <vector>
#include <cstring>
#include <cmath>
#include <unordered_map>
#include <sstream>
#include <fstream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>

// Fireman Function to handle zombie processes
void fireman(int) { while (waitpid(-1, NULL, WNOHANG) > 0) {} }

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

std::vector<std::string> initializeVectorFunction(std::string &stringReceived)
{
    std::string input = stringReceived; 
    std::vector<std::string> stringVec;

    size_t sizeInput =input.size(); 
    for(int i=0; i<sizeInput; i++)
        stringVec.push_back(std::string(1, input[i]));
    return stringVec;
}

std::vector<std::string> lineToVector(const std::string& line) {
    std::vector<std::string> result;
    std::istringstream iss(line);
    std::string token;
    while (iss >> token) {
        result.push_back(token);
    }
    return result;
}

// Entropy algorithm fucnction 
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

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno, clilen;
    struct sockaddr_in serv_addr, cli_addr;
    signal(SIGCHLD, fireman);

    // Check the commandline arguments
    if (argc != 2)
    {
        std::cerr << "Port not provided" << std::endl;
        exit(0);
    }

    // Create the socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "Error opening socket" << std::endl;
        exit(0);
    }

    // Populate the sockaddr_in structure
    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    // Bind the socket with the sockaddr_in structure
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "Error binding" << std::endl;
        exit(0);
    }

    // Set the max number of concurrent connections
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    
    while(true)
    {   
        // Accept a new connection
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);
        if (newsockfd < 0)
        {
            std::cerr << "Error accepting new connections" << std::endl;
            exit(0);
        }
        
        // Making the  server multiprocess
        if (fork() == 0)
        {
            // Reading size of msg from client
            int n, msgSize;
            n = read(newsockfd, &msgSize, sizeof(int));
            if (n < 0)
            {
                std::cerr << "Error reading from socket" << std::endl;
                exit(0);
            }

            char *tempBuffer = new char[msgSize + 1];
            bzero(tempBuffer, msgSize + 1);
            // Reading actual msg from client
            n = read(newsockfd, tempBuffer, msgSize + 1);
            if (n < 0)
            {
                std::cerr << "Error reading from socket" << std::endl;
                exit(0);
            }
            std::string buffer = tempBuffer;
            delete[] tempBuffer;

            // Handeling string from client 
            std::vector<double> bufferVector = mainAlgoFunc(buffer);
            // Serialize the vector into a continuous block of bytes
            // Sending back a vector of doubles
            size_t dataSize = bufferVector.size() * sizeof(double);
            char* serializedData = reinterpret_cast<char*>(bufferVector.data());

            // Send the size of the serialized data
            int msgSizeV = dataSize;
            n = write(newsockfd, &msgSizeV, sizeof(int));
            if (n < 0) 
            {
                std::cerr << "Error writing size to socket" << std::endl;
                exit(0);
            }

            // Send the serialized data
            n = write(newsockfd, serializedData, dataSize);
            if (n < 0) 
            {
                std::cerr << "Error writing data to socket" << std::endl;
                exit(0);
            }
            // Closing new socket and exiting process
            close(newsockfd);
            _exit(0);
        }
        
    }
    // Closing socket
    close(sockfd);
    return 0;
}
