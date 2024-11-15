#include "messaging.h"

#include "../db.h"
#include "../utils.h"
#include "json.hpp"

#include <string.h>
#include <string>
#include <thread>
using json = nlohmann::json;
using namespace std;

string createMsgStmt = "INSERT INTO messages(id, author_id, server_id, content, created_at) VALUES "
                       "(?, ?, ?, ?, ?)";
string selectMsgsStmt = "SELECT * FROM messages WHERE server_id=?";

vector<json> getMessages(string serverId) {
  vector<json> messages;
  Database *db = Database::getInstance();
  Statement stmt = db->newStatement(selectMsgsStmt);
  stmt.bind(serverId);
  while (stmt.step() == SQLITE_ROW) {
    messages.push_back(json{{"id", stmt.getResultString(0)},
                            {"authorId", stmt.getResultString(1)},
                            {"serverId", stmt.getResultString(2)},
                            {"content", stmt.getResultString(3)},
                            {"createdAt", stmt.getResultInt(4)}});
  }
  stmt.finish();
  return messages;
}

// Sends message to connected clients
void broadcastMessage(message msg) {
  cout << "New message " + msg.content << endl;
};

// Inserts a new message into the database
void addMessageToDb(message msg) {
  Database *db = Database::getInstance();
  Statement stmt = db->newStatement(createMsgStmt);
  stmt.bind(msg.id);
  stmt.bind(msg.authorId);
  stmt.bind(msg.serverId);
  stmt.bind(msg.content);
  stmt.bind(msg.createdAt);
  stmt.execute();
  stmt.finish();
}

// Broadcasts the message and adds it to the database
json sendMessage(string authorId, string serverId, string content) {
  string messageId = createId();
  int createdAt = time(NULL);
  message msg = {messageId, authorId, serverId, content, createdAt};

  thread msgThread(broadcastMessage, msg);
  msgThread.detach();
  thread dbThread(addMessageToDb, msg);
  dbThread.detach();
  return json{{"id", messageId},
              {"authorId", authorId},
              {"serverId", serverId},
              {"content", content},
              {"createdAt", createdAt}};
}
