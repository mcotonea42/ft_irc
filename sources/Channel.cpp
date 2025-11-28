#include "../includes/Channel.hpp"
#include "../includes/Server.hpp"
#include <algorithm>

Channel::Channel(const std::string& name): _name(name), _topic(""), _password(""), _userConnectedNbr(0), _userLimit(0), _isInviteOnly(false), _isTopicRestricted(true){}
Channel::~Channel(){}

void Channel::addClient(Client* client){
	_clients[client->getFd()] = client;
	_userConnectedNbr++;
}

void Channel::removeClient(Client *client){
	_clients.erase(client->getFd());
	_userConnectedNbr--;
	std::vector<int>::iterator it = std::find(_operators.begin(), _operators.end(), client->getFd());
	if (it != _operators.end()) {
		_operators.erase(it);	// Supprime le client qui vient de quitter de la liste d'operateurs 
	}
}

void Channel::broadcast(Server& server, const std::string& message){
	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
		server.sendToClient(*it->second, message);
}

const std::string& Channel::getName() const{
	return _name;
}

const std::map<int, Client*>& Channel::getClients() const{
	return this->_clients;
}

const std::string& Channel::getTopic() const{
	return _topic;
}

void Channel::setTopic(const std::string& topic){
	this->_topic = topic;
}

bool Channel::isOperator(int client_fd){
	for (size_t i = 0; i < _operators.size(); ++i)
		if (_operators[i] == client_fd)
			return true;
	return false;
}

void Channel::addOperator(int client_fd){
	if (!isOperator(client_fd))
		_operators.push_back(client_fd);
}

void Channel::removeOperator(int client_fd){
	std::vector<int>::iterator it = std::find(_operators.begin(), _operators.end(), client_fd);
	if (it != _operators.end())
		_operators.erase(it);
}

std::string Channel::getNicknamesList(){
	std::string list;
	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it){
		if (isOperator(it->first)){
			list += "@";
		}
		list += it->second->getNickname() + " ";
	}
	if (!list.empty())
		list.resize(list.size() - 1);
	return list;
}

bool Channel::isClientInChannel(int client_fd){
	return _clients.count(client_fd) > 0;
}

void Channel::broadcastToOthers(Server& server, Client& sender, const std::string& message){
	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it){
		if (it->first != sender.getFd())
			server.relayMessage(*(it->second), message);
	}
}

bool Channel::isInviteOnly() const{
	return _isInviteOnly;
}

void Channel::setInviteOnly(bool status){
	_isInviteOnly = status;
}

void Channel::addInvitedUsers(int userFd){
	if (!isInvited(userFd))
		_invitedUsers.push_back(userFd);
}

bool Channel::isInvited(int userFd) const{
	for (size_t i = 0; i < _invitedUsers.size(); ++i){
		if (_invitedUsers[i] == userFd)
			return true;
	}
	return false;
}

void Channel::removeInvitedUser(int userFd){
	for (std::vector<int>::iterator it = _invitedUsers.begin(); it != _invitedUsers.end(); ++it){
		if (*it == userFd){
			_invitedUsers.erase(it);
			return;
		}
	}
}

void Channel::setPassword(const std::string& password){
	_password = password;
}

const std::string& Channel::getPassword() const{
	return _password;
}

bool Channel::hasPassword() const{
	return !_password.empty();
}

bool Channel::isTopicRestricted() const{
	return _isTopicRestricted;
}

void Channel::setTopicRestricted(bool status){
	_isTopicRestricted = status;
}

void Channel::setUserLimit(int limit){
	if (limit < 0)
		_userLimit = 0;
	else
		_userLimit = limit;
}

bool Channel::isFull() const{
	if (_userLimit <= 0)
		return false;
	if (_clients.size() >= _userLimit)
		return true;
	return false;
}

size_t Channel::getUserConnectedNbr() {
	return this->_userConnectedNbr;
}