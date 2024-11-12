#ifndef SERVERS_H
#define SERVERS_H

#include "json.hpp"

#include <string>
using json = nlohmann::json;
using namespace std;

struct server {
    string id;
    string adminId;
    string name;
};

void createServer(string adminId, string name);
vector<json> getServers();

#endif