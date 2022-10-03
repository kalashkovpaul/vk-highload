#pragma once

#include <iostream>
#include <vector>
#include <thread>
#include <netinet/in.h>
#include <mutex>
#include <queue>
#include <cstring>
#include <condition_variable>

#include <fstream>
#include <sstream>
#include <arpa/inet.h>
#include <string>
#include <unistd.h>
#include <filesystem>

#include "constants.hpp"
#include "responses.hpp"
#include "utils.hpp"

// Сервер будет работать стандартно:
// инициализация - настройка адреса сервера (port, family...), bind к сокету, listen
// run - закидываем запросы в очередь с использованием связки mutex + cv,
class Server {
public:
    Server() = delete;

    explicit Server(size_t poolSize = POOL_SIZE, int port = PORT, int countConn = CONN_COUNT)
    :poolSize(poolSize), serverPort(port), queueConnections(CONN_COUNT) {
        this->serverSocket = 0, this->clientSocket =  0;
        memset(&this->serverAddress, 0, sizeof(struct sockaddr_in));
        memset(&this->clientAddress, 0, sizeof(struct sockaddr_in));
        this->threadPool.resize(this->poolSize);
        for(size_t i = 0; i < this->poolSize; i++) {
            this->threadPool[i] = std::thread([this](){this->handleRequest();});
        }
    };

    int init();

    void run();

private:
    int serverSocket;
    int clientSocket;
    struct sockaddr_in serverAddress;
    struct sockaddr_in clientAddress;
    int serverPort;

    std::vector<std::thread> threadPool;
    size_t poolSize;
    std::queue<int> requestQueue;

    int queueConnections;

    std::mutex queueMutex;
    std::condition_variable cv;

    void handleRequest();

    int loadConfig();
};