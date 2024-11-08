#ifndef MESSAGING_H
#define MESSAGING_H

#include <string>
using namespace std;

struct message {
    string id;
    string authorId;
    string serverId;
    string content;
};

void broadcastMessage(message msg);
void addMessageToDb(message msg);
void sendMessage(string authorId, string serverId, string content);

#endif