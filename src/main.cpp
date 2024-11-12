#include "db.h"
#include "httplib.h"
#include "json.hpp"
#include "services/auth.h"
#include "services/messaging.h"
#include "services/servers.h"
#include "services/users.h"
#include "utils.h"

#include <iostream>
#include <sqlite3.h>
using namespace std;
using namespace httplib;
using json = nlohmann::json;

Server svr;

int main() {
  Database *newDb = Database::getInstance();

  // createServer()
  // vector<ChatServer> servers = loadChatServersFromDb();
  // ChatServer server = ChatServer(1, "Nates server");
  // Message msg = Message(1, server.getId(), "Hello world");
  // sendMessage(1, 1, "Test message");

  // createServer("testUser", "Nates new server");

  // string userId = createUser("nate2", "nate@gmail.com", "Password!");
  // cout << "user id" << userId << endl;

  // string sessionToken = createSessionToken(userId);
  // cout << sessionToken << endl;
  // logoutUser(sessionToken);
  // cout << "done" << endl;

  // bool success = authenticateUser("nate@gmail.com", "Password!");
  // cout << "done" << endl;

  svr.Get("/", [](const Request &, Response &res) {
    res.set_content(loadHTML("index.html"), "text/html");
  });
  svr.Get("/login", [](const Request &, Response &res) {
    res.set_content(loadHTML("login.html"), "text/html");
  });
  svr.Get("/server", [](const Request &, Response &res) {
    res.set_content(loadHTML("server.html"), "text/html");
  });

  svr.Post("/api/login", [](const Request &req, Response &res) {
    json j = json::parse(req.body);

    bool success = authenticateUser(j["email"], j["password"]);
    cout << "success" << success << endl;
    if (!success) {
      res.set_content(to_string(json{
                        {"error", "Invalid email or password"}
      }),
                      "application/json");
      res.status = 400;
      return;
    }
    string userId = getUserIdByEmail(j["email"]);
    string sessionToken = createSessionToken(userId);
    res.set_header("Set-Cookie", getCookieString(sessionToken));
  });

  svr.Get("/api/servers", [](const Request &, Response &res) {
    json j = json{
      {"servers", getServers()}
    };
    res.set_content(to_string(j), "application/json");
  });
  svr.Post("/api/servers/new", [](const Request &req, Response &res) {
    json j = json::parse(req.body);
    string userId = getUserIdByEmail("nate@gmail.com");
    cout << userId << "user id" << endl;
    string serverId = createServer(userId, j["name"]);
    json resJson = json{
      {"id", serverId}
    };
    res.set_content(to_string(resJson), "application/json");
  });
  // svr.Get("/api/messages", [](const Request &req, Response &res) {
  //   if (!req.has_param("server")) {
  //     res.set_content("{\"error\": \"missing server id\"}", "application/json");
  //     return;
  //   }
  //   int serverId = atoi(req.get_param_value("server").c_str());

  //   ChatServer server = ChatServer(serverId);
  //   cout << server.toJson() << endl;

  //   json j;
  //   j["messages"] = loadMessagesFromDb(serverId);
  //   res.set_content(to_string(j), "application/json");
  // });
  // svr.Post("/api/message", [](const Request &req, Response &res) {
  //   cout << req.body << endl;
  //   // int serverId = atoi(req.get_param_value("server").c_str());
  //   json j;
  //   // j["messages"] = loadMessagesFromDb(serverId);
  //   res.set_content(to_string(j), "application/json");
  // });
  cout << "Server running on port 8080" << endl;
  svr.listen("0.0.0.0", 8080);
  return 0;
}