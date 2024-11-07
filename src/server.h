#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <vector>
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
};

vector<ChatServer> loadChatServersFromDb();

#endif
