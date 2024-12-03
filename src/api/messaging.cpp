#include "messaging.h"

#include "../db.h"
#include "../utils.h"
#include "json.hpp"

#include <string.h>
#include <string>
#include <thread>
using json = nlohmann::json;
using namespace std;

string createMsgStmt =
  "INSERT INTO messages(id, author_id, channel_id, content, created_at) VALUES "
  "(?, ?, ?, ?, ?)";
string selectMsgsStmt = "SELECT * FROM messages WHERE channel_id=?";

// Gets all messages for a server
// TODO: add pagination
// Returns an array: [{"id": "", "authorId": "", "channelId": "", "content": "", "createdAt": ""}]
vector<json> getMessages(string serverId) {
  vector<json> messages;
  Database *db = Database::getInstance();
  Statement stmt = db->newStatement(selectMsgsStmt);
  stmt.bind(serverId);
  while (stmt.step() == SQLITE_ROW) {
    // TODO: abstract to helper function
    messages.push_back(json{{"id", stmt.getResultString(0)},
                            {"authorId", stmt.getResultString(1)},
                            {"channelId", stmt.getResultString(2)},
                            {"content", stmt.getResultString(3)},
                            {"createdAt", stmt.getResultInt(4)}});
  }
  stmt.finish();
  return messages;
}

// Inserts a new message into the database
// Takes in a struct, since theres a bunch of fields
void addMessageToDb(message msg) {
  Database *db = Database::getInstance();
  Statement stmt = db->newStatement(createMsgStmt);
  stmt.bind(msg.id);
  stmt.bind(msg.authorId);
  stmt.bind(msg.channelId);
  stmt.bind(msg.content);
  stmt.bind(msg.createdAt);
  stmt.execute();
  stmt.finish();
}

// Adds a message to the database, returns single json
// TODO: rename this function
// Returns {"id": "", "authorId": "", "channelId": "", "content": "", "createdAt": ""}
json sendMessage(string authorId, string channelId, string content) {
  string messageId = createId();
  int createdAt = time(NULL);
  message msg = {messageId, authorId, channelId, content, createdAt};

  // thread msgThread(broadcastMessage, msg);
  // msgThread.detach();
  // premature optimization
  thread dbThread(addMessageToDb, msg);
  dbThread.detach();
  return json{{"id", messageId},
              {"authorId", authorId},
              {"channelId", channelId},
              {"content", content},
              {"createdAt", createdAt}};
}
