#include "db.h"
#include "httplib.h"
#include "json.hpp"
#include "message.h"
#include "server.h"
#include "utils.h"

#include <iostream>
#include <sqlite3.h>
using namespace std;
using namespace httplib;
using json = nlohmann::json;

Server svr;

int main() {
  Database *newDb = Database::getInstance();
  vector<ChatServer> servers = loadChatServersFromDb();
  // ChatServer server = ChatServer(1, "Nates server");
  // Message msg = Message(1, server.getId(), "Hello world");

  svr.Get("/", [](const Request &, Response &res) {
    string html = loadHTML("index.html");
    res.set_content(html, "text/html");
  });
  svr.Get("/login", [](const Request &, Response &res) {
    string html = loadHTML("login.html");
    res.set_content(html, "text/html");
  });

  svr.Get("/test", [](const Request &, Response &res) {
    json j;
    j["hello"] = "world";
    res.set_content(to_string(j), "application/json");
  });
  cout << "Server running on port 8080" << endl;
  svr.listen("0.0.0.0", 8080);
  return 0;
}