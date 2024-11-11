#include "../db.h"
#include "../utils.h"

#include <string.h>
#include <string>
using namespace std;

string insertuserStmt = "INSERT INTO users(id, username, email, password) VALUES "
                        "(?, ?, ?, ?)";

string createUser(string username, string email, string password) {
  string userId = createId();
  Database *db = Database::getInstance();
  sqlite3_stmt *stmt;
  int rc = sqlite3_prepare_v2(db->getConnection(), insertuserStmt.c_str(), -1, &stmt, NULL);
  if (rc != SQLITE_OK) {
    handleDbError(db);
    return "";
  }

  sqlite3_bind_text(stmt, 1, userId.c_str(), strlen(userId.c_str()), NULL);
  sqlite3_bind_text(stmt, 2, username.c_str(), strlen(username.c_str()), NULL);
  sqlite3_bind_text(stmt, 3, email.c_str(), strlen(email.c_str()), NULL);
  sqlite3_bind_text(stmt, 4, password.c_str(), strlen(password.c_str()), NULL);

  rc = sqlite3_step(stmt);
  if (rc != 101) {
    handleDbError(db);
    return "";
  }
  rc = sqlite3_finalize(stmt);
  if (rc != 0) {
    handleDbError(db);
    return "";
  }
  return userId;
}