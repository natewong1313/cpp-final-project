#ifndef SERVER_H
#define SERVER_H

#include "json.hpp"

#include <string>
#include <vector>
using json = nlohmann::json;
using namespace std;

class ChatServer {
  private:
    int id;
    int adminId;
    string name;

  public:
    ChatServer(int adminId, string name);
    ChatServer(int id, int adminId, const unsigned char *name);
    int getId();
    json toJson();
};

vector<json> loadChatServersFromDb();

#endif
