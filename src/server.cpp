#include <string>
#include <vector>
using namespace std;

class Server {
  public:
    Server();

  private:
    string name;
    vector<Message> messages;
};

class Message {
  public:
    Message(string content, string authorId);

  private:
    string content;
    string authorId;
};