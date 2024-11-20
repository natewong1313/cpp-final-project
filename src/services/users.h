#ifndef USERS_H
#define USERS_H
#include "json.hpp"

#include <string>
using namespace std;
using json = nlohmann::json;

string createUser(string username, string email, string password);
string getUserIdByEmail(string email);
json getUser(string userId);

#endif