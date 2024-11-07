#ifndef SERVER_H
#define SERVER_H

#include <string>
using namespace std;

class ChatServer {
  private:
    int id;
    int adminId;
    string name;

  public:
    ChatServer(int adminId, string name);
    int getId();
};

#endif
