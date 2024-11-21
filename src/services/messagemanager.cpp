#include "messagemanager.h"

#include <iostream>
#include <mutex>
#include <string>
#include <unordered_map>
using namespace std;

MessageManager::MessageManager() {
  channels = unordered_map<string, shared_ptr<ChannelListener>>();
}

void MessageManager::broadcast_message(string channelId, string message) {
  // lock_guard<mutex> lk(m);
  // cv.notify_all();
  // cout << "message broadcasted" << endl;
}

shared_ptr<ChannelListener> MessageManager::get_listener(string channelId) {
  // shared lock which defers unlocking
  shared_lock<shared_mutex> lock(m);
  auto listener = channels.find(channelId);
  if (listener == channels.end()) { return nullptr; }
  return listener->second;
}

void MessageManager::listen_for_message(string channelId) {
  cout << "Listening" << endl;

  auto listener = get_listener(channelId);
  if (listener == nullptr) {
    cout << "Channel not found" << endl;
    return;
  } else {
    cout << "Channel found" << endl;
  }

  // unique_lock<mutex> lk(m);
  // cv.wait(lk);
  // cout << "Message received" << endl;
}