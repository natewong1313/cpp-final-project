// Handles user auth operations with the userTokens and users tables

#ifndef AUTH_H
#define AUTH_H

#include "httplib.h"

#include <string>
using namespace httplib;
using namespace std;

bool authenticateUser(string email, string password);
string createSessionToken(string userId);
void logoutUser(string sessionToken);
string buildCookieString(string sessionToken);
bool hasTokenCookie(const Request &req);
string getTokenFromReq(const Request &req);
string getUserIdFromToken(string sessionToken);
bool isAuthenticatedReq(const Request &req);

#endif