#pragma once

#include "ICommand.hpp"
#include "Server.hpp"
#include "Client.hpp"

class NickCommand: public ICommand{
    public:

        NickCommand();
        virtual ~NickCommand();

        void execute(Server& server, Client& client, const std::vector<std::string>& args);
};