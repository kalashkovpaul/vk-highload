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

        if (key[0] == '#') {
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

void Server::run() {
    while (true) {
        socklen_t addr_size = sizeof(this->clientAddress);
        this->clientSocket = accept(serverSocket, (struct sockaddr *) &this->clientAddress, &addr_size);

        char peerIP[INET_ADDRSTRLEN] = {0};
        if (inet_ntop(AF_INET, &this->clientAddress.sin_addr, peerIP, sizeof(peerIP))) {
            // std::cout << "Accepted connection with " << peerIP << "\n";
        } else {
            // std::cout << "Failed to get the IP of the client\n";
            return;
        }

        this->queueMutex.lock();
        // TODO: разобраться, что будет, если переполнимся
        // Ответ: ничего не будет, исключения не бросаются, контейнер по
        // умолчанию (std::deque) умеет делать resize и гарантирует, что не
        // будет бросать исключений: If an exception is thrown
        // (which can be due to Allocator::allocate() or element copy/move
        // constructor/assignment), this function has no effect
        // (strong exception guarantee).
        this->requestQueue.push(clientSocket);
        this->cv.notify_one();
        this->queueMutex.unlock();
        // std::cout << "Pushed request to the queue\n";
    }
}

std::string getResponse(std::stringstream &requestLine, std::string &url, bool isHead) {
    std::string version;
    std::getline(requestLine, version, ' ');
    if (version != HTTP_1_1 && version != HTTP_1_0) {
        // std::cout << version << "\n";
        std::string
            response = VERSION_NOT_SUPPORTED;
        return response;
    }

    url = decodeUrl(url);
    url = StripQueryParams(url);
    url.insert(0, "..");
    bool isDir = false;
    if (std::filesystem::is_directory(url)) {
        url += ROOT_FILE;
        isDir = true;
    }

    std::fstream targetFile;
    targetFile.open(url, std::ios::in);
    if (!targetFile.is_open()) {
        if (isDir) {
            std::cerr << "Failed to load index.html in dir path\n";
            std::string response = FORBIDDEN;
            if (!isHead) {
                response += FORBIDDEN_BODY;
            }
            return response;
        }
        std::cerr << "Failed to load the file\n";
        return notFound();
    }
    auto res = readFile(url);

    std::string response = head();
    auto length = res.size();

    response = response + CONTENT_LENGTH + " " + std::to_string(length);

    if (!isHead) {
        if (std::filesystem::is_regular_file(url)) {
            response += "\r\n" + CONTENT_TYPE + " " + parseMime(url);
        }
        response += "\r\n\r\n" + res;
    } else {
        response += "\r\n\r\n";
    }

    return response;
}

std::string handle(const std::string &request) {
    std::stringstream reqStream(request);
    std::string line;
    std::getline(reqStream, line);

    std::stringstream requestLine(line);
    std::string method;
    std::getline(requestLine, method, ' ');

    std::string url;

    std::getline(requestLine, url, ' ');

    // TODO чтобы только до корня, до него доступ есть, после него - нет
    // Это для локального запуска прокатит, для общего случая нет.
    // По идее нужно бы написать блок с парсингом текущего путя относительно
    // корня, чтобы определение было универсальным, но... таких требований
    // нет, так что... Сойдёт и так, пожалуй
    if (url.find("../../../../../") != std::string::npos) {
        return notFound();
    }
    bool isHead = false;
    if (method != HEAD && method != GET) {
        return notImplemented();
    }
    if (method == HEAD) {
        isHead = true;
    }
    return getResponse(requestLine, url, isHead);
}

void Server::handleRequest() {
    std::unique_lock<std::mutex> lock(this->queueMutex, std::defer_lock);
    int client_socket = -1;

    while (true) {
        lock.lock();
        this->cv.wait(lock, [this]() { return !this->requestQueue.empty(); });
        client_socket = this->requestQueue.front();
        this->requestQueue.pop();
        lock.unlock();

        char req[2 * REQ_SIZE];
        recv(client_socket, req, sizeof(req), 0);
        // std::cout << "Created handler\n";
        std::string reply = handle(req);
        // std::cout << reply << "\n";
        // std::cout << "Client Request : \n" << req << "\n";
        send(client_socket, reply.c_str(), reply.size(), 0);
        close(client_socket);
    }
}

