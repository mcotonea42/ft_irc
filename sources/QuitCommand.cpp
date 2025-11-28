#include "../includes/QuitCommand.hpp"

QuitCommand::QuitCommand(){}
QuitCommand::~QuitCommand(){}

void QuitCommand::execute(Server& server, Client& client, const std::vector<std::string>& args){
	std::string reason;
	if (args.size() > 2)
		reason = args[2];
	else
		reason = "Client Quit";
	
	std::string quitMessage = ":" + client.getNickname() + "!" + client.getUsername() + "@" + server.getServerName() + " QUIT :" + reason;
	server.notifyClientQuit(client, quitMessage);
	server.disconnectClient(client.getFd());
}