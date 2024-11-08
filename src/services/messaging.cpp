#include "messaging.h"

#include "../db.h"
#include "../utils.h"

#include <string.h>
#include <string>
#include <thread>
using namespace std;

string createMsgStmt = "INSERT INTO messages(id, author_id, server_id, content, created_at) VALUES "
                       "(?, ?, ?, ?, ?)";

// Sends message to connected clients
void broadcastMessage(message msg) {
  cout << "New message " + msg.content << endl;
};

// Inserts a new message into the database
void addMessageToDb(message msg) {
  Database *db = Database::getInstance();
  sqlite3_stmt *stmt;
  int rc = sqlite3_prepare_v2(db->getConnection(), createMsgStmt.c_str(), -1, &stmt, NULL);
  if (rc != SQLITE_OK) { return handleDbError(db); }

  sqlite3_bind_text(stmt, 1, msg.id.c_str(), strlen(msg.id.c_str()), NULL);
  sqlite3_bind_text(stmt, 2, msg.authorId.c_str(), strlen(msg.authorId.c_str()), NULL);
  sqlite3_bind_text(stmt, 3, msg.serverId.c_str(), strlen(msg.serverId.c_str()), NULL);
  sqlite3_bind_text(stmt, 4, msg.content.c_str(), strlen(msg.content.c_str()), NULL);
  sqlite3_bind_int(stmt, 5, time(NULL));

  rc = sqlite3_step(stmt);
  if (rc != SQLITE_OK) { return handleDbError(db); }
  rc = sqlite3_finalize(stmt);
  if (rc != SQLITE_OK) { return handleDbError(db); }
}

// Broadcasts the message and adds it to the database
void sendMessage(string authorId, string serverId, string content) {
  message msg = {
    createId(),
    authorId,
    serverId,
    content,
  };

  thread msgThread(broadcastMessage, msg);
  msgThread.detach();
  thread dbThread(addMessageToDb, msg);
  dbThread.detach();
}
