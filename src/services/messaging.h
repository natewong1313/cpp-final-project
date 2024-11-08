#ifndef MESSAGING_H
#define MESSAGING_H

#include <string>
using namespace std;

void broadcastMessage(int authorId, int serverId, int messageId, string content);
bool sendMessage(int authorId, int serverId, string content);

#endif