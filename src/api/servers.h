#ifndef SERVERS_H
#define SERVERS_H

#include "../db.h"
#include "json.hpp"

#include <string>
using json = nlohmann::json;
using namespace std;

// returns server id.
string createServer(string adminId, string name);
// returns an array of servers in json format
// [{"id", "adminId", "name"}]
vector<json> getServers();
// internal helper function
json dbResultToJson(Statement stmt);
// returns a single server object in json format
json getServer(string serverId);
bool isValidServerId(string serverId);

#endif