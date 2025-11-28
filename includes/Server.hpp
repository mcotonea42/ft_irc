#pragma once

#include "Client.hpp"
#include "ICommand.hpp"
#include "Channel.hpp"
#include "./Macro.hpp"
#include "./Bot.hpp"
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include <cerrno>
#include <stdio.h>
#include <exception>
#include <sstream>
#include <fcntl.h>

class Server{
    private:

        int _port;
        int _listenFd;
        std::string _password;
        std::string _serverName;
        std::map<int, Client*> _clients;
        std::map<std::string, ICommand*> _commands;
        std::vector<pollfd> _fds;
        std::map<std::string, Channel> _channels;

        int acceptNewClient();
        void handleClientData(int fd);
        bool parseAndExecute(Client& client, std::string message);

        Bot _bot;

        
    public:
        
        Server(int port, const std::string& password);
        ~Server();

        static volatile bool g_running;
        static void signalHandler(int signum);

        //BONUS
        Bot&    getBot();
        int     getPort() const;
        void    setNew_clients(int fd);

        // MANDATORY
        void sendReply(const Client& client, const std::string& message);
        void relayMessage(Client& client, const std::string& message);
        void notifyClientQuit(Client& client, const std::string& message);
        void disconnectClient(int fd);
        Client* getClientByNick(const std::string& nick);
        const std::string& getPassword() const;
        const std::string& getServerName() const;

        void sendToClient(Client& client, const std::string& message);
        Channel* getChannel(const std::string& name);
        std::map<std::string, Channel>& getChannels();
        Channel* createChannel(const std::string& name);

        void setup();
        void run();
};