// comment this file out later

#include <iostream>
#include <fstream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib") 
using namespace std;

const int PORT = 8080;
const int BUFFER_SIZE = 1024;

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main() {

    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (iResult != 0) {
        cerr << "WSAStartup failed: " << iResult << endl;
        return 1;
    }

    SOCKET sockfd = socket(AF_INET, SOCK_STREAM, 0); // opens socket
    if (sockfd == INVALID_SOCKET) {
        cerr << "ERROR opening socket" << endl;
        WSACleanup();
        return 1;
    }

    struct sockaddr_in serv_addr;
    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("10.0.0.129");
    serv_addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        cerr << "ERROR on binding" << endl;
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    if (listen(sockfd, 5) == SOCKET_ERROR) {
        cerr << "ERROR on listen" << endl;
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    struct sockaddr_in cli_addr;
    int clilen = sizeof(cli_addr);
    char buffer[BUFFER_SIZE];

    ofstream logFile("remote_log.txt", ios::app);
    if (!logFile.is_open()) {
        cerr << "Unable to open log file!" << endl;
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    while (true) {
        SOCKET newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd == INVALID_SOCKET) {
            cerr << "ERROR on accept" << endl;
            continue;
        }

        memset(buffer, 0, BUFFER_SIZE);
        int n = recv(newsockfd, buffer, BUFFER_SIZE - 1, 0);
        if (n == SOCKET_ERROR) {
            cerr << "ERROR reading from socket" << endl;
        } else {
            logFile << buffer << endl;
        }

        closesocket(newsockfd);
    }

    closesocket(sockfd);
    WSACleanup();
    return 0;
}