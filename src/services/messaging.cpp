#include "messaging.h"

#include "../db.h"
#include "../utils.h"

#include <string.h>
#include <string>
#include <thread>
using namespace std;

string createMsgStmt = "INSERT INTO messages(id, author_id, server_id, content, created_at) VALUES "
                       "(?, ?, ?, ?, ?)";

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
  stmt.bind(time(NULL));
  stmt.execute();
  stmt.finish();
}

// Broadcasts the message and adds it to the database
void sendMessage(string authorId, string serverId, string content) {
  message msg = {
    createId(),
    authorId,
    serverId,
    content,
  };

  thread msgThread(broadcastMessage, msg);
  msgThread.detach();
  thread dbThread(addMessageToDb, msg);
  dbThread.detach();
}
