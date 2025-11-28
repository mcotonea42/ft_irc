// #include <iostream>
// #include <vector>
// #include <string>
// #include <sstream>
// #include <cstring>
// #include <cstdlib>
// #include <unistd.h>
// #include <arpa/inet.h>
// #include <poll.h>
// #include <cerrno>
// #include <stdio.h>
#include <signal.h>

#include "../includes/Server.hpp"

void    check_port_validity(char * port) {
    char*   endptr;
    long    port_long = strtol(port, &endptr, 10);
    if (errno == ERANGE || port_long < MIN_PORT || port_long > MAX_PORT) {
        std::cerr << "Error: Port must be a number between 1024 and 65535" << std::endl;
        exit(1);
    }
    return ;
}

int main(int argc, char **argv){
    if (argc != 3){
        std::cerr << "Usage: ./ircserv <port> <password>\n";
        return 1;
    }

    check_port_validity(argv[1]);
    int port = atoi(argv[1]);
    std::string password = argv[2];

    signal(SIGINT, Server::signalHandler);
    signal(SIGTERM, Server::signalHandler);

    try {
        Server server(port, password);
        server.setup();
        server.run();
    }
    catch (const std::exception& e){
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}