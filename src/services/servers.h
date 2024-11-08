#ifndef SERVERS_H
#define SERVERS_H

#include <string>
using namespace std;

void addServerToDb(string serverId, string adminId, string name);
void createServer(string adminId, string name);

#endif