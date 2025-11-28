#include "../includes/PassCommand.hpp"

PassCommand::PassCommand(){}
PassCommand::~PassCommand(){}

void PassCommand::execute(Server& server, Client& client, const std::vector<std::string>& args){
	if (client.isRegistered()){
		server.sendReply(client, "462 " + client.getNickname() + " :You may not reregister");
		return;
	}

	if (args.empty()){
		server.sendReply(client, "461 PASS :Not enough parameters");
		return;
	}

	if (args[0] == server.getPassword())
		client.setAuthenticated(true);  // Si le Client a le bon mdp autorise la connection au serveur et donc les autres commandes
	else
		server.sendReply(client, "464 " + client.getNickname() + " :Password incorrect");
}