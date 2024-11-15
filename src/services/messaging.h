#ifndef MESSAGING_H
#define MESSAGING_H

#include "json.hpp"

#include <string>

using json = nlohmann::json;
using namespace std;

struct message {
    string id;
    string authorId;
    string serverId;
    string content;
};

vector<json> getMessages(string serverId);
void broadcastMessage(message msg);
void addMessageToDb(message msg);
void sendMessage(string authorId, string serverId, string content);

#endif