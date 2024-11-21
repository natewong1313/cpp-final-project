#include "messagemanager.h"

#include <iostream>
#include <mutex>
#include <string>
using namespace std;

MessageManager::MessageManager() {}

void MessageManager::broadcast_message(string channelId, string message) {
  lock_guard<mutex> lk(m);
  cv.notify_all();
  cout << "message broadcasted" << endl;
}

void MessageManager::listen_for_message(string channelId) {
  cout << "Listening" << endl;
  unique_lock<mutex> lk(m);
  cv.wait(lk);
  cout << "Message received" << endl;
}