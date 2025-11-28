#pragma once

#include "ICommand.hpp"
#include "Server.hpp"
#include "Client.hpp"

class KickCommand: public ICommand{
	public:

		KickCommand();
		virtual ~KickCommand();

		void execute(Server& server, Client& client, const std::vector<std::string>& args);
};