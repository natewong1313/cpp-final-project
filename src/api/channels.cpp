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

// Given a server id and a server name, create a new channel
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

// Returns an array of channels in json format for a specified server id
// [{"id": "", "serverId": "", "name": ""}]
vector<json> getChannels(string serverId) {
  vector<json> channels;
  Database *db = Database::getInstance();
  Statement stmt = db->newStatement(selectChannelsStmt);
  stmt.bind(serverId);
  while (stmt.step() == SQLITE_ROW) {
    // TODO: abstract this into a helper function?
    channels.push_back(json{{"id", stmt.getResultString(0)},
                            {"serverId", stmt.getResultString(1)},
                            {"name", stmt.getResultString(2)}});
  }
  stmt.finish();
  return channels;
}

// This takes in a serverId and finds a matching
// channel in the server with the name of "general"
// Every server needs to have a general channel
// The general channel is not deletable or editable
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

// Validates whether a channel exists given a channelId.
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

// Returns all authors in a channel, meaning people who have sent a message
// This is used on the frontend to assign message ids to usernames
// Returns an array: [{"id": "", "username": ""}]
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

// Updates a specified channels name given a channel id and server id
// This function will be expanded in the future, but you can only edit the name for now
// Returns false if a user is trying to edit the general channel
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

// Deletes a channel given a channel id
// Returns false if the user is trying to delete the general channel
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