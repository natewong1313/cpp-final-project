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
  // vector<ChatServer> servers = loadChatServersFromDb();
  // ChatServer server = ChatServer(1, "Nates server");
  // Message msg = Message(1, server.getId(), "Hello world");

  svr.Get("/", [](const Request &, Response &res) {
    res.set_content(loadHTML("index.html"), "text/html");
  });
  svr.Get("/server", [](const Request &, Response &res) {
    res.set_content(loadHTML("server.html"), "text/html");
  });

  svr.Get("/api/servers", [](const Request &, Response &res) {
    json j;
    j["servers"] = loadChatServersFromDb();
    res.set_content(to_string(j), "application/json");
  });
  svr.Get("/api/messages", [](const Request &req, Response &res) {
    if (!req.has_param("server")) {
      res.set_content("{\"error\": \"missing server id\"}", "application/json");
      return;
    }
    int serverId = atoi(req.get_param_value("server").c_str());
    json j;
    j["messages"] = loadMessagesFromDb(serverId);
    res.set_content(to_string(j), "application/json");
  });
  cout << "Server running on port 8080" << endl;
  svr.listen("0.0.0.0", 8080);
  return 0;
}