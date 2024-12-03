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
string selectChannelStmt = "SELECT * FROM channels WHERE id=?";
string selectAuthorsStmt =
  "SELECT DISTINCT u.id, u.username FROM messages m JOIN users u ON u.id = "
  "m.author_id WHERE m.channel_id =?";
string updateChannelStmt = "UPDATE channels SET name=? WHERE id=?";
string deleteChannelStmt = "DELETE FROM channels WHERE id=?";

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
  int rc = stmt.execute();
  if (rc != SQLITE_ROW) {
    stmt.finish();
    throw invalid_argument("Invalid server id");
  }
  string channelId = stmt.getResultString(0);
  stmt.finish();
  return channelId;
}

bool isValidChannelId(string channelId) {
  if (channelId == "") { return false; }
  Database *db = Database::getInstance();
  Statement stmt = db->newStatement(selectChannelStmt);
  stmt.bind(channelId);
  int rc = stmt.execute();
  bool isValid = rc == SQLITE_ROW;
  stmt.finish();
  return isValid;
}

vector<json> getAuthors(string channelId) {
  Database *db = Database::getInstance();
  Statement stmt = db->newStatement(selectAuthorsStmt);
  stmt.bind(channelId);
  vector<json> authors;
  while (stmt.step() == SQLITE_ROW) {
    authors.push_back(json{{"id", stmt.getResultString(0)}, {"username", stmt.getResultString(1)}});
  }
  stmt.finish();
  return authors;
}

bool editChannel(string channelId, string name, string serverId) {
  // if the channel id is same as general channel, we dont want to allow renaming
  string generalChannelId = getGeneralChannel(serverId);
  if (generalChannelId == channelId) { return false; }
  Database *db = Database::getInstance();
  Statement stmt = db->newStatement(updateChannelStmt);
  stmt.bind(name);
  stmt.bind(channelId);
  stmt.execute();
  stmt.finish();
  return true;
}

bool deleteChannel(string channelId, string serverId) {
  // if the channel id is same as general channel, we dont wanna delete it
  string generalChannelId = getGeneralChannel(serverId);
  if (generalChannelId == channelId) { return false; }
  Database *db = Database::getInstance();
  Statement stmt = db->newStatement(deleteChannelStmt);
  stmt.bind(channelId);
  stmt.execute();
  stmt.finish();
  return true;
}