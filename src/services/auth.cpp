#include "../db.h"
#include "../utils.h"
#include "httplib.h"

#include <string>
using namespace httplib;
using namespace std;

string selectUserStmt = "SELECT password FROM users WHERE email=?";
string insertTokenStmt = "INSERT INTO userTokens(token, user_id) VALUES (?, ?)";
string deleteTokenStmt = "DELETE FROM userTokens WHERE token=?";
string selectUserFromTokenStmt = "SELECT user_id FROM userTokens WHERE token=?";

// not gonna hash passwords for now but that is something that should be done in the future
bool authenticateUser(string email, string password) {
  Database *db = Database::getInstance();
  Statement stmt = db->newStatement(selectUserStmt);
  stmt.bind(email);
  stmt.execute();
  string pwToCompare = stmt.getResultString(0);
  stmt.finish();
  if (pwToCompare == "") { return false; }
  return pwToCompare == password;
}

string createSessionToken(string userId) {
  string sessionToken = createId();
  Database *db = Database::getInstance();
  Statement stmt = db->newStatement(insertTokenStmt);
  stmt.bind(sessionToken);
  stmt.bind(userId);
  stmt.execute();
  stmt.finish();
  return sessionToken;
}

void logoutUser(string sessionToken) {
  Database *db = Database::getInstance();
  Statement stmt = db->newStatement(deleteTokenStmt);
  stmt.bind(sessionToken);
  stmt.execute();
  stmt.finish();
}

int weekExpiry = 7 * 24 * 60 * 60;

string getCookieString(string sessionToken) {
  time_t expiry = time(0) + weekExpiry;
  char expiration[256];
  strftime(expiration, sizeof(expiration), "%a, %d %b %Y %H:%M:%S GMT", gmtime(&expiry));
  return "token=" + sessionToken + "; Expires=" + expiration + "; Path=/";
}

bool hasTokenCookie(const Request &req) {
  return req.has_header("Cookie") && req.get_header_value("Cookie").find("token=") != string::npos;
}

string getTokenFromReq(const Request &req) {
  string cookieHeader = req.get_header_value("Cookie");
  int startPos = cookieHeader.find("token=") + 6;
  int endPos = cookieHeader.find(";", startPos);
  // if the header only has one cookie
  if (endPos == -1) { return cookieHeader.substr(startPos); }
  return cookieHeader.substr(startPos, endPos - 6);
}

string getUserIdFromToken(string sessionToken) {
  Database *db = Database::getInstance();
  Statement stmt = db->newStatement(selectUserFromTokenStmt);
  stmt.bind(sessionToken);
  int rc = stmt.execute();
  if (rc != SQLITE_ROW) {
    stmt.finish();
    throw invalid_argument("Invalid token");
  }
  string userId = stmt.getResultString(0);
  stmt.finish();
  return userId;
}

bool isAuthenticatedReq(const Request &req) {
  if (!hasTokenCookie(req)) { return false; }
  string sessionToken = getTokenFromReq(req);
  try {
    getUserIdFromToken(sessionToken);
    return true;
  } catch (const invalid_argument &e) { return false; }
}
