#pragma once

#include "ICommand.hpp"
#include "Server.hpp"
#include "Client.hpp"

class UserCommand: public ICommand{
    public:

        UserCommand();
        virtual ~UserCommand();

        void execute(Server& server, Client& client, const std::vector<std::string>& args);
};