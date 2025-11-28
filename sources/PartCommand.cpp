#include "../includes/PartCommand.hpp"

PartCommand::PartCommand(){}

PartCommand::~PartCommand(){}

void PartCommand::execute(Server& server, Client& client, const std::vector<std::string>& args){
	if (args.empty()){
		server.sendReply(client, "461 " + client.getNickname() + " PART :Not enough parameters");
		return;
	}

	const std::string& channelName = args[0];
	Channel* channel = server.getChannel(channelName);

	if (channel == NULL){
		server.sendReply(client, "403 " + client.getNickname() + " " + channelName + " :No such channel");
		return;
	}

	if (!channel->isClientInChannel(client.getFd())){
		server.sendReply(client, "402 " + client.getNickname() + " " + channelName + " :You're not on the channel");
		return;
	}

	std::string partMessage = ":" + client.getNickname() + "!" + client.getUsername() + "@" + server.getServerName() + " PART " + channelName;
	channel->broadcast(server, partMessage);
	channel->removeClient(&client);
}