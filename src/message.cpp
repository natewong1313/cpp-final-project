#include "message.h"

#include "db.h"

#include <ctime>
#include <format>
#include <iostream>
#include <sqlite3.h>
#include <string.h>
using namespace std;

// sql constants
string createMessageStmt = "INSERT INTO messages(author_id, server_id, content, created_at) VALUES "
                           "(?, ?, ?, ?) RETURNING id";

Message::Message(int authorId, int serverId, string content) {
  // https://stackoverflow.com/questions/6012663/get-unix-timestamp-with-c
  unsigned long int createdAt = time(NULL);
  this->authorId = authorId;
  this->content = content;

  Database *db = Database::getInstance();
  sqlite3 *dbConn = db->getConnection();
  sqlite3_stmt *stmt;
  // https://gist.github.com/jsok/2936764
  // https://stackoverflow.com/questions/31745465/how-to-prepare-sql-statements-and-bind-parameters
  int rc = sqlite3_prepare_v2(dbConn, createMessageStmt.c_str(), -1, &stmt, NULL);
  if (rc != SQLITE_OK) {
    cerr << "Error creating message" << sqlite3_errmsg(dbConn) << endl;
    return;
  }
  sqlite3_bind_int(stmt, 1, authorId);
  sqlite3_bind_int(stmt, 2, serverId);
  sqlite3_bind_text(stmt, 3, content.c_str(), strlen(content.c_str()), NULL);
  sqlite3_bind_int(stmt, 4, createdAt);

  rc = sqlite3_step(stmt);
  if (rc != SQLITE_ROW) {
    cerr << "Error creating message" << sqlite3_errmsg(dbConn) << endl;
    return;
  }

  int messageId = sqlite3_column_int(stmt, 0);
  this->id = messageId;

  rc = sqlite3_finalize(stmt);
  if (rc != SQLITE_OK) {
    cerr << "Error creating message" << sqlite3_errmsg(dbConn) << endl;
    return;
  }
  cout << "Created message successfully" << endl;
}