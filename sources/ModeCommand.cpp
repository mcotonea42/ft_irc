#include "../includes/ModeCommand.hpp"

ModeCommand::ModeCommand(){}
ModeCommand::~ModeCommand(){}

void ModeCommand::execute(Server& server, Client& client, const std::vector<std::string>& args){
	if (args.size() < 2){
		server.sendReply(client, "461 " + client.getNickname() + " MODE :Not enough parameters");
		return;
	}

	const std::string& channelName = args[0];
	const std::string& modeString = args[1];

	Channel* channel = server.getChannel(channelName);
	if (channel == NULL){
		server.sendReply(client, "403 " + client.getNickname() + " " + channelName + " :No such channel");
		return;
	}
	if (!channel->isOperator(client.getFd())){
		server.sendReply(client, "482 " + client.getNickname() + " " + channelName + " :You're not channel operator");
		return;
	}

	size_t modeIndex = 2; 
	bool addingMode = true;
	for (size_t i = 0; i < modeString.length(); ++i){
		char mode = modeString[i];

		if (mode == '+'){
			addingMode = true;
			continue;
		}
		else if (mode == '-'){
			addingMode = false;
			continue;
		}
		switch(mode){
			case 'i':
				channel->setInviteOnly(addingMode);
				break;
			case 't':
				channel->setTopicRestricted(addingMode);
				break;
			case 'k':
				if (addingMode){
					if (modeIndex >= args.size()){
						server.sendReply(client, "461 " + client.getNickname() + " MODE +k :Not enough parameters");
						continue;
					}
					channel->setPassword(args[modeIndex]);
					modeIndex++;
				}
				else
					channel->setPassword("");
				break;
			case 'l':
				if (addingMode){
					if (modeIndex >= args.size()){
						server.sendReply(client, "461 " + client.getNickname() + " MODE +l :Not enough parameters");
						continue;
					}
					int limit = std::atoi(args[modeIndex].c_str());
					channel->setUserLimit(limit);
					modeIndex++;
				}
				else
					channel->setUserLimit(0);
				break;
			case 'o':{
				if (modeIndex >= args.size()){
					std::string modeSign;
					if (addingMode)
						modeSign = "+o";
					else
						modeSign = "-o";
					server.sendReply(client, "461 " + client.getNickname() + " MODE " + modeSign + " :Not enough parameters");
                    continue;
				}
				const std::string& targetNick = args[modeIndex];
				Client* targetClient = server.getClientByNick(targetNick);
				if (targetClient == NULL || !channel->isClientInChannel(targetClient->getFd())){
					server.sendReply(client, "401 " + client.getNickname() + " " + targetNick + " :No such nick/channel");
                    modeIndex++;
                    continue;
				}
				if (addingMode)
					channel->addOperator(targetClient->getFd());
				else
					channel->removeOperator(targetClient->getFd());
				modeIndex++;
				break;
				}
			default:
				server.sendReply(client, "472 " + client.getNickname() + " " + mode + " :is unknown mode char to me for " + channelName);
				break;
		}		
	}

	std::string notification = ":" + client.getNickname() + "!" + client.getUsername() + "@" + server.getServerName() + " MODE " + channelName;
	for (size_t i = 1; i < args.size(); ++i)
		notification += " " + args[i];
	channel->broadcast(server, notification);
}