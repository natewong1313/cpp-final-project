#include "../db.h"
#include "../utils.h"

#include <string.h>
#include <string>
#include <thread>
using namespace std;

void broadcastMessage(int authorId, int serverId, int messageId, string content) {
  cout << content << endl;
};

string createMsgStmt = "INSERT INTO messages(author_id, server_id, content, created_at) VALUES "
                       "(?, ?, ?, ?) RETURNING id";

// TODO: generate message id before insertion so we can optimistically send updates to clients
bool sendMessage(int authorId, int serverId, string content) {
  Database *db = Database::getInstance();
  sqlite3_stmt *stmt;

  int rc = sqlite3_prepare_v2(db->getConnection(), createMsgStmt.c_str(), -1, &stmt, NULL);
  if (rc != SQLITE_OK) { return handleDbError(db); }
  sqlite3_bind_int(stmt, 1, authorId);
  sqlite3_bind_int(stmt, 2, serverId);
  sqlite3_bind_text(stmt, 3, content.c_str(), strlen(content.c_str()), NULL);
  sqlite3_bind_int(stmt, 4, time(NULL));

  rc = sqlite3_step(stmt);
  if (rc != SQLITE_ROW) { return handleDbError(db); }

  thread msgThread(broadcastMessage, authorId, serverId, sqlite3_column_int(stmt, 0), content);
  msgThread.detach();

  rc = sqlite3_finalize(stmt);
  if (rc != SQLITE_OK) { return handleDbError(db); }
  return true;
}