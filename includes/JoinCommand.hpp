#pragma once

#include "ICommand.hpp"
#include "Client.hpp"
#include "Server.hpp"

class JoinCommand: public ICommand{
	public:

		JoinCommand();
		virtual ~JoinCommand();

		void execute(Server& server, Client& client, const std::vector<std::string>& args);
};