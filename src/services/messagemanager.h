#ifndef MESSAGEMANAGER_H
#define MESSAGEMANAGER_H

#include <condition_variable>
#include <map>
#include <mutex>
using namespace std;

class MessageManager {
  private:
    condition_variable cv;
    mutex m;
    map<string, ChannelListener> channels;

  public:
    void broadcast_message(string channelId, string message);
    void listen_for_message(string channelId);
};

class ChannelListener {};

#endif