#include "Server.hpp"

int Server::init() {
    this->loadConfig(); // загружаем информацию о размере пула, порте и бэклоге
    this->serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Can not create socket\n";
        return SERVER_ERROR;
    }

    this->serverAddress.sin_family = AF_INET;
    this->serverAddress.sin_port = htons(this->serverPort);
    this->serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(this->serverSocket, (struct sockaddr *) &this->serverAddress, sizeof(this->serverAddress)) == -1) {
        std::cerr << "Failed to bind the socket to address " << errno << std::endl;
        return SERVER_ERROR;
    }

    listen(serverSocket, queueConnections);

    return OK;
}

int Server::loadConfig() {
    std::fstream configFile;
    configFile.open("../config", std::ios::in);
    if (!configFile.is_open()) {
        std::cerr << "Warning : Failed to load configuration file\n";
        return FILE_OPEN_ERROR;
    }

    int lineNum = 1;
    std::string line;
    std::string key;
    std::string value;
    while (getline(configFile, line)) {
        std::stringstream ssline(line);
        std::getline(ssline, key, '=');
        std::getline(ssline, value);

        if (key[0] == '#') { // comment
            lineNum++;
            continue;
        } else if (key == "pool_size") {
            try {
                this->poolSize = std::stoi(value);
            } catch (std::exception &e) {
                std::cerr << "Error : Can't set the pool size " << e.what() << "\n";
            }
        } else if (key == "port_number") {
            try {
                this->serverPort = std::stoi(value);
            } catch (std::exception &e) {
                std::cerr << "Error : Can't set the port number " << e.what() << "\n";
            }
        } else if (key == "backlog_size") {
            try {
                this->queueConnections = std::stoi(value);
            } catch (std::exception &e) {
                std::cerr << "Error : Can't set the backlog size " << e.what() << "\n";
            }
        } else {
            std::cerr << "Warning : Invalid configuration key at line " << lineNum << "\n";
        }
        lineNum++;
    }
    configFile.close();

    return OK;
}
