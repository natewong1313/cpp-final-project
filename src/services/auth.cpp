#include "../db.h"
#include "../utils.h"

#include <string.h>
#include <string>
using namespace std;

string selectUserStmt = "SELECT password FROM users WHERE email=?";
string insertTokenStmt = "INSERT INTO userTokens(token, user_id) VALUES (?, ?)";

// not gonna hash passwords for now but that is something that should be done in the future
bool authenticateUser(string email, string password) {
  Database *db = Database::getInstance();
  sqlite3_stmt *stmt;
  int rc = sqlite3_prepare_v2(db->getConnection(), selectUserStmt.c_str(), -1, &stmt, NULL);
  if (rc != SQLITE_OK) {
    handleDbError(db);
    return false;
  }
  sqlite3_bind_text(stmt, 1, email.c_str(), strlen(email.c_str()), NULL);

  rc = sqlite3_step(stmt);
  if (rc != SQLITE_ROW) {
    handleDbError(db);
    return false;
  }

  string pwToCompare = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));

  rc = sqlite3_finalize(stmt);
  if (rc != SQLITE_OK) {
    handleDbError(db);
    return false;
  }

  return pwToCompare.compare(password);
}

string createSessionToken(string userId) {
  string sessionToken = createId();

  Database *db = Database::getInstance();
  sqlite3_stmt *stmt;
  int rc = sqlite3_prepare_v2(db->getConnection(), insertTokenStmt.c_str(), -1, &stmt, NULL);
  if (rc != SQLITE_OK) {
    handleDbError(db);
    return "";
  }
  sqlite3_bind_text(stmt, 1, sessionToken.c_str(), strlen(sessionToken.c_str()), NULL);
  sqlite3_bind_text(stmt, 2, userId.c_str(), strlen(userId.c_str()), NULL);

  rc = sqlite3_step(stmt);
  if (rc != SQLITE_OK) {
    handleDbError(db);
    return "";
  }
  rc = sqlite3_finalize(stmt);
  if (rc != SQLITE_OK) {
    handleDbError(db);
    return "";
  }

  return sessionToken;
}