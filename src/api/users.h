#ifndef USERS_H
#define USERS_H
#include "json.hpp"

#include <string>
using namespace std;
using json = nlohmann::json;

// returns user id
string createUser(string username, string email, string password);
// returns email
string getUserIdByEmail(string email);
// returns user as a json object,
// {"id", "username", "email", "password"}
json getUser(string userId);

#endif