#pragma once

#include "ICommand.hpp"
#include "Server.hpp"
#include "Client.hpp"

class PartCommand: public ICommand{
	public:

		PartCommand();
		virtual ~PartCommand();

		void execute(Server& server, Client& client, const std::vector<std::string>& args);
};