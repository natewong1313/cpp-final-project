#include "messagemanager.h"

#include "../api/messaging.h"
#include "httplib.h"
#include "json.hpp"

#include <iostream>
#include <mutex>
#include <string>
#include <unordered_map>
using json = nlohmann::json;
using namespace std;
using namespace httplib;

// A ChannelListener uses condition_variable to share messages with connected listeners
ChannelListener::ChannelListener() {}

// Sends a message to all listeners
void ChannelListener::broadcast_message(json message) {
  lock_guard<mutex> lk(m);
  // latestMessage needs mutex to be thread safe
  // since we can't send a message with condition_variable, we use a shared variable
  latestMessage = message;
  cv.notify_all();
  cout << "Sent message " << to_string(message) << endl;
}

// Waits for the condition_variable to be updated and on update,
// it writes the new message to the sink. If the connection is broken, it returns false
bool ChannelListener::listen_for_message(DataSink &sink) {
  unique_lock<mutex> lk(m);
  cv.wait(lk);
  string msg = "data: " + to_string(latestMessage) + "\n\n";
  return sink.write(msg.data(), msg.size());
}

// Initialize channels as an empty map that maps channel ids to ChannelListener instances
MessageManager::MessageManager() {
  channels = unordered_map<string, shared_ptr<ChannelListener>>();
}

// Shares a message with all listeners in a channel
void MessageManager::broadcast_message(string channelId, json message) {
  // There should always be a listener, since this function is called
  // After the listener is established
  shared_ptr<ChannelListener> listener = get_existing_listener(channelId);
  if (listener == nullptr) { listener = new_listener(channelId); }

  listener->broadcast_message(message);
}

shared_ptr<ChannelListener> MessageManager::get_existing_listener(string channelId) {
  // lock with read lock
  shared_lock<shared_mutex> lock(m);
  auto listener = channels.find(channelId);
  // if a listener isnt found, return null, otherwise return the ptr
  if (listener == channels.end()) { return nullptr; }
  return listener->second;
}

// Creates a new ChannelListener
shared_ptr<ChannelListener> MessageManager::new_listener(string channelId) {
  // lock the mutex since channels is being shared between threads
  lock_guard<shared_mutex> lock(m);
  // handle race condition
  auto existingListener = channels.find(channelId);
  // if a the listener exists, we don't want to create a new one and just
  // return the existing instance instead
  if (existingListener != channels.end()) { return existingListener->second; }
  // create a new listener and add it to channels
  shared_ptr<ChannelListener> listener = make_shared<ChannelListener>();
  channels.insert({channelId, listener});
  return listener;
}

// This function creates a new ChannelListener if it doesn't already exists
// And then calls listen_for_message in a loop until the connection is cut
void MessageManager::listen_for_message(string channelId, DataSink &sink) {
  shared_ptr<ChannelListener> listener = get_existing_listener(channelId);
  if (listener == nullptr) { listener = new_listener(channelId); }

  while (sink.is_writable()) {
    // If we aren't able to recieve messages anymore, this
    // Returns false and we exit
    bool success = listener->listen_for_message(sink);
    if (!success) { return; }
  }
}