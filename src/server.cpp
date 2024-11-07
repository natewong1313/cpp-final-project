#include "server.h"

#include "db.h"
#include "json.hpp"

#include <iostream>
#include <sqlite3.h>
#include <string.h>
#include <vector>
using json = nlohmann::json;

string selectServersStmt = "SELECT * FROM servers";
string createServerStmt = "INSERT INTO servers(admin_id, name) VALUES (?, ?) RETURNING id";

vector<ChatServer> loadChatServersFromDb() {
  vector<ChatServer> servers;
  Database *db = Database::getInstance();
  sqlite3 *dbConn = db->getConnection();
  sqlite3_stmt *stmt;

  int rc = sqlite3_prepare_v2(dbConn, selectServersStmt.c_str(), -1, &stmt, NULL);
  if (rc != SQLITE_OK) {
    cerr << "Error loading servers" << sqlite3_errmsg(dbConn) << endl;
    return servers;
  }

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    ChatServer server = ChatServer(sqlite3_column_int(stmt, 0), sqlite3_column_int(stmt, 1),
                                   sqlite3_column_text(stmt, 2));
    servers.push_back(server);
  };
  rc = sqlite3_finalize(stmt);
  if (rc != SQLITE_OK) { cerr << "Error loading servers" << sqlite3_errmsg(dbConn) << endl; }
  return servers;
}

vector<json> chatServersToJson(vector<ChatServer> chatServers) {
  vector<json> servers;
  for (ChatServer server : chatServers) { servers.push_back(server.toJson()); }
  return servers;
}

ChatServer::ChatServer(int adminId, string name) {
  this->adminId = adminId;
  this->name = name;

  Database *db = Database::getInstance();
  sqlite3 *dbConn = db->getConnection();
  sqlite3_stmt *stmt;

  int rc = sqlite3_prepare_v2(dbConn, createServerStmt.c_str(), -1, &stmt, NULL);
  if (rc != SQLITE_OK) {
    cerr << "Error creating server 1" << sqlite3_errmsg(dbConn) << endl;
    return;
  }
  sqlite3_bind_int(stmt, 1, adminId);
  sqlite3_bind_text(stmt, 2, name.c_str(), strlen(name.c_str()), NULL);

  rc = sqlite3_step(stmt);
  if (rc != SQLITE_ROW) {
    cerr << "Error creating server 2" << sqlite3_errmsg(dbConn) << endl;
    return;
  }

  int serverId = sqlite3_column_int(stmt, 0);
  this->id = serverId;

  rc = sqlite3_finalize(stmt);
  if (rc != SQLITE_OK) {
    cerr << "Error creating server 3" << sqlite3_errmsg(dbConn) << endl;
    return;
  }
  cout << "Server" << endl;
}

ChatServer::ChatServer(int id, int adminId, const unsigned char *name) {
  this->id = id;
  this->adminId = adminId;
  // https://stackoverflow.com/questions/17746688/convert-unsigned-char-to-stdstring
  this->name = reinterpret_cast<const char *>(name);
}

int ChatServer::getId() {
  return id;
}

json ChatServer::toJson() {
  return json{
    {     "id",      id},
    {"adminId", adminId},
    {   "name",    name},
  };
}