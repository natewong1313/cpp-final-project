#include "servers.h"

#include "../db.h"
#include "../utils.h"
#include "json.hpp"

#include <string.h>
#include <vector>
using json = nlohmann::json;
using namespace std;
string insertServerStmt = "INSERT INTO servers(id, admin_id, name) VALUES "
                          "(?, ?, ?)";
string selectServersStmt = "SELECT * FROM servers";

// Inserts a new server into the database
string createServer(string adminId, string name) {
  string serverId = createId();
  cout << serverId << endl;
  Database *db = Database::getInstance();
  Statement stmt = db->newStatement(insertServerStmt);
  stmt.bind(serverId);
  stmt.bind(adminId);
  stmt.bind(name);
  stmt.execute();
  stmt.finish();
  return serverId;
}

vector<json> getServers() {
  vector<json> servers;
  Database *db = Database::getInstance();
  Statement stmt = db->newStatement(selectServersStmt);
  while (stmt.step() == SQLITE_ROW) {
    servers.push_back(json{
      {     "id", stmt.getResultString(0)},
      {"adminId", stmt.getResultString(1)},
      {   "name", stmt.getResultString(2)}
    });
  }
  stmt.finish();
  return servers;
}