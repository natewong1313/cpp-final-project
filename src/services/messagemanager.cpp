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

ChannelListener::ChannelListener() {}

void ChannelListener::broadcast_message(json message) {
  lock_guard<mutex> lk(m);
  latestMessage = message;
  cv.notify_all();
  cout << "message broadcasted" << endl;
}

void ChannelListener::listen_for_message(DataSink &sink) {
  unique_lock<mutex> lk(m);
  cv.wait(lk);
  string msg = "data: " + to_string(latestMessage) + "\n\n";
  sink.write(msg.data(), msg.size());
}

MessageManager::MessageManager() {
  channels = unordered_map<string, shared_ptr<ChannelListener>>();
}

void MessageManager::broadcast_message(string channelId, json message) {
  // there should always be a listener
  shared_ptr<ChannelListener> listener = get_existing_listener(channelId);
  if (listener == nullptr) { listener = new_listener(channelId); }

  listener->broadcast_message(message);
  // lock_guard<mutex> lk(m);
  // cv.notify_all();
  // cout << "message broadcasted" << endl;
}

shared_ptr<ChannelListener> MessageManager::get_existing_listener(string channelId) {
  // lock with read lock
  shared_lock<shared_mutex> lock(m);
  auto listener = channels.find(channelId);
  // if a listener isnt found, return null, otherwise return the ptr
  if (listener == channels.end()) { return nullptr; }
  return listener->second;
}

shared_ptr<ChannelListener> MessageManager::new_listener(string channelId) {
  // write lock
  lock_guard<shared_mutex> lock(m);
  // handle rqace condition
  auto existingListener = channels.find(channelId);
  if (existingListener != channels.end()) { return existingListener->second; }
  // make the new listener
  shared_ptr<ChannelListener> listener = make_shared<ChannelListener>();
  channels.insert({channelId, listener});
  return listener;
}

void MessageManager::listen_for_message(string channelId, DataSink &sink) {
  shared_ptr<ChannelListener> listener = get_existing_listener(channelId);
  if (listener == nullptr) { listener = new_listener(channelId); }

  while (true) { listener->listen_for_message(sink); }
}