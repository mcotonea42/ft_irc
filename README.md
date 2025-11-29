# ft_irc

![Language](https://img.shields.io/badge/Language-C%2B%2B98-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![School](https://img.shields.io/badge/School-42-000000?style=for-the-badge&)
![Status](https://img.shields.io/badge/Status-Completed-success?style=for-the-badge)
![Network](https://img.shields.io/badge/Network-TCP%2FIP-green?style=for-the-badge)
![RFC](https://img.shields.io/badge/Standard-RFC%201459-orange?style=for-the-badge)

## 📝 Description

**ft_irc** est un serveur IRC complet implémenté en C++98, respectant le standard RFC 1459.
Ce projet marque une rupture avec la programmation impérative classique pour adopter une **Architecture Orientée Objet** forte. Il met l'accent sur la maintenabilité, la gestion d'erreurs et la performance via des I/O non bloquants (`poll()`).

## 🏗️ Architecture Logicielle

Pour éviter une structure monolithique complexe (le fameux "if/else forest"), j'ai structuré le projet autour de Design Patterns éprouvés.

Le code est organisé pour maximiser la clarté et la maintenabilité :

- Chaque commande IRC (ex. `NICK`, `JOIN`, `PRIVMSG`, `KICK`, ...) implémente l'interface `ICommand` (fichier `includes/ICommand.hpp`).
- Le serveur (classe `Server`) conserve une table associative (map) liant le nom d'une commande à l'objet qui l'implémente. Cela évite des chaînes de `if/else` et permet d'ajouter facilement de nouvelles commandes.
- Un `Logger` (fichier `includes/Logger.hpp`) fournit des niveaux de log (`DEBUG`, `INFO`, `CMD`, `ERROR`) et écrit les messages d'erreur sur `stderr` alors que les autres niveaux vont sur `stdout`.

## Pourquoi une map de commandes ?

Avantages :

- Recherche O(log N) pour le conteneur `std::map`
- Enregistrement centralisé des commandes (facile à initialiser dans le constructeur de `Server`).
- Extensibilité : ajouter une commande revient à créer une classe dérivée et l'enregistrer dans la map.

## Interface `ICommand` — Exemple

Voici un exemple minimal de ce à quoi ressemble l'interface `ICommand` (inspiré de `includes/ICommand.hpp`) :

```cpp
class ICommand {
public:
    virtual ~ICommand() {}
    // execute agit sur le serveur, le client appelant et les arguments de la commande
    virtual void execute(Server& server, Client& client, const std::vector<std::string>& args) = 0;
};
```

Chaque commande implémente `execute` et utilise uniquement l'API publique du `Server` et du `Client` pour effectuer son travail.

## Enregistrer les commandes — Exemple (dans `Server`)

Dans le constructeur (ou une méthode d'initialisation) du serveur, les commandes sont instanciées et placées dans la map :

```cpp
// pseudo-extrait du constructeur Server::Server(...)
_commands["NICK"] = new NickCommand();
_commands["JOIN"] = new JoinCommand();
_commands["PRIVMSG"] = new PrivmsgCommand();
_commands["QUIT"] = new QuitCommand();
// ... autres commandes
```

## Logger — Fonctionnement et bonne pratique

Le logger du projet (fichier `includes/Logger.hpp`) définit des niveaux : `DEBUG`, `INFO`, `CMD`, `ERROR`.

- Les messages de niveau `ERROR` sont écrits sur `std::cerr` et contiennent un préfixe `[ERROR]`.
- Les autres niveaux sont écrits sur `std::cout` avec des préfixes appropriés (`[INFO]`, `[DEBUG]`, `[CMD]`).

Exemples d'utilisation :

```cpp
Logger::log(INFO, "Serveur démarré sur le port 6667");
Logger::log(CMD, "Exécution de JOIN par user 'alice'");
Logger::log(ERROR, "Impossible d'écouter sur le socket : address already in use");
```

## Build & Exécution

1. Compiler :

```bash
make
```

2. Lancer le binaire produit (nom selon `Makefile`) :

```bash
./ircserv <port> <password>
```


3. Se connecter via irssi :

```bash
irssi <IP_address> <port> <password>
```

4. Se connecter via netcat :

```bash
nc -C <IP_address> <port>
```

Il faudra alors taper manuellement les commandes IRC, par exemple :

```
PASS <password>
NICK testuser
USER testuser 0 * :Test User
JOIN #test
PRIVMSG #test :Hello world!
```
## 👥 Auteurs

Ce projet a été réalisé en binôme par :

* **COTONEA Melvin** - [Voir le profil GitHub](https://github.com/mcotonea42)
* **MILLIOT Marc** - [Voir le profil GitHub](https://github.com/NaYruk)