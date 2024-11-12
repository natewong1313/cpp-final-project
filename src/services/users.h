#ifndef USERS_H
#define USERS_H

#include <string>
using namespace std;

string createUser(string username, string email, string password);
string getUserIdByEmail(string email);

#endif