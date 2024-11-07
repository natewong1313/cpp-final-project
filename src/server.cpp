#include <string>
#include <vector>
using namespace std;

class Server {
  private:
    int id;
    int admin_id;
    string name;
    // vector<Message> messages;

  public:
    Server(int admin_id, string name) {
      this->admin_id = admin_id;
      this->name = name;
    }
};