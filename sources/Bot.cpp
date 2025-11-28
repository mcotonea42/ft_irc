#include "../includes/Bot.hpp"
#include "../includes/Server.hpp"

Bot::Bot() : Client(), _botConnected(false), _lastTimeSend(0) {
    _funnyBotMsg.push_back("Les poissons aussi ont des serveurs IRC. On appelle ça des carpes-à-puces.");
    _funnyBotMsg.push_back("J'ai tenté de séduire un bot Discord. Elle m'a ghosté.");
    _funnyBotMsg.push_back("Si je lag, c'est émotionnel.");
    _funnyBotMsg.push_back("Les 3 C de la vie : Compiler, Crash, Café.");
    _funnyBotMsg.push_back("J'ai mis un printf dans ma vie pour voir où j'en suis.");
    _funnyBotMsg.push_back("Un jour j'aurai un return 0; heureux.");
    _funnyBotMsg.push_back("Je fais des malloc d'amour et des free de tristesse.");
    _funnyBotMsg.push_back("Vous êtes tous beaux dans vos struct respectives.");
    _funnyBotMsg.push_back("Je me suis réveillé du mauvais thread.");
    _funnyBotMsg.push_back("Mon âme est en UTF-8.");
    _funnyBotMsg.push_back("J'ai demandé un raise, on m'a répondu Segmentation fault.");
    _funnyBotMsg.push_back("Ma vie est un infinite loop sans break.");
    _funnyBotMsg.push_back("Je rêve d'un monde sans warnings.");
    _funnyBotMsg.push_back("Mon animal totem est un pointeur nul.");
    _funnyBotMsg.push_back("J'ai voulu delete mes erreurs, mais elles n'étaient pas allouées dynamiquement.");
    _funnyBotMsg.push_back("Je fais des backups de mes émotions, au cas où.");
    _funnyBotMsg.push_back("J'ai tenté un push sur la vie, mais j'ai oublié le commit.");
    _funnyBotMsg.push_back("La réalité est en retard sur le serveur de production.");
    _funnyBotMsg.push_back("Je suis né dans un thread secondaire.");
    _funnyBotMsg.push_back("J'ai forké la réalité, mais le parent et l'enfant se disputent encore.");
}

Bot::~Bot() {}


bool Bot::getBotConnected() const {
    return _botConnected;
}

time_t Bot::getLastTimeSend() const{
    return _lastTimeSend;
}

void    Bot::setLastTimeSend(time_t now) {
    _lastTimeSend = now;
}

void Bot::setupBot(Server & server) {
        _fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_fd < 0) {
        Logger::log(ERROR, "Erreur lors de la creation du socket du BOT");
        throw std::runtime_error("Erreur lors de la création du socket du BOT");
    }

    fcntl(_fd, F_SETFL, O_NONBLOCK);

    sockaddr_in botAddr;
    botAddr.sin_family = AF_INET;
    botAddr.sin_port = htons(server.getPort());
    botAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(_fd, (struct sockaddr*)&botAddr, sizeof(botAddr)) < 0) {
        if (errno != EINPROGRESS) {
            Logger::log(ERROR, "Erreur lors de la connexion du BOT au serveur");
            throw std::runtime_error("Erreur lors de la connexion du BOT au serveur");
        }
    }

    _botConnected = true; // EVITE la repetition de la creation du BOT

    // Creation du BOT
    std::string pass = "PASS " + server.getPassword() + "\r\n";
    std::string nick = "NICK BOT\r\n";
    std::string user = "USER BOT 0 * :BOT_BOT\r\n";

    send(_fd, pass.c_str(), pass.size(),0);
    send(_fd, nick.c_str(), nick.size(), 0);
    send(_fd, user.c_str(), user.size(), 0);

    _lastTimeSend = time(NULL);
}


void    Bot::printBotMsg(Server & server) {
    srand(time(NULL)); // Utiliser le temps actuel comme seed pour avoir des messages aleatoires

    // Recuperer le vrai client Bot depuis _clients (celui qui a ete enregistre avec NICK/USER)
    Client* botClient = server.getClientByNick("BOT");
    if (!botClient) {
        return; // Le bot n'est pas encore enregistre
    }

    for (std::map<std::string, Channel>::iterator it = server.getChannels().begin(); it != server.getChannels().end(); it++) {
        // Verifier que le bot est membre du channel avant d'envoyer un message
        if (it->second.getUserConnectedNbr() > 1 && it->second.isClientInChannel(botClient->getFd())) {
            ssize_t randNbr = rand() % _funnyBotMsg.size();
            std::string msg = _funnyBotMsg[randNbr];

            // Format IRC: :BOT!BOT@serveur PRIVMSG #channel :message
            std::string prefix = ":" + botClient->getNickname() + "!" + botClient->getUsername() + "@" + server.getServerName();
            std::string fullMessage = prefix + " PRIVMSG " + it->second.getName() + " :" + msg;

            it->second.broadcast(server, fullMessage);
        }
    }
}