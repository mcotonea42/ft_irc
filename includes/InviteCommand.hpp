#pragma once

#include "ICommand.hpp"
#include "Server.hpp"
#include "Client.hpp"

class InviteCommand: public ICommand{
    public:

        InviteCommand();
        ~InviteCommand();

        void execute(Server& server, Client& client, const std::vector<std::string>& args);
};