#include "db.h"
#include "httplib.h"
#include "json.hpp"
#include "services/auth.h"
#include "services/channels.h"
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

  svr.Get("/", [](const Request &req, Response &res) {
    if (!isAuthenticatedReq(req)) {
      res.set_redirect("/login");
      return;
    }
    res.set_content(loadHTML("index.html"), "text/html");
  });
  svr.Get("/login", [](const Request &req, Response &res) {
    res.set_content(loadHTML("login.html"), "text/html");
  });
  svr.Get("/server", [](const Request &req, Response &res) {
    if (!isAuthenticatedReq(req)) {
      res.set_redirect("/login");
      return;
    }
    string serverId = req.get_param_value("id");
    if (serverId == "") {
      res.set_redirect("/");
      return;
    }
    string channelId = req.get_param_value("channel");
    if (channelId == "") {
      string generalChannelId = getGeneralChannel(serverId);
      res.set_redirect("/server?id=" + serverId + "&channel=" + generalChannelId);
      return;
    }
    res.set_content(loadHTML("server.html"), "text/html");
  });

  svr.Post("/api/signin", [](const Request &req, Response &res) {
    json j = json::parse(req.body);
    bool success = authenticateUser(j["email"], j["password"]);
    if (!success) {
      res.set_content(to_string(json{{"error", "Invalid email or password"}}), "application/json");
      res.status = 400;
      return;
    }
    string userId = getUserIdByEmail(j["email"]);
    string sessionToken = createSessionToken(userId);
    res.set_header("Set-Cookie", getCookieString(sessionToken));
  });
  svr.Post("/api/signup", [](const Request &req, Response &res) {
    json j = json::parse(req.body);
    string userId = createUser(j["username"], j["email"], j["password"]);
    if (!userId.compare("")) {
      res.set_content(to_string(json{{"error", "User already exists"}}), "application/json");
      res.status = 400;
      return;
    }
    string sessionToken = createSessionToken(userId);
    res.set_header("Set-Cookie", getCookieString(sessionToken));
  });

  svr.Get("/api/user", [](const Request &req, Response &res) {
    string sessionToken = getTokenFromReq(req);
    string userId = getUserIdFromToken(sessionToken);
    res.set_content(to_string(json{{"id", userId}}), "application/json");
  });

  svr.Get("/api/servers", [](const Request &, Response &res) {
    json j = json{{"servers", getServers()}};
    res.set_content(to_string(j), "application/json");
  });
  svr.Post("/api/servers/new", [](const Request &req, Response &res) {
    json j = json::parse(req.body);
    string sessionToken = getTokenFromReq(req);
    string userId = getUserIdFromToken(sessionToken);
    string serverId = createServer(userId, j["name"]);
    json resJson = json{{"id", serverId}};
    res.set_content(to_string(resJson), "application/json");
  });
  svr.Get("/api/server", [](const Request &req, Response &res) {
    if (!req.has_param("id")) {
      res.set_content(to_string(json{{"error", "Missing server id"}}), "application/json");
      res.status = 400;
      return;
    }
    json serverData = getServer(req.get_param_value("id"));
    res.set_content(to_string(serverData), "application/json");
  });
  svr.Get("/api/channels", [](const Request &req, Response &res) {
    if (!req.has_param("server")) {
      res.set_content(to_string(json{{"error", "Missing server id"}}), "application/json");
      res.status = 400;
      return;
    }
    json channelsData = getChannels(req.get_param_value("server"));
    res.set_content(to_string(channelsData), "application/json");
  });
  svr.Post("/api/channels/new", [](const Request &req, Response &res) {
    json j = json::parse(req.body);
    // string sessionToken = getTokenFromReq(req);
    // string userId = getUserIdFromToken(sessionToken);
    string channelId = createChannel(j["serverId"], j["name"]);
    json resJson = json{{"id", channelId}};
    res.set_content(to_string(resJson), "application/json");
  });
  svr.Get("/api/messages", [](const Request &req, Response &res) {
    if (!req.has_param("channel")) {
      res.set_content(to_string(json{{"error", "Missing channel id"}}), "application/json");
      res.status = 400;
      return;
    }
    json messagesData = getMessages(req.get_param_value("channel"));
    res.set_content(to_string(messagesData), "application/json");
  });
  svr.Post("/api/messages/new", [](const Request &req, Response &res) {
    json j = json::parse(req.body);
    string sessionToken = getTokenFromReq(req);
    string channelId = j["channelId"];
    string msgContent = j["message"];
    string userId = getUserIdFromToken(sessionToken);
    json msgJson = sendMessage(userId, channelId, msgContent);
    res.set_content(to_string(msgJson), "application/json");
  });

  cout << "Server running on port 8080" << endl;
  svr.listen("0.0.0.0", 8080);
  return 0;
}