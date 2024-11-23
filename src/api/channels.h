#ifndef CHANNELS_H
#define CHANNELS_H
#include "json.hpp"

#include <string>
using json = nlohmann::json;
using namespace std;

string createChannel(string serverId, string name);
vector<json> getChannels(string serverId);
string getGeneralChannel(string serverId);
bool isValidChannelId(string channelId);
vector<json> getAuthors(string channelId);

#endif