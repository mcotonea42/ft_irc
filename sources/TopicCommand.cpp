#include "../includes/TopicCommand.hpp"

TopicCommand::TopicCommand(){}
TopicCommand::~TopicCommand(){}

void TopicCommand::execute(Server& server, Client& client, const std::vector<std::string>& args){
	if (args.empty()){
		server.sendReply(client, "461 " + client.getNickname() + " TOPIC :Not enough parameters");
		return;
	}

	const std::string& channelName = args[0];
	Channel* channel = server.getChannel(channelName);

	if (!channel){
		server.sendReply(client, "403 " + client.getNickname() + " " + channelName + " :No such channel");
		return;
	}

	if (!channel->isClientInChannel(client.getFd())){
		server.sendReply(client, "442 " + client.getNickname() + " " + channelName + " :You're not on that channel");
		return;
	}

	if (args.size() == 1){
		const std::string& topic = channel->getTopic();
		if (topic.empty()){
			server.sendReply(client, "331 " + client.getNickname() + " " + channelName + " :No topic is set");
			return;
		}
		else
			server.sendReply(client, "332 " + client.getNickname() + " " + channelName + " :" + topic);
	}
	else{
		if (channel->isTopicRestricted() && !channel->isOperator(client.getFd())){
			server.sendReply(client, "482 " + client.getNickname() + " " + channelName + " :You're not channel operator");
			return;
		}

		std::string newTopic;
		for (size_t i = 1; i < args.size(); ++i){
			newTopic += args[i];
			if (i < args.size() - 1)
				newTopic += " ";
		}

		if (!newTopic.empty() && newTopic[0] == ':')
			newTopic.erase(0, 1);
		channel->setTopic(newTopic);
		std::string topicChangeMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@" + server.getServerName() + " TOPIC " + channelName + " :" + newTopic;
		channel->broadcast(server, topicChangeMsg);
	}
}