# ft_irc

![Language](https://img.shields.io/badge/Language-C%2B%2B98-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![School](https://img.shields.io/badge/School-42-000000?style=for-the-badge&logo=42&logoColor=white)
![Status](https://img.shields.io/badge/Status-Completed-success?style=for-the-badge)
![Network](https://img.shields.io/badge/Network-TCP%2FIP-green?style=for-the-badge)
![RFC](https://img.shields.io/badge/Standard-RFC%201459-orange?style=for-the-badge)

## 📝 Description

**ft_irc** is a complete IRC server implemented in C++98, complying with the RFC 1459 standard.
This project marks a shift from classical imperative programming to a strong **Object-Oriented Architecture**. It focuses on maintainability, error handling, and performance via non-blocking I/O (`poll()`).

## 🏗️ Software Architecture

To avoid a complex monolithic structure (the famous "if/else forest"), I structured the project around proven Design Patterns.

The code is organized to maximize clarity and maintainability:

- Each IRC command (e.g., `NICK`, `JOIN`, `PRIVMSG`, `KICK`...) implements the `ICommand` interface (file `includes/ICommand.hpp`).
- The server (`Server` class) maintains an associative map linking a command name to the object implementing it. This avoids long `if/else` chains and makes adding new commands easy.
- A `Logger` (file `includes/Logger.hpp`) provides log levels (`DEBUG`, `INFO`, `CMD`, `ERROR`) and writes error messages to `stderr` while other levels go to `stdout`.

## Why use a Command Map?

Advantages:

- **O(log N)** lookup time for the `std::map` container.
- **Centralized command registration** (easy to initialize in the `Server` constructor).
- **Extensibility:** Adding a command simply means creating a derived class and registering it in the map.

## `ICommand` Interface — Example

Here is a minimal example of what the `ICommand` interface looks like (inspired by `includes/ICommand.hpp`):

```cpp
class ICommand {
public:
    virtual ~ICommand() {}
    // execute acts on the server, the calling client, and the command arguments
    virtual void execute(Server& server, Client& client, const std::vector<std::string>& args) = 0;
};
```

Chaque commande implémente `execute` et utilise uniquement l'API publique du `Server` et du `Client` pour effectuer son travail.

## Registering Commands — Example (in `Server`)

In the server's constructor (or an initialization method), commands are instantiated and placed into the map:

```cpp
// pseudo-snippet from Server::Server(...) constructor
_commands["NICK"] = new NickCommand();
_commands["JOIN"] = new JoinCommand();
_commands["PRIVMSG"] = new PrivmsgCommand();
_commands["QUIT"] = new QuitCommand();
// ... other commands
```

## Logger — Operation and Best Practices

The project logger (file includes/Logger.hpp) defines levels: DEBUG, INFO, CMD, ERROR.

- Messages with the ERROR level are written to std::cerr and include an [ERROR] prefix.
- Other levels are written to std::cout with appropriate prefixes ([INFO], [DEBUG], [CMD]).

Usage examples:

```cpp
Logger::log(INFO, "Server started on port 6667");
Logger::log(CMD, "Execution of JOIN by user 'alice'");
Logger::log(ERROR, "Failed to listen on socket: address already in use");
```

## Build & Exécution

1. Compile :

```bash
make
```

2. Run the binary :

```bash
./ircserv <port> <password>
```


3. Connect via irssi :

```bash
irssi <IP_address> <port> <password>
```

4. Connect via netcat :

```bash
nc -C <IP_address> <port>
```

You will then need to manually type the IRC commands, for example:

```
PASS <password>
NICK testuser
USER testuser 0 * :Test User
JOIN #test
PRIVMSG #test :Hello world!
```
## 👥 Authors

This project was built by a duo:

* **COTONEA Melvin** - [Voir le profil GitHub](https://github.com/mcotonea42)
* **MILLIOT Marc** - [Voir le profil GitHub](https://github.com/NaYruk)