#pragma once

#include "ICommand.hpp"
#include "Server.hpp"
#include "Client.hpp"

class TopicCommand: public ICommand{
	public:

		TopicCommand();
		~TopicCommand();

		void execute(Server& server, Client& client, const std::vector<std::string>& args);
};
