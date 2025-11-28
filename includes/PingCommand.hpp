#pragma once

#include "ICommand.hpp"
#include "Server.hpp"
#include "Client.hpp"

class PingCommand: public ICommand{
	public:

		PingCommand();
		virtual ~PingCommand();

		void execute(Server& server, Client& client, const std::vector<std::string>& args);
};