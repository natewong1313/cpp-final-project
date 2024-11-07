#include "server.h"

#include "db.h"

#include <iostream>
#include <sqlite3.h>
#include <string.h>

string createServerStmt = "INSERT INTO servers(admin_id, name) VALUES (?, ?) RETURNING id";

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

int ChatServer::getId() {
  return id;
}