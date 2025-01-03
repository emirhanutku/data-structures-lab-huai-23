#ifndef NETWORK_H
#define NETWORK_H

#include <vector>
#include <iostream>
#include "Packet.h"
#include "Client.h"

using namespace std;

class Network {
public:
    Network();
    ~Network();

    // Executes commands given as a vector of strings while utilizing the remaining arguments.
    void process_commands(vector<Client> &clients, vector<string> &commands, int message_limit, const string &sender_port,
                     const string &receiver_port);

    // Initialize the network from the input files.
    static vector<Client> read_clients(string const &filename);
    void read_routing_tables(vector<Client> & clients, string const &filename);
    vector<string> read_commands(const string &filename);
    void  receiveCommand(vector<Client> &clients);

    void frameInfoCommand(vector<Client> clients,const std::vector<std::string>& words);
    void QInfoCommand(vector<Client> clients,const std::vector<std::string>& words);
    void printLogCommand(vector<Client> &clients,const string& clientId);

    string findClientIdFromIp( vector<Client> &clients, const string& ipAddress);
    static bool  controlClientExistFromId(vector<Client> &clients,const string& clientToControl);

    void splitMessage(std::string command,vector<string> &words);
    void messageCommand(vector<string> &words,vector<Client> &clients,int message_limit,
                                 const string &sender_port, const string &receiver_port);
    Client* findRequiredClient(vector<Client> &clients,const string& clientId);
    static std::string returnCurrentTime();
    void sendCommand(vector<Client> &clients);
    Client* findClientFromMac(vector<Client> &clients,const string& macAddress);
};

#endif
