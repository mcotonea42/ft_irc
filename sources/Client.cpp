#include "../includes/Client.hpp"

Client::Client(): _fd(-1){}

Client::Client(int fd):
    _fd(fd),
    _nickname(""),
    _username(""),
    _realname(""),
    _isRegistered(false), 
    _isAuthenticated(false){}

Client::~Client(){}

void Client::setNickname(const std::string& nick){
    this->_nickname = nick;
}

void Client::setUsername(const std::string& user){
    this->_username = user;
}

void Client::setRealname(const std::string& realname){
    this->_realname = realname;
}

void Client::setRegistered(bool status){
    this->_isRegistered = status;
}

void Client::setAuthenticated(bool status){
    this->_isAuthenticated = status;
}

int Client::getFd() const{
    return this->_fd;
}

const std::string& Client::getNickname() const{
    return this->_nickname;
}

const std::string& Client::getUsername() const{
    return this->_username;
}

const std::string& Client::getRealname() const{
    return this->_realname;
}

bool Client::isRegistered() const{
    return this->_isRegistered;
}

bool Client::isAuthenticated() const{
    return this->_isAuthenticated;
}

std::string& Client::getBuffer(){
    return this->_buffer;
}

void Client::addToBuffer(const std::string& data){
    this->_buffer += data;
}