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

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

struct SharedData
{
    const char *portNum;
    const char *hostName;
    int threadIndex;
    std::string input;
    std::string msgBack;
    std::vector<double> entrophyVector;
    std::vector<std::string> vectorOfStringVectors;
};

void *threadFunction(void *arg)
{ 
    SharedData *sharedData = static_cast<SharedData *>(arg);
    std::string buffer = sharedData->input;

    int n,msgSize = 0;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    int portno = atoi(sharedData->portNum);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "ERROR opening socket" << std::endl;
        exit(0);
    }
    // Host Name
    server = gethostbyname(sharedData->hostName);
    if (server == NULL)
    {
        std::cerr << "ERROR, no such host" << std::endl;
        exit(0);
    }
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "ERROR connecting" << std::endl;
        exit(0);
    }
    
    // Sending input line
    msgSize = sizeof(buffer);
    n = write(sockfd,&msgSize,sizeof(int));
    if (n < 0) 
    {
        std::cerr << "ERROR writing to socket" << std::endl;
        exit(0);
    }
    n = write(sockfd,buffer.c_str(),msgSize);
    if (n < 0) 
    {
        std::cerr << "ERROR writing to socket" << std::endl;
        exit(0);
    }

    // Read the size of the serialized data
    int msgSizeV;
    n = read(sockfd, &msgSizeV, sizeof(int));
    if (n < 0) {
        std::cerr << "ERROR reading size from socket" << std::endl;
        exit(0);
    }

    // Allocate a buffer to store the serialized data
    char* serializedData = new char[msgSizeV];

    // Read the serialized data
    n = read(sockfd, serializedData, msgSizeV);
    if (n < 0) {
        std::cerr << "ERROR reading data from socket" << std::endl;
        exit(0);
    }

    // Deserialize the received data into a vector of doubles
    std::vector<double> receivedVector;
    if (msgSizeV % sizeof(double) == 0) {
        size_t numDoubles = msgSizeV / sizeof(double);
        const double* doubles = reinterpret_cast<const double*>(serializedData);
        receivedVector.assign(doubles, doubles + numDoubles);
    }

    // Clean up the allocated memory
    delete[] serializedData;
    // Assign the recieved vector to the struct
    sharedData->entrophyVector = receivedVector;
    // Close Socket
    close(sockfd);
    return nullptr;
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

std::vector<std::string> lineToVector(const std::string& line) {
    std::vector<std::string> result;
    std::istringstream iss(line);
    std::string token;
    while (iss >> token) {
        result.push_back(token);
    }

    return result;
}

int main(int argc, char *argv[])
{
    const char *hostName = argv[1];
    const char *portNum = argv[2];

    // Managing input
    std::vector<std::vector<std::string>> inputLines; // Vector to store groups of lines
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
    std::vector<pthread_t> tid(NTHREADS);
    std::vector<SharedData> sharedData(NTHREADS);

    // Pthread Create loop
    for (int i = 0; i < NTHREADS; i++)
    {
        sharedData[i].hostName = hostName;
        sharedData[i].portNum = portNum;
        sharedData[i].input = inputLines[i][0];
        sharedData[i].threadIndex = i; // Set the thread index
        pthread_create(&tid[i], nullptr, threadFunction, &sharedData[i]);// pthread_create
    }

    // pthread_join loop
    for (int i = 0; i < NTHREADS; i++)
        pthread_join(tid[i], nullptr);

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
    return 0;
}