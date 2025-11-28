#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/NickCommand.hpp"
#include "../includes/UserCommand.hpp"
#include "../includes/PassCommand.hpp"
#include "../includes/PingCommand.hpp"
#include "../includes/ModeCommand.hpp"
#include "../includes/JoinCommand.hpp"
#include "../includes/PartCommand.hpp"
#include "../includes/PrivmsgCommand.hpp"
#include "../includes/KickCommand.hpp"
#include "../includes/QuitCommand.hpp"
#include "../includes/TopicCommand.hpp"
#include "../includes/InviteCommand.hpp"
#include "../includes/Channel.hpp"
#include "../includes/Logger.hpp"

volatile bool Server::g_running = true;

Server::Server(int port, const std::string& password): _port(port), _listenFd(-1), _password(password), _serverName("my_server_irc"), _bot(Bot()) {
    try {
        _commands["NICK"] = new NickCommand();
        _commands["USER"] = new UserCommand();
        _commands["PASS"] = new PassCommand();
        _commands["PING"] = new PingCommand();
        _commands["MODE"] = new ModeCommand();
        _commands["JOIN"] = new JoinCommand();
        _commands["PRIVMSG"] = new PrivmsgCommand();
        _commands["PART"] = new PartCommand();
        _commands["KICK"] = new KickCommand();
        _commands["QUIT"] = new QuitCommand();
        _commands["TOPIC"] = new TopicCommand();
        _commands["INVITE"] = new InviteCommand();
    }
    catch (std::bad_alloc & e) {
        for(std::map<std::string, ICommand*>::iterator it = _commands.begin(); it != _commands.end(); it++) {
            delete it->second;
        }
        _commands.clear();
        throw;
    }
}

Server::~Server(){
    std::map<std::string, ICommand*>::iterator ite = _commands.begin();
    for (; ite != _commands.end(); ++ite)
        delete ite->second;
    _commands.clear();

    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it){
        close (it->first);
        delete it->second;
    }
    _clients.clear();

    if (_listenFd != -1)
        close (_listenFd);
    
    if (_bot.getBotConnected())
        close (_bot.getFd());
}

void Server::sendReply(const Client& client, const std::string& message){
    std::string fullMessage = ":" + _serverName + " " + message + "\r\n";

    ssize_t bytes = send(client.getFd(), fullMessage.c_str(), fullMessage.length(), 0);
    if (bytes < 0){
        if (errno == EAGAIN || errno == EWOULDBLOCK) { // ERREUR pour le mode non bloquant, signifie que lorsque qu'il n'y a rien a lire, on return directement et on attend pas.
            return ;
        }
        std::ostringstream msg;
        msg << "Erreur sur send() vers le client " << client.getNickname() << "(fd: " << client.getFd() << ")";
        Logger::log(ERROR, msg.str());
    }
    else{
        std::ostringstream msg;
        msg << "S--> [" << client.getNickname() << "]" << message;
        Logger::log(DEBUG, msg.str());
    }
}

void Server::relayMessage(Client& client, const std::string& message){
    std::string fullMessage = message + "\r\n";

    if (send(client.getFd(), fullMessage.c_str(), fullMessage.length(), 0) < 0){
        std::ostringstream msg;
        msg << "Erreur sur send() lors du relai a " << client.getNickname();
        Logger::log(ERROR,msg.str());
    }
    std::ostringstream msg;
    msg << "S--> Relaye a [" << client.getNickname() << "]" << message;
    Logger::log(DEBUG, msg.str());
}

void Server::notifyClientQuit(Client& client, const std::string& message){
    int clientFd = client.getFd();

    for (std::map<std::string, Channel>::iterator it = _channels.begin(); it != _channels.end(); ++it){
        Channel& channel = it->second;
        if (channel.isClientInChannel(clientFd)){
            channel.broadcastToOthers(*this, client, message);
            channel.removeClient(&client);
        }
    }
}

Client* Server::getClientByNick(const std::string& nick){
    std::map<int, Client*>::iterator it = _clients.begin();
    for (; it != _clients.end(); ++it){
        if (it->second->getNickname() == nick)
            return it->second;
    }
    return NULL;
}

