#include "message.h"

#include "db.h"

#include <ctime>
#include <format>
#include <iostream>
#include <sqlite3.h>
#include <string.h>
using namespace std;

// sql constants
string createMessageStmt = "INSERT INTO messages(author_id, content, created_at) VALUES (?, ?, ?)";

Message::Message(int authorId, string content) {
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
  sqlite3_bind_text(stmt, 2, content.c_str(), strlen(content.c_str()), NULL);
  sqlite3_bind_int(stmt, 3, createdAt);

  rc = sqlite3_step(stmt);
  if (rc != SQLITE_OK) {
    cerr << "Error creating message" << endl;
    return;
  }
  rc = sqlite3_finalize(stmt);
  if (rc != SQLITE_OK) {
    cerr << "Error creating message" << endl;
    return;
  }
  cout << "Created message successfully" << endl;
  // char *errorMsg;
  // int rc = sqlite3_exec(db, , NULL, 0, &errorMsg);
  // if (rc != SQLITE_OK) {
  //   // todo: improve error handling
  //   cout << "Error creating message" << endl;
  //   sqlite3_free(errorMsg);
  //   return
  // }
}