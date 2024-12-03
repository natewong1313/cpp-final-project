#include "../db.h"
#include "../utils.h"
#include "json.hpp"

#include <sqlite3.h>
#include <string.h>
#include <string>
using namespace std;
using json = nlohmann::json;

string insertuserStmt = "INSERT INTO users(id, username, email, password) VALUES "
                        "(?, ?, ?, ?)";
string selectUserByEmailStmt = "SELECT id FROM users WHERE email=?";
string selectUserByIdStmt = "SELECT * FROM users WHERE id=?";

// Creates a user in the db
// Throws an error if the username or email already exists
// Returns the created userId
string createUser(string username, string email, string password) {
  string userId = createId();
  Database *db = Database::getInstance();
  Statement stmt = db->newStatement(insertuserStmt);
  stmt.bind(userId);
  stmt.bind(username);
  stmt.bind(email);
  stmt.bind(password);
  int rc = stmt.execute();
  stmt.finish();
  if (rc == 19) {
    string errMsg = sqlite3_errmsg(db->getConnection());
    if (errMsg == "UNIQUE constraint failed: users.username") {
      throw invalid_argument("Username already exists");
    } else if (errMsg == "UNIQUE constraint failed: users.email") {
      throw invalid_argument("Email already exists");
    }
  }
  return userId;
}

// Returns a userId given an email
string getUserIdByEmail(string email) {
  Database *db = Database::getInstance();
  Statement stmt = db->newStatement(selectUserByEmailStmt);
  stmt.bind(email);
  stmt.execute();
  string userId = stmt.getResultString(0);
  stmt.finish();
  return userId;
}

// Returns a user json obj given the id
// {"id": "", "userrname": "", "email": "", "password": ""}
json getUser(string userId) {
  Database *db = Database::getInstance();
  Statement stmt = db->newStatement(selectUserByIdStmt);
  stmt.bind(userId);
  stmt.execute();
  json userData = json{{"id", stmt.getResultString(0)},
                       {"username", stmt.getResultString(1)},
                       {"email", stmt.getResultString(2)},
                       {"password", stmt.getResultString(3)}};
  stmt.finish();
  return userData;
}