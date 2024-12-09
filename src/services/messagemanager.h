#ifndef MESSAGEMANAGER_H
#define MESSAGEMANAGER_H

#include "../api/messaging.h"
#include "httplib.h"
#include "json.hpp"

#include <condition_variable>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <unordered_map>
using json = nlohmann::json;
using namespace std;
using namespace httplib;

// Processes and emits events to listeners
// Created by MessageManager
class ChannelListener {
  private:
    condition_variable cv;
    mutex m;
    json latestMessage;

  public:
    ChannelListener();

    void broadcast_message(json message);
    bool listen_for_message(DataSink &sink);
};

// Handles creating ChannelListener's
class MessageManager {
  private:
    condition_variable cv;
    shared_mutex m;
    unordered_map<string, shared_ptr<ChannelListener>> channels;

    shared_ptr<ChannelListener> get_existing_listener(string channelId);
    shared_ptr<ChannelListener> new_listener(string channelId);

  public:
    MessageManager();
    void broadcast_message(string channelId, json message);
    void listen_for_message(string channelId, DataSink &sink);
};

#endif