#include "../includes/InviteCommand.hpp"

InviteCommand::InviteCommand(){}
InviteCommand::~InviteCommand(){}

void InviteCommand::execute(Server& server, Client& client, const std::vector<std::string>& args){
    if (args.size() < 2){
        server.sendReply(client, "461 " + client.getNickname() + " INVITE :Not enough parameters");
        return;
    }

    const std::string& targetNick = args[0];
    const std::string& channelName = args[1];

    Channel* channel = server.getChannel(channelName);
    
    if (channel == NULL){
        server.sendReply(client, "403 " + client.getNickname() + " " + channelName + " :No such channel");
        return;
    }
    
    if (!channel->isOperator(client.getFd())) {
        server.sendReply(client, "482" + client.getNickname() + " " + channelName + " :You're not channel operator");
        return;
    }
    

    Client* targetClient = server.getClientByNick(targetNick);
    if (targetClient == NULL){
        server.sendReply(client, "401 " + client.getNickname() + " " + targetNick + " :No such nick/channel");
        return;
    }

    if (channel->isClientInChannel(targetClient->getFd())){
        server.sendReply(client, "433 " + client.getNickname() + " " + targetNick + " " + channelName + " :is already on channel");
        return;
    }

    channel->addInvitedUsers(targetClient->getFd());
    server.sendReply(client, "341 " + client.getNickname() + " " + targetNick + " " + channelName);

    std::string inviteMessage = ":" + client.getNickname() + "!" + client.getUsername() + "@" + server.getServerName() + " INVITE " + targetNick + " :" + channelName;
    server.sendToClient(*targetClient, inviteMessage);
}