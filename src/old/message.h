#ifndef MESSAGE_H
#define MESSAGE_H

#include "json.hpp"

#include <string>
#include <vector>
using json = nlohmann::json;
using namespace std;

class Message {
  private:
    int id;
    int authorId;
    int serverId;
    string content;
    int createdAt;

  public:
    Message(int authorId, int serverId, string content);
    Message(int id, int authorId, int serverId, const unsigned char *content, int createdAt);
    void update(string newContent);
    void save();
    json toJson();
};

vector<json> loadMessagesFromDb(int serverId);

#endif