#pragma once

#include "ICommand.hpp"
#include "Server.hpp"
#include "Client.hpp"

class QuitCommand: public ICommand{
	public:

		QuitCommand();
		~QuitCommand();

		void execute(Server& server, Client& client, const std::vector<std::string>& args);
};