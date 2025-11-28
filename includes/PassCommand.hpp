#pragma once

#include "ICommand.hpp"
#include "Server.hpp"
#include "Client.hpp"

class PassCommand: public ICommand{
	public:

		PassCommand();
		virtual ~PassCommand();

		void execute(Server& server, Client& client, const std::vector<std::string>& args);
};