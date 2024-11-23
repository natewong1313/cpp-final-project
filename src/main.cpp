#include "api/auth.h"
#include "api/channels.h"
#include "api/messaging.h"
#include "api/servers.h"
#include "api/users.h"
#include "db.h"
#include "httplib.h"
#include "json.hpp"
#include "services/messagemanager.h"
#include "utils.h"

#include <iostream>
#include <sqlite3.h>
#include <thread>
using namespace std;
using namespace httplib;
using json = nlohmann::json;

Server svr;
MessageManager mm;

int main() {
  Database *newDb = Database::getInstance();

  thread listenThread(&MessageManager::listen_for_message, &mm, "test");
  listenThread.detach();
  thread listenThread2(&MessageManager::listen_for_message, &mm, "test");
  listenThread2.detach();
  sleep(2);

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
    if (!isAuthenticatedReq(req)) { return res.set_redirect("/login"); }
    res.set_content(loadHTML("index.html"), "text/html");
  });
  svr.Get("/login", [](const Request &req, Response &res) {
    if (isAuthenticatedReq(req)) { return res.set_redirect("/"); }
    res.set_content(loadHTML("login.html"), "text/html");
  });
  svr.Get("/logout", [](const Request &req, Response &res) {
    res.set_header("Set-Cookie", "token=; Path=/; Expires=Thu, 01 Jan 1970 00:00:00 GMT");
    res.set_redirect("/login");
  });
  svr.Get("/server", [](const Request &req, Response &res) {
    if (!isAuthenticatedReq(req)) { return res.set_redirect("/login"); }
    string serverId = req.get_param_value("id");
    if (!isValidServerId(serverId)) { return res.set_redirect("/"); }
    string channelId = req.get_param_value("channel");
    if (!isValidChannelId(channelId)) {
      try {
        // if no channel id is provided, we'll redirect to the general channel
        string generalChannelId = getGeneralChannel(serverId);
        return res.set_redirect("/server?id=" + serverId + "&channel=" + generalChannelId);
      } catch (const invalid_argument &e) {
        // if the server doesnt exist, we'll redirect to the home page
        return res.set_redirect("/");
      }
    }
    res.set_content(loadHTML("server.html"), "text/html");
  });
  svr.Post("/api/signin", [](const Request &req, Response &res) {
    json j = json::parse(req.body);
    bool success = authenticateUser(j["email"], j["password"]);
    // if the login is invalid, we return an error
    if (!success) {
      res.status = 400;
      return res.set_content(to_string(json{{"error", "Invalid email or password"}}),
                             "application/json");
    }
    string userId = getUserIdByEmail(j["email"]);
    string sessionToken = createSessionToken(userId);
    res.set_header("Set-Cookie", getCookieString(sessionToken));
  });
  svr.Post("/api/signup", [](const Request &req, Response &res) {
    json j = json::parse(req.body);
    string userId;
    try {
      userId = createUser(j["username"], j["email"], j["password"]);
    } catch (const invalid_argument &e) {
      // if username or email already exist, we return an error
      res.status = 400;
      return res.set_content(to_string(json{{"error", e.what()}}), "application/json");
    }
    string sessionToken = createSessionToken(userId);
    res.set_header("Set-Cookie", getCookieString(sessionToken));
  });
  svr.Get("/api/user", [](const Request &req, Response &res) {
    string sessionToken = getTokenFromReq(req);
    try {
      // get the user id from the token and return the user data json
      string userId = getUserIdFromToken(sessionToken);
      res.set_content(to_string(getUser(userId)), "application/json");
    } catch (const invalid_argument &e) {
      // return an error if the token is invalid
      res.status = 401;
      return;
    }
  });
  svr.Get("/api/servers", [](const Request &, Response &res) {
    json j = json{{"servers", getServers()}};
    res.set_content(to_string(j), "application/json");
  });
  svr.Post("/api/servers/new", [](const Request &req, Response &res) {
    json j = json::parse(req.body);
    string userId;
    try {
      userId = getUserIdFromToken(getTokenFromReq(req));
    } catch (const invalid_argument &e) {
      // if token has an invalid user id, we'll return an error
      res.status = 401;
      return res.set_content(to_string(json{{"error", e.what()}}), "application/json");
    }
    string serverId;
    try {
      serverId = createServer(userId, j["name"]);
    } catch (const invalid_argument &e) {
      // if a server with the same name already exists, we'll return an error
      res.status = 400;
      return res.set_content(to_string(json{{"error", e.what()}}), "application/json");
    }
    res.set_content(to_string(json{{"id", serverId}}), "application/json");
  });
  svr.Get("/api/server", [](const Request &req, Response &res) {
    if (!req.has_param("id")) {
      res.status = 400;
      return res.set_content(to_string(json{{"error", "Missing server id"}}), "application/json");
    }
    try {
      // get the server data by id and return it
      json serverData = getServer(req.get_param_value("id"));
      res.set_content(to_string(serverData), "application/json");
    } catch (const invalid_argument &e) {
      // if the server id is invalid, we'll return an error
      res.status = 400;
      return res.set_content(to_string(json{{"error", e.what()}}), "application/json");
    }
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
  svr.Get("/api/test", [](const Request &req, Response &res) {
    mm.broadcast_message("test", {"test", "test", "test", "test", 0});
    res.set_content("{}", "application/json");
  });

  cout << "Server running on port 8080" << endl;
  svr.listen("0.0.0.0", 8080);
  return 0;
}