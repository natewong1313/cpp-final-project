#include "../db.h"
#include "../utils.h"

#include <string.h>
#include <string>
#include <thread>
using namespace std;

string createMsgStmt = "INSERT INTO messages(id, author_id, server_id, content, created_at) VALUES "
                       "(?, ?, ?, ?, ?) RETURNING id";

// Sends message to connected clients
void broadcastMessage(string messageId, int authorId, int serverId, string content) {
  cout << "New message " + content << endl;
};

void addMessageToDb(string messageId, int authorId, int serverId, string content) {
  Database *db = Database::getInstance();
  sqlite3_stmt *stmt;
  int rc = sqlite3_prepare_v2(db->getConnection(), createMsgStmt.c_str(), -1, &stmt, NULL);
  if (rc != SQLITE_OK) { return handleDbError(db); }

  sqlite3_bind_text(stmt, 1, messageId.c_str(), strlen(messageId.c_str()), NULL);
  sqlite3_bind_int(stmt, 2, authorId);
  sqlite3_bind_int(stmt, 3, serverId);
  sqlite3_bind_text(stmt, 4, content.c_str(), strlen(content.c_str()), NULL);
  sqlite3_bind_int(stmt, 5, time(NULL));

  rc = sqlite3_step(stmt);
  if (rc != SQLITE_ROW) { return handleDbError(db); }
  rc = sqlite3_finalize(stmt);
  if (rc != SQLITE_OK) { return handleDbError(db); }
}

// Broadcasts the message and adds it to the database
void sendMessage(int authorId, int serverId, string content) {
  string messageId = createId();

  thread msgThread(broadcastMessage, messageId, authorId, serverId, content);
  msgThread.detach();
  thread dbThread(addMessageToDb, messageId, authorId, serverId, content);
  dbThread.detach();
}
