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
  Statement stmt = db->newStatement(insertuserStmt);
  stmt.bind(userId);
  stmt.bind(username);
  stmt.bind(email);
  stmt.bind(password);
  stmt.execute();
  stmt.finish();
  return userId;
}