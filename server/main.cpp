#include <iostream>
#include "Server.hpp"

int main() {
    Server server(POOL_SIZE, PORT);
    if(server.init() == SERVER_ERROR) {
        return 1;   
    }
    std::cout << "SERVER START" << std::endl;
    server.run();
    return 0;
}
