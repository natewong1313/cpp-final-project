#ifndef MESSAGING_H
#define MESSAGING_H

#include "json.hpp"

#include <string>

using json = nlohmann::json;
using namespace std;

struct message {
    string id;
    string authorId;
    string channelId;
    string content;
    int createdAt;
};

vector<json> getMessages(string channelId);
void broadcastMessage(message msg);
void addMessageToDb(message msg);
json sendMessage(string authorId, string channelId, string content);

#endif