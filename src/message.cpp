#include "message.h"

#include "db.h"
#include "json.hpp"

#include <ctime>
#include <format>
#include <iostream>
#include <sqlite3.h>
#include <string.h>
using json = nlohmann::json;
using namespace std;

string selectMessagesStmt = "SELECT * FROM messages WHERE server_id=? ORDER BY created_at";
string createMessageStmt = "INSERT INTO messages(author_id, server_id, content, created_at) VALUES "
                           "(?, ?, ?, ?) RETURNING id";

vector<json> loadMessagesFromDb(int serverId) {
  vector<json> messagesJson;
  Database *db = Database::getInstance();
  sqlite3 *dbConn = db->getConnection();
  sqlite3_stmt *stmt;

  int rc = sqlite3_prepare_v2(dbConn, selectMessagesStmt.c_str(), -1, &stmt, NULL);
  if (rc != SQLITE_OK) {
    cerr << "Error loading messages" << sqlite3_errmsg(dbConn) << endl;
    return messagesJson;
  }
  sqlite3_bind_int(stmt, 1, serverId);

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    Message msg =
      Message(sqlite3_column_int(stmt, 0), sqlite3_column_int(stmt, 1), sqlite3_column_int(stmt, 2),
              sqlite3_column_text(stmt, 3), sqlite3_column_int(stmt, 4));
    messagesJson.push_back(msg.toJson());
  };
  rc = sqlite3_finalize(stmt);
  if (rc != SQLITE_OK) { cerr << "Error loading servers" << sqlite3_errmsg(dbConn) << endl; }
  return messagesJson;
}

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

Message::Message(int id, int authorId, int serverId, const unsigned char *content, int createdAt) {
  this->id = id;
  this->authorId = authorId;
  this->serverId = serverId;
  this->content = reinterpret_cast<const char *>(content);
  this->createdAt = createdAt;
}

json Message::toJson() {
  return json{
    {       "id",        id},
    { "authorId",  authorId},
    // { "serverId",  serverId},
    {  "content",   content},
    {"createdAt", createdAt},
  };
}