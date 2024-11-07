#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
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
    // todo: add constructor from db row
    void update(string newContent);
    void save();
};

#endif