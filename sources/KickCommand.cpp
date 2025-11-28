#include "../includes/KickCommand.hpp"

KickCommand::KickCommand(){}
KickCommand::~KickCommand(){}

void KickCommand::execute(Server& server, Client& client, const std::vector<std::string>& args){
	if (args.size() < 2){
		server.sendReply(client, "461 " + client.getNickname() + " KICK :Not enough parameters");
		return;
	}

	const std::string& channelName = args[0];
	const std::string& targetNick = args[1];
	Channel* channel = server.getChannel(channelName);

	if (channel == NULL){
		server.sendReply(client, "403 " + client.getNickname() + " " + channelName + " :No such channel");
		return;
	}

	if (!channel->isOperator(client.getFd())){
		server.sendReply(client, "482 " + client.getNickname() + " " + channelName + " :You're not channel operator");
		return;
	}

	Client* targetClient = server.getClientByNick(targetNick);
	if (targetClient == NULL || !channel->isClientInChannel(targetClient->getFd())){
		server.sendReply(client, "441 " + client.getNickname() + " " + targetNick + " " + channelName + " :They aren't on that channel");
		return;
	}

	/*if (targetClient->getNickname() == "BOT"){ A Ajouter si on veux que le Bot ne soit pas Kickable des channels
		return;
	}*/

	std::string reason;
	if (args.size() > 2)
		reason = args[2];
	else
		reason = "Kicked by operator";
	if (!reason.empty() && reason[0] == ':')
		reason = reason.substr(1);

	std::string kickMessage = ":" + client.getNickname() + " KICK " + channelName + " " + targetNick + " :" + reason;
	channel->broadcast(server, kickMessage);
	channel->removeClient(targetClient);
}