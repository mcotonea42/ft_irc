#include "../includes/JoinCommand.hpp"

JoinCommand::JoinCommand(){}
JoinCommand::~JoinCommand(){}

void JoinCommand::execute(Server& server, Client& client, const std::vector<std::string>& args){
	if (!client.isRegistered()){
		server.sendReply(client, "451 " + client.getNickname() + " :You have not registered");
		return;
	}

	if (args.empty()){
		server.sendReply(client, "461 " + client.getNickname() + " JOIN :Not enough parameters");
		return;
	}

	const std::string& channelName = args[0];

	if (channelName.empty() || channelName[0] != '#'){
		server.sendReply(client, "403 " + client.getNickname() + " " + channelName + " :No such channel");
		return;
	}

	Channel* channel = server.getChannel(channelName);
	if (channel == NULL){
		channel = server.createChannel(channelName);
		channel->addOperator(server.getBot().getFd());
		channel->addOperator(client.getFd());
	}
	else{
		if (channel->isFull()){
			server.sendReply(client, "471 " + client.getNickname() + " " + channelName + " :Cannot join channel (+l) - Channel is full");
			return;
		}
		if (channel->hasPassword()){
			if (args.size() < 2 || channel->getPassword() != args[1]){
				server.sendReply(client, "475 " + client.getNickname() + " " + channelName + " :Cannot join channel (+k) - Bad channel key");
				return;
			}
		}
		if (channel->isInviteOnly()){
			if (!channel->isInvited(client.getFd())){
				server.sendReply(client, "473 " + client.getNickname() + " " + channelName + " :Cannot join channel (+i) - You must be invited");
				return;
			}
		}
	}

	channel->addClient(&client);

	if (channel->isInvited(client.getFd()))
		channel->removeInvitedUser(client.getFd());

	std::string user_prefix = ":" + client.getNickname() + "!" + client.getUsername() + "@" + server.getServerName();
	std::string joinMsg = user_prefix + " JOIN :" + channelName;

	channel->broadcast(server, joinMsg);
	server.sendReply(client, "332 " + client.getNickname() + " " + channelName + " :" + channel->getTopic());
	server.sendReply(client, "353 " + client.getNickname() + " = " + channelName + " :" + channel->getNicknamesList());
	server.sendReply(client, "366 " + client.getNickname() + " " + channelName + " :End of /NAMES list.");
}