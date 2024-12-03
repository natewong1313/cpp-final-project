#include "servers.h"

#include "../db.h"
#include "../utils.h"
#include "./channels.h"
#include "json.hpp"

#include <string.h>
#include <vector>
using json = nlohmann::json;
using namespace std;
string insertServerStmt = "INSERT INTO servers(id, admin_id, name) VALUES "
                          "(?, ?, ?)";
string selectServersStmt = "SELECT * FROM servers";
string selectServerStmt = "SELECT * FROM servers WHERE id=?";

// Inserts a new server into the database
string createServer(string adminId, string name) {
  string serverId = createId();
  Database *db = Database::getInstance();
  Statement stmt = db->newStatement(insertServerStmt);
  stmt.bind(serverId);
  stmt.bind(adminId);
  stmt.bind(name);
  int rc = stmt.execute();
  stmt.finish();
  if (rc == 19) {
    throw invalid_argument("Server name already exists");
    // string errMsg = sqlite3_errmsg(db->getConnection());
    // if (errMsg == "UNIQUE constraint failed: servers.name") {
    //   throw invalid_argument("Server name already exists");
    // }
  }
  createChannel(serverId, "general");
  return serverId;
}

// Helper function to return a db result as json
// {"id": "", "adminId": "", "name": ""}
json dbResultToJson(Statement stmt) {
  return json{{"id", stmt.getResultString(0)},
              {"adminId", stmt.getResultString(1)},
              {"name", stmt.getResultString(2)}};
}

// Returns all servers as a json array
// see dbResultToJson
vector<json> getServers() {
  vector<json> servers;
  Database *db = Database::getInstance();
  Statement stmt = db->newStatement(selectServersStmt);
  while (stmt.step() == SQLITE_ROW) { servers.push_back(dbResultToJson(stmt)); }
  stmt.finish();
  return servers;
}

// Returns a server as a json obj given a server id
// uses dbResultToJson for return type
json getServer(string serverId) {
  Database *db = Database::getInstance();
  Statement stmt = db->newStatement(selectServerStmt);
  stmt.bind(serverId);
  int rc = stmt.execute();
  if (rc != SQLITE_ROW) {
    stmt.finish();
    throw invalid_argument("Invalid server id");
  }
  json serverData = dbResultToJson(stmt);
  stmt.finish();
  return serverData;
}

// Checks if a serverId is a valid server
bool isValidServerId(string serverId) {
  if (serverId == "") { return false; }
  Database *db = Database::getInstance();
  Statement stmt = db->newStatement(selectServerStmt);
  stmt.bind(serverId);
  int rc = stmt.execute();
  bool isValid = rc == SQLITE_ROW;
  stmt.finish();
  return isValid;
}