int Server::getPort() const {
    return this->_port;
}

const std::string& Server::getPassword() const{
    return this->_password;
}

void Server::setup(){
    _listenFd = socket(AF_INET, SOCK_STREAM, 0); // AF_INET = IPV4, SOCK_STREAM = TCP
    if (_listenFd < 0){
        Logger::log(ERROR, "Erreur lors de la creation du socket");
        throw std::runtime_error("Erreur lors de la création du socket");
    }

    // FCNTL permet de changer les parametre du FD et le rendre non Bloquant, recv, send ou accept renverrons directement -1 si il n'y a rien a faire pour que le serveur ne bloque pas en attendant une commande
    fcntl(_listenFd, F_SETFL, O_NONBLOCK);

    int opt = 1;
    if (setsockopt(_listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0){ // SOL_SOCKET = ce socket, SO_REUSEADDR = reutiliser une adresse locale
        Logger::log(ERROR, "Erreur de la configuration de SO_REUSEADDR");
    }

    sockaddr_in serverAddr; // Structure pour stocker la data du serveur
    serverAddr.sin_family = AF_INET; // IPV4
    serverAddr.sin_port = htons(_port); // Transforme le port serveur en format reseau
    serverAddr.sin_addr.s_addr = INADDR_ANY; // Peut ecouter sur TOUTES les interfaces reseau disponibles

    if (bind(_listenFd, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0){ // Attache le socket au port
        Logger::log(ERROR, "Erreur lors de la liaison du socket (bind)");
        throw std::runtime_error("Erreur lors de la liaison du socket (bind)");
    }

    if (listen(_listenFd, 10)< 0){ // Mise en ecoute du Socket
        Logger::log(ERROR, "Erreur lors de la mise en écoute (listen)");
        throw std::runtime_error("Erreur lors de la mise en écoute (listen)");
    }
    std::ostringstream msg;
    msg << "Serveur IRC démarré sur le port: " << _port;
    Logger::log(INFO, msg.str());
}

int Server::acceptNewClient(){
    sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);

    int client_fd = accept(_listenFd, (sockaddr*)&clientAddr, &clientLen); // Accepte un client, CREE UN NOUVEAU SOCKET CLIENT et le supprime du backlog
    if (client_fd < 0){
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return 0;
        else {
            Logger::log(ERROR, "Erreur lors de l'acceptation du client");
            return -1;
        }
    }

    fcntl(client_fd, F_SETFL, O_NONBLOCK);
    
    try {
        _clients[client_fd] = new Client(client_fd); // stock dans la map le nouveau client
    }
    catch (const std::bad_alloc& e) {
        Logger::log(ERROR, "Impossible d'allouer le client - new error");
        throw;
    }

    std::ostringstream msg;
    msg << "Nouveau client connecté (fd=" << client_fd << ")";
    Logger::log(INFO, msg.str());
    return client_fd;
}

void Server::disconnectClient(int fd){

    // Recuperer et liberer la memoire du Client
    std::map<int, Client*>::iterator clientIt = _clients.find(fd);
    if (clientIt != _clients.end()){
        std::map<std::string, Channel>::iterator clientChannelIt = _channels.begin();
        while (clientChannelIt != _channels.end()) {
            clientChannelIt->second.removeClient(clientIt->second);
            clientChannelIt++;
        }
        delete clientIt->second; // Liberer la memoire
        _clients.erase(clientIt); // Supprimer de la map
    }

    close (fd);
    // Supprimer de _fds
    for (std::vector<pollfd>::iterator it = _fds.begin(); it != _fds.end(); ++it){
        if (it->fd == fd){
            _fds.erase(it);
            break;
        }
    }

    std::ostringstream msg;
    msg << "Client (fd=" << fd << ") déconnecté.";
    Logger::log(INFO, msg.str());
}

void Server::handleClientData(int fd){
    char buffer[512]; // Limite les messages a 512 bytes max (Norme IRC)

    ssize_t bytes = recv(fd, buffer, sizeof(buffer) - 1, 0); // Lis les donnees sur le socket et les mets dans le buffer
    if (bytes == 0){ // Un client s'est deconnecter
        std::ostringstream msg;
        msg << "Client (fd=" << fd << ") a fermé la connexion.";
        Logger::log(INFO, msg.str());
        disconnectClient(fd);
        return;
    }
    else if (bytes < 0){
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return;
        else{ 
        Logger::log(ERROR, "Erreur recv()");
        disconnectClient(fd);
        return;
        }
    }


    // Lire les donnee, les interprete pour les executer
    Client* client = _clients.at(fd); // at : permet de lancer une exception si le [fd] n'existe pas. ATTENTION A SURVEILLER / REVOIR
    client->addToBuffer(std::string(buffer, bytes));

    std::string& clientBuffer = client->getBuffer();
    size_t pos;

    while ((pos = clientBuffer.find("\r\n")) != std::string::npos){
        std::string message = clientBuffer.substr(0, pos);
        clientBuffer.erase(0, pos + 2);

        std::ostringstream msg;
        std::string nick;
        if (client->getNickname().empty()){
            std::ostringstream oss_nick;
            oss_nick << "fd=" << fd;
            nick = oss_nick.str();
        }
        else
            nick = client->getNickname();
        msg << "C<-- [" << nick << "]" << message;
        Logger::log(DEBUG, msg.str());
        bool clientDisconnected = parseAndExecute(*client, message);
        if (clientDisconnected) {
            return; // Le client a ete deconnecte, arreter le traitement
        }
    }
}

static std::string& trimLeft(std::string& s){
    size_t startpos = s.find_first_not_of("\t\r\n");
    if (std::string::npos != startpos)
        s = s.substr(startpos);
    return s;
}

bool Server::parseAndExecute(Client& client, std::string message) {
    trimLeft(message); // Supprime espaces, tabs et \n en debut de ligne
    if (message.empty()) {
        return false;
    }

    std::string commandName;
    size_t space_pos = message.find(' ');
    if (space_pos != std::string::npos) {
        commandName = message.substr(0, space_pos); // Extrait la commande de la string (premier mot)
        message.erase(0, space_pos + 1);
    } else {
        commandName = message;
        message.clear();
    }

    for (std::string::iterator it = commandName.begin(); it != commandName.end(); ++it) {
        *it = ::toupper(static_cast<unsigned char>(*it)); // Mettre la commande en Majuscule pour uniformiser
    }

    if (commandName == "CAP") { // Lister les capacites que le serveur supporte
        if (message.find("LS") != std::string::npos) {
            sendReply(client, "CAP * LS :"); // Indiquer qu'on ne supporte aucune capacite(serveur basique)
        }
        return false;
    }

    std::vector<std::string> args;
    while (!message.empty()) { // Separe tout les Arguments de la string ex: "john 0 * :John Doe" -> args = ["john", "0", "*", "John Doe"]
        trimLeft(message);
        if (message[0] == ':') {
            args.push_back(message.substr(1));
            break;
        }
        space_pos = message.find(' ');
        if (space_pos != std::string::npos) {
            args.push_back(message.substr(0, space_pos));
            message.erase(0, space_pos + 1);
        } else {
            args.push_back(message);
            message.clear();
        }
    }

    if (_commands.count(commandName)) { //Cherche une cle valide qui correspond au nom de la commande.
        std::ostringstream msg;
        std::string nick;
        if (client.getNickname().empty()){
            std::ostringstream oss_nick;
            oss_nick << "fd=" << client.getFd();
            nick = oss_nick.str();
        }
        else
            nick = client.getNickname(); // Recupere le Nom du Client
        msg << "Exécution de " << commandName << " par '" << nick << "'";
        Logger::log(CMD, msg.str());
        int clientFd = client.getFd(); // Sauvegarder le fd avant l'execution
        _commands.at(commandName)->execute(*this, client, args); // Executer la commande
        // Verifier si le client a ete deconnecte (ex: commande QUIT)
        if (_clients.find(clientFd) == _clients.end()) {
            return true; // Client deconnecte
        }
    } else { // Commande Inconnue !
        std::ostringstream msg;
        msg << "Commande inconnue '" << commandName << "' par '" << client.getNickname() << "'";
        Logger::log(INFO, msg.str());
        sendReply(client, "421 " + client.getNickname() + " " + commandName + " :Unknown command");
    }
    return false; // Client toujours connecte
}

void Server::setNew_clients(int fd) {
    try {
        _clients[fd] = new Client(fd);
    }
    catch(std::bad_alloc& e) {
        Logger::log(ERROR, "Impossible d'allouer le client - new error");
        throw;
    }
}

Bot& Server::getBot(){
    return _bot;
}

void Server::run(){
    pollfd serverFd; // Struct pour surveiller le fd du socket pour detecter de l'activite
    serverFd.fd = _listenFd;
    serverFd.events = POLLIN; // POLLIN = Donnees disponibles en lecture
    serverFd.revents = 0; // Vide temps qu'aucunes activiter n'est detecte
    _fds.push_back(serverFd); // vector qui contient tout les sockets a surveiller

    while (Server::g_running){
        
        if (_bot.getBotConnected() == false)
            _bot.setupBot(*this);

        int activity = poll(_fds.data(), _fds.size(), 1000); // Timeout de 1 seconde pour permettre au bot d'envoyer des messages periodiquement
        

        if (activity < 0 && Server::g_running){
            Logger::log(ERROR, "Erreur dans poll()");
            throw std::runtime_error("Erreur dans poll()");
        }

        if (_fds[0].revents & POLLIN){ // Si le socket serveur a de l'activite, ajouter le nouveau client
            int fd = acceptNewClient();
            if (fd >= 0){
                if (fd != _bot.getFd()) {
                    pollfd newFd;
                    newFd.fd = fd;
                    newFd.events = POLLIN;
                    newFd.revents = 0;
                    _fds.push_back(newFd);
                }
            }
        }
        for (size_t i = 1; i < _fds.size(); ++i){ // Gestion des clients existants
            int fd = _fds[i].fd;

            if (_fds[i].revents & (POLLIN | POLLHUP | POLLERR)) // POLLIN = Envoie msg, POLLHUP = Deconnection, POLLERR = Erreur deconnection
                handleClientData(fd);

            if (_clients.find(fd) == _clients.end()){
                --i; // Le client a ete supprime, ajuster l'index car _fds a change
            }
        }

        time_t now = time(NULL);
        if (now - _bot.getLastTimeSend() > TIME_FOR_BOT) {
            _bot.printBotMsg(*this);
            _bot.setLastTimeSend(now);
        }
    }
}

Channel* Server::getChannel(const std::string& name){
    std::map<std::string, Channel>::iterator it = _channels.find(name);
    if (it != _channels.end())
        return &(it->second);
    return NULL;
}

std::map<std::string, Channel>& Server::getChannels(){
    return _channels;
}

Channel* Server::createChannel(const std::string& name){
    _channels.insert(std::make_pair(name, Channel(name)));
    Channel* channel = &(_channels.at(name));
    
    // Ajouter le bot au channel lors de sa création
    Client* botClient = getClientByNick("BOT");
    if (botClient) {
        channel->addClient(botClient);
    }
    
    return channel;
}

void Server::sendToClient(Client& client, const std::string& message){
    std::string fullMessage = message + "\r\n";

    ssize_t bytes = send(client.getFd(), fullMessage.c_str(), fullMessage.length(), 0);
    if (bytes < 0){
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return ;
        std::ostringstream msg;
        msg << "Erreur: impossible d'envoyer des données au client (fd: " << client.getFd() << ").";
        Logger::log(DEBUG, msg.str());
    }
    else{
        std::ostringstream msg;
        msg << "S--> [" << client.getNickname() << "] " << fullMessage.substr(0, fullMessage.length() - 2);
        Logger::log(DEBUG, msg.str());
    }
}

const std::string& Server::getServerName() const{
    return _serverName;
}

void Server::signalHandler(int signum){
    (void)signum;
    std::ostringstream msg;
    msg << "Signal " << signum << " reçu. Arrêt du serveur...";
    Logger::log(INFO, msg.str());
    Server::g_running = false;
}
