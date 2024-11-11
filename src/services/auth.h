#ifndef AUTH_H
#define AUTH_H

#include <string>
using namespace std;

bool authenticateUser(string email, string password);
string createSessionToken(string userId);
void logoutUser(string sessionToken);

#endif