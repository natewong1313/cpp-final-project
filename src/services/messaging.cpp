#include "../db.h"
#include "../utils.h"

#include <string.h>
#include <string>
#include <thread>
using namespace std;

string createMsgStmt = "INSERT INTO messages(id, author_id, server_id, content, created_at) VALUES "
                       "(?, ?, ?, ?, ?)";

// Sends message to connected clients
void broadcastMessage(string messageId, string authorId, string serverId, string content) {
  cout << "New message " + content << endl;
};

// Inserts a new message into the database
void addMessageToDb(string messageId, string authorId, string serverId, string content) {
  Database *db = Database::getInstance();
  sqlite3_stmt *stmt;
  int rc = sqlite3_prepare_v2(db->getConnection(), createMsgStmt.c_str(), -1, &stmt, NULL);
  if (rc != SQLITE_OK) { return handleDbError(db); }

  sqlite3_bind_text(stmt, 1, messageId.c_str(), strlen(messageId.c_str()), NULL);
  sqlite3_bind_text(stmt, 2, authorId.c_str(), strlen(authorId.c_str()), NULL);
  sqlite3_bind_text(stmt, 3, serverId.c_str(), strlen(serverId.c_str()), NULL);
  sqlite3_bind_text(stmt, 4, content.c_str(), strlen(content.c_str()), NULL);
  sqlite3_bind_int(stmt, 5, time(NULL));

  rc = sqlite3_step(stmt);
  if (rc != SQLITE_OK) { return handleDbError(db); }
  rc = sqlite3_finalize(stmt);
  if (rc != SQLITE_OK) { return handleDbError(db); }
}

// Broadcasts the message and adds it to the database
void sendMessage(string authorId, string serverId, string content) {
  string messageId = createId();

  thread msgThread(broadcastMessage, messageId, authorId, serverId, content);
  msgThread.detach();
  thread dbThread(addMessageToDb, messageId, authorId, serverId, content);
  dbThread.detach();
}
