#pragma once

#include "Client.hpp"
#include <string>
#include <vector>
#include <map>

class Server;

class Channel{
	private:

		std::string _name;
		std::string _topic;
		std::string _password;
		size_t _userConnectedNbr;
		size_t _userLimit;
		std::map<int, Client*> _clients;
		std::vector<int> _operators;
		std::vector<int> _invitedUsers;

		bool _isInviteOnly;
		bool _isTopicRestricted;

	public:

		Channel(const std::string& name);
		~Channel();

		void addClient(Client* client);
		void removeClient(Client* client);
		void broadcast(Server& server, const std::string& message);
		void broadcastToOthers(Server& server, Client& sender, const std::string& message);

		const std::string& getName() const;
		const std::map<int, Client*>& getClients() const;
		const std::string& getTopic() const;
		void setTopic(const std::string& topic);
		std::string getNicknamesList();

		bool isOperator(int client_fd);
		void addOperator(int client_fd);
		void removeOperator(int client_fd);

		bool isClientInChannel(int client_fd);

		bool isInviteOnly() const;
		void setInviteOnly(bool status);

		void addInvitedUsers(int userFd);
		bool isInvited(int userFd) const;
		void removeInvitedUser(int userFd);

		void setPassword(const std::string& password);
		const std::string& getPassword() const;
		bool hasPassword() const;

		bool isTopicRestricted() const;
		void setTopicRestricted(bool status);

		void setUserLimit(int limit);
		bool isFull() const;

		size_t getUserConnectedNbr();
};