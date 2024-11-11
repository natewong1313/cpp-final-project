#include "servers.h"

#include "../db.h"
#include "../utils.h"

#include <string.h>
#include <vector>
using namespace std;
string insertServerStmt = "INSERT INTO servers(id, admin_id, name) VALUES "
                          "(?, ?, ?)";

// Inserts a new server into the database
void createServer(string adminId, string name) {
  string serverId = createId();
  Database *db = Database::getInstance();
  Statement stmt = db->newStatement(insertServerStmt);
  stmt.bind(serverId);
  stmt.bind(adminId);
  stmt.bind(name);
  stmt.execute();
  stmt.finish();
}

vector<server> getServers() {}