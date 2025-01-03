#ifndef SRC_CLIENT_H
#define SRC_CLIENT_H

#include <string>
#include <unordered_map>
#include <ostream>
#include <stack>
#include <queue>
#include <vector>

#define APPLICATION_LAYER_ID 0
#define TRANSPORT_LAYER_ID 1
#define NETWORK_LAYER_ID 2
#define PHYSICAL_LAYER_ID 3

#include "Packet.h"
#include "ApplicationLayerPacket.h"
#include "TransportLayerPacket.h"
#include "NetworkLayerPacket.h"
#include "PhysicalLayerPacket.h"
#include "Log.h"

using namespace std;

class Client {
public:
    Client(string const& id, string const& ip, string const& mac);
    ~Client();

    string client_id;
    string client_ip;
    string client_mac;
    vector<Log> log_entries;
    unordered_map < string, string > routing_table;
    queue<stack<Packet*>> incoming_queue;
    queue<stack<Packet*>> outgoing_queue;

    friend ostream &operator<<(ostream &os, const Client &client);
};


#endif //SRC_CLIENT_H
