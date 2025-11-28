#pragma once

#include "ICommand.hpp"
#include "Server.hpp"
#include "Client.hpp"

class PrivmsgCommand: public ICommand{
	public:

		PrivmsgCommand();
		virtual ~PrivmsgCommand();

		void execute(Server& server, Client& client, const std::vector<std::string>& args);
};