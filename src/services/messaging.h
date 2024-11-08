#ifndef MESSAGING_H
#define MESSAGING_H

#include <string>
using namespace std;

void broadcastMessage(string messageId, int authorId, int serverId, string content);
void addMessageToDb(string messageId, int authorId, int serverId, string content);
void sendMessage(int authorId, int serverId, string content);

#endif