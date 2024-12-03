#ifndef SERVERS_H
#define SERVERS_H

#include "../db.h"
#include "json.hpp"

#include <string>
using json = nlohmann::json;
using namespace std;

string createServer(string adminId, string name);
vector<json> getServers();
json dbResultToJson(Statement stmt);
json getServer(string serverId);
bool isValidServerId(string serverId);

#endif