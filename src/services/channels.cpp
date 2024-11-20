#include "../db.h"
#include "../utils.h"
#include "json.hpp"

#include <string>
using json = nlohmann::json;
using namespace std;

string insertChannelStmt = "INSERT INTO channels(id, server_id, name) VALUES "
                           "(?, ?, ?)";
string selectChannelsStmt = "SELECT * FROM channels WHERE server_id=?";
string selectGeneralChannelStmt = "SELECT id FROM channels WHERE server_id=? AND name='general'";

string createChannel(string serverId, string name) {
  string channelId = createId();
  Database *db = Database::getInstance();
  Statement stmt = db->newStatement(insertChannelStmt);
  stmt.bind(channelId);
  stmt.bind(serverId);
  stmt.bind(name);
  stmt.execute();
  stmt.finish();
  return channelId;
}

vector<json> getChannels(string serverId) {
  vector<json> channels;
  Database *db = Database::getInstance();
  Statement stmt = db->newStatement(selectChannelsStmt);
  stmt.bind(serverId);
  while (stmt.step() == SQLITE_ROW) {
    channels.push_back(json{{"id", stmt.getResultString(0)},
                            {"serverId", stmt.getResultString(1)},
                            {"name", stmt.getResultString(2)}});
  }
  stmt.finish();
  return channels;
}

string getGeneralChannel(string serverId) {
  Database *db = Database::getInstance();
  Statement stmt = db->newStatement(selectGeneralChannelStmt);
  stmt.bind(serverId);
  stmt.execute();
  string channelId = stmt.getResultString(0);
  stmt.finish();
  return channelId;
}