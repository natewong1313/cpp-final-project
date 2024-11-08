#ifndef SERVERS_H
#define SERVERS_H

#include <string>
using namespace std;

struct server {
    string id;
    string adminId;
    string name;
};

void createServer(string adminId, string name);

#endif