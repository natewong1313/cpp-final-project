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
  sqlite3_stmt *stmt;
  int rc = sqlite3_prepare_v2(db->getConnection(), insertServerStmt.c_str(), -1, &stmt, NULL);
  if (rc != SQLITE_OK) { return handleDbError(db); }

  sqlite3_bind_text(stmt, 1, serverId.c_str(), strlen(serverId.c_str()), NULL);
  sqlite3_bind_text(stmt, 2, adminId.c_str(), strlen(adminId.c_str()), NULL);
  sqlite3_bind_text(stmt, 3, name.c_str(), strlen(name.c_str()), NULL);

  rc = sqlite3_step(stmt);
  if (rc != SQLITE_OK) { return handleDbError(db); }
  rc = sqlite3_finalize(stmt);
  if (rc != SQLITE_OK) { return handleDbError(db); }
}

vector<server> getServers() {}