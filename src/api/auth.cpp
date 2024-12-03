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

// Given a users email and password, validate that they match an entry in the users table
bool authenticateUser(string email, string password) {
  // TOOO: hash passwords
  Database *db = Database::getInstance();
  Statement stmt = db->newStatement(selectUserStmt);
  stmt.bind(email);
  stmt.execute();
  string pwToCompare = stmt.getResultString(0);
  stmt.finish();
  if (pwToCompare == "") { return false; }
  return pwToCompare == password;
}

// Given a user id, create a token in the userTokens table
// and return the created sessionToken
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

// To logout a user, we just want to remove the token from userTokens
void logoutUser(string sessionToken) {
  Database *db = Database::getInstance();
  Statement stmt = db->newStatement(deleteTokenStmt);
  stmt.bind(sessionToken);
  stmt.execute();
  stmt.finish();
}

// Week expiry constant
int weekExpiry = 7 * 24 * 60 * 60;

// Given a session token, create a valid Cookie header value that includes the token
// and an expiration that is a week from now
// If I add more cookies to the site I'll need to update this function
string buildCookieString(string sessionToken) {
  time_t expiry = time(0) + weekExpiry;
  char expiration[256];
  strftime(expiration, sizeof(expiration), "%a, %d %b %Y %H:%M:%S GMT", gmtime(&expiry));
  return "token=" + sessionToken + "; Expires=" + expiration + "; Path=/";
}

// Given a request, check if the request has a Cookie header and
// that the Cookie value contains "token="
bool hasTokenCookie(const Request &req) {
  return req.has_header("Cookie") && req.get_header_value("Cookie").find("token=") != string::npos;
}

// Given a request, return the token cookie value by parsing the Cookie header
string getTokenFromReq(const Request &req) {
  string cookieHeader = req.get_header_value("Cookie");
  // The Cookie header value is structured weirdly so we have to do some hacky parsing
  int startPos = cookieHeader.find("token=") + 6;
  int endPos = cookieHeader.find(";", startPos);
  // if the header only has one cookie, it won't have ; in it
  if (endPos == -1) { return cookieHeader.substr(startPos); }
  return cookieHeader.substr(startPos, endPos - 6);
}

// Given a tokem, return the user id
// Throws an invalid_argument error if the token does not
// exist in the database
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

// Given a request, check if it contains a token cookie
// Also, validate the token by calling getUserIdFromToken, which
// will throw an exception if the token is invalid
bool isAuthenticatedReq(const Request &req) {
  if (!hasTokenCookie(req)) { return false; }
  string sessionToken = getTokenFromReq(req);
  try {
    getUserIdFromToken(sessionToken);
    return true;
  } catch (const invalid_argument &e) { return false; }
}
