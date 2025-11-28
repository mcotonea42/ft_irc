 #pragma once

 #include <string>
 #include <vector>

 class Server;
 class Client;

 class ICommand{
    public:

        virtual ~ICommand(){};
        virtual void execute(Server& server, Client& client, const std::vector<std::string>& args) = 0;
 };