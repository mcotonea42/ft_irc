#include "../includes/NickCommand.hpp"

NickCommand::NickCommand(){}
NickCommand::~NickCommand(){}

bool isValidNick(const std::string& nick) {
    for (size_t i = 0; i < nick.size() ; i++) {
        char c = nick[i];
        if (std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c =='-' || c == '[' || c == ']' || c == '{' || c == '}' || c == '\\' || c == '|') { 
            continue ;
        }
        else
            return false;
    }
    return true;
}

void NickCommand::execute(Server& server, Client& client, const std::vector<std::string>& args){
    if (args.empty()){
        server.sendReply(client, "431 " + client.getNickname() + " :No nickname given");
        return;
    }

    if (args.size() > 1) {
        server.sendReply(client, "461 " + client.getNickname() + " " + " :Too many arguments");
        return ;
    }
    const std::string& newNick = args[0];
    if (newNick.size() > 9) { 
        server.sendReply(client, "432 " + client.getNickname() + " " + newNick + " :Erroneus nickname");
        return ;
    }
    if (isValidNick(newNick) == false) {
        server.sendReply(client, "432 " + client.getNickname() + " " + newNick + " :Erroneus nickname");
        return ;
    }
    const std::string oldNick = client.getNickname();

    if (oldNick == newNick)
        return;

    Client* existingClient = server.getClientByNick(newNick); // Cherche et verifie si le pseudo est pas deja utiliser par un autre client si oui : ERROR
    if (existingClient != NULL && existingClient != &client){
        std::string nickToDisplay;
        if (oldNick.empty())
            nickToDisplay = "*";
        else
            nickToDisplay = oldNick;
        server.sendReply(client, "433 " + nickToDisplay + " " + newNick + " :Nickname is already in use");
        return;
    }
    std::string nickChangeMessage = ":" + oldNick + "!" + client.getUsername() + "@" + server.getServerName() + " NICK :" + newNick;
    std::map<std::string, Channel>& channels = server.getChannels();
    for (std::map<std::string, Channel>::iterator it = channels.begin(); it != channels.end(); ++it){
        Channel& currentChannel = it->second;
        if (currentChannel.isClientInChannel(client.getFd()))
            currentChannel.broadcast(server, nickChangeMessage); // Notifier tout les canaux ou le client est present
    }
    server.sendToClient(client, nickChangeMessage);
    client.setNickname(newNick); // Mise a jour du pseudo du Client

    if (!client.isRegistered() && !client.getUsername().empty()){ // Si le Client n'etait pas encore enregistrer marque le client comme enregistre.
        client.setRegistered(true);
        server.sendReply(client, "001 " + newNick + " :Welcome to my IRC Network, " + newNick);
    }
}