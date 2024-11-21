#ifndef MESSAGEMANAGER_H
#define MESSAGEMANAGER_H

#include <condition_variable>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <unordered_map>
using namespace std;

class ChannelListener {
  private:
    condition_variable cv;
    mutex m;

  public:
    ChannelListener();
};

class MessageManager {
  private:
    condition_variable cv;
    shared_mutex m;
    unordered_map<string, shared_ptr<ChannelListener>> channels;

    shared_ptr<ChannelListener> get_listener(string channelId);

  public:
    MessageManager();
    void broadcast_message(string channelId, string message);
    void listen_for_message(string channelId);
};

#endif