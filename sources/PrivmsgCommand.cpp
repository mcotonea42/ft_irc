#include "../includes/PrivmsgCommand.hpp"

PrivmsgCommand::PrivmsgCommand(){}

PrivmsgCommand::~PrivmsgCommand(){}

void PrivmsgCommand::execute(Server& server, Client& client, const std::vector<std::string>& args){
	if (!client.isRegistered()){
		server.sendReply(client, "451 " + client.getNickname() + " :You have not register");
		return;
	}

	if (args.size() < 2){
		if (args.empty())
			server.sendReply(client, "411 " + client.getNickname() + " :No recipient given (PRIVMSG)");
		else
			server.sendReply(client, "412 " + client.getNickname() + " :No test to send");
		return;
	}

	const std::string& targetName = args[0];
	std::string message = "";

	for (size_t i = 1; i < args.size(); ++i){
		message += args[i];

		if (i < args.size() - 1)
		message += " ";
	}

	std::string prefix = ":" + client.getNickname() + "!" + client.getUsername() + "@" + server.getServerName();

	if (targetName[0] == '#'){
		Channel* channel = server.getChannel(targetName);

		if (channel == NULL){
			server.sendReply(client, "403 " + client.getNickname() + " " + targetName + " :No such channel");
			return;
		}
		if (!channel->isClientInChannel(client.getFd())){
			server.sendReply(client, "404 " + client.getNickname() + " " + targetName + " :Cannot send to channel");
			return;
		}

		std::string fullMessage = prefix + " PRIVMSG " + targetName + " :" + message;
		channel->broadcastToOthers(server, client, fullMessage);
	}
	else{
		Client* targetClient = server.getClientByNick(targetName);
		if (targetClient == NULL){
			server.sendReply(client, "401 " + client.getNickname() + " " + targetName + " :No such nick/channel");
			return;
		}

		std::string fullMessage = prefix + " PRIVMSG " + targetName + " :" + message;
		server.relayMessage(*targetClient, fullMessage);
	}
}
