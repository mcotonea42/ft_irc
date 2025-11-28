#pragma once

#include "Client.hpp"
#include <vector>
#include <string>
#include <ctime>

class Server; // Forward declaration

class Bot : public Client {
    private:
        bool _botConnected;
        time_t _lastTimeSend;
        std::vector<std::string> _funnyBotMsg;

    public:
        Bot();
        ~Bot();

        bool    getBotConnected() const;
        time_t  getLastTimeSend() const;
        void    setLastTimeSend(time_t now);
        void    setupBot(Server & server);
        void    printBotMsg(Server & server);
};