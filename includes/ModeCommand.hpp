#pragma once

#include "ICommand.hpp"
#include "Server.hpp"
#include "Client.hpp"

class ModeCommand: public ICommand{
	public:

		ModeCommand();
		virtual ~ModeCommand();

		void execute(Server& server, Client& client, const std::vector<std::string>& args);
};