#include "Network.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

Network::Network() {

}

void Network::process_commands(vector<Client> &clients, vector<string> &commands, int message_limit,
                      const string &sender_port, const string &receiver_port) {
    for (const std::string& command : commands) {
        std::vector<std::string> words;
        splitMessage(command,words);
        cout << string(command.size() + 9, '-') << "\nCommand: " << command << "\n" << string(command.size() + 9, '-') << endl;




        if (words[0]=="MESSAGE"){
            messageCommand(words,clients,message_limit,sender_port,receiver_port);
        }
        else if (words[0]=="SEND"){
            sendCommand(clients);
        } else if (words[0]=="RECEIVE"){
            receiveCommand(clients);
        }
        else if (words[0]=="SHOW_FRAME_INFO"){
            frameInfoCommand(clients,words);
        }
        else if (words[0]=="SHOW_Q_INFO"){
            QInfoCommand(clients,words);
        }
        else if (words[0]=="PRINT_LOG"){
            printLogCommand(clients,words[1]);
        }
        else{
            std::cout<<"Invalid command.\n";
        }
        }
}



void Network::printLogCommand(vector<Client> &clients, const string& clientId){
    Client* requiredClient{findRequiredClient(clients,clientId)};
    int logEntryNumber{1};

    for(Log& log:requiredClient->log_entries){
        if (logEntryNumber==1){std::cout<<"Client "<<clientId<<" Logs:\n";}
        std::cout<<"--------------\n";

        std::cout<<"Log Entry #"<<logEntryNumber<<":\n";

        if (log.activity_type == ActivityType::MESSAGE_SENT) {
            cout << "Activity: Message Sent" << endl;
        } else if (log.activity_type == ActivityType::MESSAGE_FORWARDED) {
            cout << "Activity: Message Forwarded" << endl;
        } else if (log.activity_type == ActivityType::MESSAGE_DROPPED) {
            cout << "Activity: Message Dropped" << endl;
        } else if (log.activity_type == ActivityType::MESSAGE_RECEIVED) {
            cout << "Activity: Message Received" << endl;
        }
        std::cout<<"Timestamp: "<<log.timestamp<<"\n";
        std::cout<< "Number of frames: "<<log.number_of_frames<<"\n";
        std::cout<<"Number of hops: "<<log.number_of_hops<<"\n";
        std::cout<<"Sender ID: "<<log.sender_id<<"\n";
        std::cout<<"Receiver ID: "<<log.receiver_id<<"\n";
        std::cout<<"Success: "<<  ((log.success_status) ? "Yes" : "No")  <<"\n";

        if (log.activity_type==ActivityType::MESSAGE_SENT||log.activity_type==ActivityType::MESSAGE_RECEIVED){std::cout<<"Message: \""<<log.message_content<<"\"\n";}

        logEntryNumber++;
    }

}

void Network::QInfoCommand(vector<Client> clients,const std::vector<std::string>& words){
    Client* requiredClient{findRequiredClient(clients,words[1])};
    queue<stack<Packet *>> queue = (words[2] == "out") ? requiredClient->outgoing_queue : requiredClient->incoming_queue;
    string queueName=(words[2]=="out") ? "Outgoing Queue" : "Incoming Queue";
    std::cout<<"Client "<<words[1]<<" "<<queueName<<" Status\n";
    std::cout<<"Current total number of frames: "<<queue.size()<<std::endl;

}

void Network::frameInfoCommand(vector<Client> clients,const std::vector<std::string>& words){
    Client* requiredClient{findRequiredClient(clients,words[1])};
    queue<stack<Packet *>> queue = (words[2] == "out") ? requiredClient->outgoing_queue : requiredClient->incoming_queue;
    string queueName=(words[2]=="out") ? "outgoing queue" : "incoming queue";
    int frameNumber{1};

    while (!queue.empty()){
        if (frameNumber== stoul(words[3])){
            stack<Packet*> currentFrame=queue.front();

            auto* physicalPacketPtr = dynamic_cast<PhysicalLayerPacket*>(currentFrame.top());
            currentFrame.pop();
            auto* networkPtr = dynamic_cast<NetworkLayerPacket*>(currentFrame.top());
            currentFrame.pop();
            auto* transportPtr = dynamic_cast<TransportLayerPacket*>(currentFrame.top());
            currentFrame.pop();
            auto* applicationPtr = dynamic_cast<ApplicationLayerPacket*>(currentFrame.top());
            currentFrame.pop();

            std::cout<<"Current Frame #"<<frameNumber<<" on the "<<queueName<<" of client "<<words[1]<<std::endl;
            std::cout<<"Carried Message: \""<<applicationPtr->message_data<<"\""<<std::endl;
            std::cout<<"Layer "<<applicationPtr->layer_ID<<" info: Sender ID: "<<applicationPtr->sender_ID<<", Receiver ID: "<<applicationPtr->receiver_ID<<std::endl;
            std::cout<<"Layer "<<transportPtr->layer_ID<<" info: ";transportPtr->print();
            std::cout<<"Layer "<<networkPtr->layer_ID<<" info: ";networkPtr->print();
            std::cout<<"Layer "<<physicalPacketPtr->layer_ID<<" info: ";physicalPacketPtr->print();
            std::cout<<"Number of hops so far: "<<applicationPtr->numberOfHops<<std::endl;
            return;
        }
        queue.pop();
        frameNumber++;
    }
    std::cout<<"No such frame.\n";
}

void  Network::receiveCommand(vector<Client> &clients){
    for (Client &client:clients){
        if (!client.incoming_queue.empty()){
            string mainReceiverId{};
            string mainSenderId{};

            while (!client.incoming_queue.empty()){
                stack<Packet*> currentFrame=client.incoming_queue.front();
                stack<Packet*> storeFrame;

                auto* physicalPacketPtr = dynamic_cast<PhysicalLayerPacket*>(currentFrame.top());
                string  senderId{findClientFromMac(clients,physicalPacketPtr->sender_MAC_address)->client_id};

                storeFrame.push(currentFrame.top());
                currentFrame.pop();

                auto* networkPtr = dynamic_cast<NetworkLayerPacket*>(currentFrame.top());
                mainReceiverId= findClientIdFromIp(clients,networkPtr->receiver_IP_address);
                mainSenderId= findClientIdFromIp(clients,networkPtr->sender_IP_address);

                if (client.client_id!=mainReceiverId){
                    string nextHopClientId=client.routing_table[mainReceiverId];

                    //Control drop or forward
                    bool controlClientExisting{controlClientExistFromId(clients,nextHopClientId)};
                    //If forward
                    if (controlClientExisting){
                        int frameNumber{};
                        std::cout<<"Client " << client.client_id<<" receiving a message from client "<<senderId<<", but intended for client "<<mainReceiverId<< ". Forwarding... \n";
                        string nextHopClientMacAddress{findRequiredClient(clients,nextHopClientId)->client_mac};

                        physicalPacketPtr->sender_MAC_address=client.client_mac;
                        physicalPacketPtr->receiver_MAC_address=nextHopClientMacAddress;
                        currentFrame.push(storeFrame.top());
                        storeFrame.pop();

                        client.incoming_queue.pop();
                        client.outgoing_queue.push(currentFrame);
                        frameNumber=currentFrame.top()->frameNumber;
                        std::cout<<"Frame #"<<frameNumber<<" MAC address change: New sender MAC "<<client.client_mac<<", new receiver MAC "<<nextHopClientMacAddress<<std::endl;

                        while (!client.incoming_queue.empty()){
                            currentFrame=client.incoming_queue.front();
                            if (frameNumber==currentFrame.top()->frameNumber||frameNumber>currentFrame.top()->frameNumber||client.incoming_queue.empty()){ break;}

                            physicalPacketPtr = dynamic_cast<PhysicalLayerPacket*>(currentFrame.top());
                            physicalPacketPtr->sender_MAC_address=client.client_mac;
                            physicalPacketPtr->receiver_MAC_address=nextHopClientMacAddress;

                            client.incoming_queue.pop();
                            client.outgoing_queue.push(currentFrame);
                            frameNumber=currentFrame.top()->frameNumber;
                            std::cout<<"Frame #"<<frameNumber<<" MAC address change: New sender MAC "<<client.client_mac<<", new receiver MAC "<<nextHopClientMacAddress<<std::endl;

                        }
                        std::cout<<"--------"<<std::endl;
                        Log log("2023-11-22 20:30:03" ,"",frameNumber,currentFrame.top()->numberOfHops,mainSenderId,mainReceiverId,true,ActivityType::MESSAGE_FORWARDED);
                        client.log_entries.push_back(log);

                    }
                    //If drop
                    else{
                        int frameNumber{};
                        currentFrame.push(storeFrame.top());
                        storeFrame.pop();
                        int numberOfHops{currentFrame.top()->numberOfHops};

                        while (!client.incoming_queue.empty()){
                            client.incoming_queue.pop();
                            frameNumber=currentFrame.top()->frameNumber;
                            std::cout<<"Client "<<client.client_id<<" receiving frame #"<<frameNumber<<" from client "<<senderId<<", but intended for client "<<mainReceiverId<<". Forwarding...\n";
                            std::cout<<"Error: Unreachable destination. Packets are dropped after "<<currentFrame.top()->numberOfHops<<" hops!\n";

                            while (!currentFrame.empty()){
                                Packet* currentPacket=currentFrame.top();
                                delete currentPacket;
                                currentFrame.pop();
                            }


                            if (!client.incoming_queue.empty()){currentFrame=client.incoming_queue.front();}
                            if (client.incoming_queue.empty()||frameNumber>currentFrame.top()->frameNumber||frameNumber==currentFrame.top()->frameNumber){
                                Log log("2023-11-22 20:30:03" ,"",frameNumber,numberOfHops,mainSenderId,mainReceiverId,false,ActivityType::MESSAGE_DROPPED);
                                client.log_entries.push_back(log);

                                break;}

                        }
                        std::cout<<"--------"<<std::endl;
                    }

                }
                else{

                    currentFrame.push(storeFrame.top());
                    storeFrame.pop();

                    string totalMessage{};

                    while(!client.incoming_queue.empty()){

                        physicalPacketPtr = dynamic_cast<PhysicalLayerPacket*>(currentFrame.top());

                        currentFrame.pop();

                        networkPtr = dynamic_cast<NetworkLayerPacket*>(currentFrame.top());

                        currentFrame.pop();

                        auto* transportPtr = dynamic_cast<TransportLayerPacket*>(currentFrame.top());

                        currentFrame.pop();

                        auto* applicationPtr = dynamic_cast<ApplicationLayerPacket*>(currentFrame.top());

                        currentFrame.pop();

                        std::cout<<"Client "<<mainReceiverId<<" receiving frame #"<<physicalPacketPtr->frameNumber<<" from client "<<senderId<<", originating from client "<<applicationPtr->sender_ID<<std::endl;
                        physicalPacketPtr->print();
                        networkPtr->print();
                        transportPtr->print();
                        applicationPtr->print();
                        std::cout<<"Number of hops so far: "<<physicalPacketPtr->numberOfHops<<std::endl;
                        std::cout<<"--------"<<std::endl;
                        totalMessage+=applicationPtr->message_data;
                        if (totalMessage.substr(totalMessage.length() - 1) == "." ||
                            totalMessage.substr(totalMessage.length() - 1) == "?" ||
                            totalMessage.substr(totalMessage.length() - 1) == "!"){
                            std::cout<<"Client "<<mainReceiverId<<" received the message \""<<totalMessage<<"\" from client "<<applicationPtr->sender_ID<<"."<<std::endl;
                            std::cout<<"--------"<<std::endl;

                            Log log("2023-11-22 20:30:03" ,totalMessage,physicalPacketPtr->frameNumber,applicationPtr->numberOfHops,mainSenderId,mainReceiverId,true,ActivityType::MESSAGE_RECEIVED);
                            client.log_entries.push_back(log);
                            totalMessage.clear();
                        }
                        client.incoming_queue.pop();


                        if (!client.incoming_queue.empty()){currentFrame=client.incoming_queue.front();}


                        delete physicalPacketPtr;
                        delete networkPtr;
                        delete transportPtr;
                        delete applicationPtr;
                    }
                }









            }
        }
    }


}

void Network::sendCommand(vector<Client> &clients){
    for (Client &client:clients) {
        if (!client.outgoing_queue.empty()){
            string  message;
            int numberOfFrame{0};

            while (!client.outgoing_queue.empty()){
                stack<Packet*> currentFrame{client.outgoing_queue.front()};
                stack<Packet*> storeFrame;
                client.outgoing_queue.pop();

                Client* receiverClient;
                string mainReceiver{};


                while (!currentFrame.empty()){
                    if (currentFrame.top()->layer_ID==PHYSICAL_LAYER_ID){
                        auto* physicalPacketPtr = dynamic_cast<PhysicalLayerPacket*>(currentFrame.top());
                        receiverClient= findClientFromMac(clients,physicalPacketPtr->receiver_MAC_address);


                        std::cout<<"Client "<<client.client_id<< " sending frame #"<<currentFrame.top()->frameNumber<< " to client "<<receiverClient->client_id<<std::endl;
                        physicalPacketPtr->print();
                        physicalPacketPtr->numberOfHops+=1;
                        currentFrame.pop();
                        storeFrame.push(physicalPacketPtr);
                    }
                    else if (currentFrame.top()->layer_ID==NETWORK_LAYER_ID){
                        auto* networkPtr = dynamic_cast<NetworkLayerPacket*>(currentFrame.top());
                        networkPtr->print();
                        networkPtr->numberOfHops+=1;
                        currentFrame.pop();
                        storeFrame.push(networkPtr);
                    }
                    else if (currentFrame.top()->layer_ID==TRANSPORT_LAYER_ID){
                        auto* transportPtr = dynamic_cast<TransportLayerPacket*>(currentFrame.top());
                        transportPtr->print();
                        transportPtr->numberOfHops+=1;
                        currentFrame.pop();
                        storeFrame.push(transportPtr);
                    }
                    else if (currentFrame.top()->layer_ID==APPLICATION_LAYER_ID){
                        auto* applicationPtr = dynamic_cast<ApplicationLayerPacket*>(currentFrame.top());
                        applicationPtr->print();
                        applicationPtr->numberOfHops+=1;
                        std::cout<<"Number of hops so far: "<<applicationPtr->numberOfHops<<std::endl<<"--------\n";

                        message+=applicationPtr->message_data;
                        numberOfFrame+=1;
                        mainReceiver=applicationPtr->receiver_ID;
                        currentFrame.pop();
                        storeFrame.push(applicationPtr);
                    }
                }
                if (message.substr(message.length() - 1) == "." ||
                    message.substr(message.length() - 1) == "?" ||
                    message.substr(message.length() - 1) == "!") {
                    if (storeFrame.top()->numberOfHops==1){
                        Log log("2023-11-22 20:30:03" ,message,numberOfFrame,storeFrame.top()->numberOfHops-1,client.client_id,mainReceiver,true,ActivityType::MESSAGE_SENT);
                        client.log_entries.push_back(log);
                    }
                    numberOfFrame=0;
                    message.clear();
                }
                while (!storeFrame.empty()){
                    currentFrame.push(storeFrame.top());
                    storeFrame.pop();
                }
                receiverClient->incoming_queue.push(currentFrame);

            }
        }
    }
}

void Network::messageCommand(vector<string> &words,vector<Client> &clients,int message_limit,
                             const string &sender_port, const string &receiver_port){

    std::string senderId=words[1];
    std::string receiverId=words[2];
    std::string message= words[3];
    int numberOfHops{0};
    std::cout<<"Message to be sent: \""<<message<<"\"\n"<<std::endl;

    Client* sender=findRequiredClient(clients,senderId);
    Client* receiver=findRequiredClient(clients,receiverId);


    std::vector<std::string> chunks;
    size_t length = message.length();
    size_t currentIndex = 0;




    while (currentIndex < length) {
        size_t chunkEnd = std::min(currentIndex + message_limit, length);

        chunks.push_back(message.substr(currentIndex, chunkEnd - currentIndex));
        currentIndex = chunkEnd;
    }

    int  numberOfFrame = 1;
    for(const string& chunk:chunks){
        stack<Packet*> frame;
        string  receiverMacClientId=sender->routing_table[receiver->client_id];

        Client* receiverMacClient= findRequiredClient(clients,receiverMacClientId);
        string  receiverMacID=receiverMacClient->client_mac;

        auto* applicationLayerPacket= new ApplicationLayerPacket(APPLICATION_LAYER_ID,sender->client_id,receiver->client_id,chunk);
        auto* transportLayerPacket=new TransportLayerPacket(TRANSPORT_LAYER_ID,sender_port,receiver_port);
        auto networkLayerPacket=new NetworkLayerPacket(NETWORK_LAYER_ID, sender->client_ip, receiver->client_ip);
        auto* physicalLayerPacket=new PhysicalLayerPacket(PHYSICAL_LAYER_ID,sender->client_mac,receiverMacID);
        applicationLayerPacket->frameNumber=numberOfFrame,transportLayerPacket->frameNumber=numberOfFrame,networkLayerPacket->frameNumber=numberOfFrame,physicalLayerPacket->frameNumber=numberOfFrame;
        applicationLayerPacket->numberOfHops=numberOfHops,transportLayerPacket->numberOfHops=numberOfHops,networkLayerPacket->numberOfHops=numberOfHops,physicalLayerPacket->numberOfHops=numberOfHops;




        std::cout<<"Frame #"<<numberOfFrame<<std::endl;
        physicalLayerPacket->print();
        networkLayerPacket->print();
        transportLayerPacket->print();
        applicationLayerPacket->print();
        std::cout<<"Number of hops so far: "<<numberOfHops<<std::endl<<"--------\n";



        frame.push(applicationLayerPacket);
        frame.push(transportLayerPacket);
        frame.push(networkLayerPacket);
        frame.push(physicalLayerPacket);
        sender->outgoing_queue.push(frame);

        numberOfFrame+=1;
    }

}

Client* Network::findClientFromMac( vector<Client> &clients, const string& macAddress){
    for (auto & client : clients) {
        if (client.client_mac==macAddress){
            return &client; }
    }
    return  &(clients[0]);

}

string Network::findClientIdFromIp( vector<Client> &clients, const string& ipAddress){
    for (size_t i = 0; i < clients.size(); ++i) {
        if (clients[i].client_ip==ipAddress){
            return clients[i].client_id;}
    }
    return "can not find";
}

bool  Network::controlClientExistFromId(vector<Client> &clients,const string& clientToControl){
    for(Client &client:clients){
        if (client.client_id==clientToControl){
            return true;
        }
    }
    return false;

}


Client* Network::findRequiredClient(vector<Client> &clients,const string& clientId){
    for (auto & client : clients) {
        if (client.client_id==clientId){
            return &client; }
    }
    return  &(clients[0]);
}

vector<Client> Network::read_clients(const string &filename) {
    vector<Client> clients;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Cannot open file: " << filename << std::endl;
        return clients;
    }

    std::string line;
    std::getline(file, line);

    while (std::getline(file, line)) {

        std::istringstream iss(line);
        std::string token;
        std::vector<std::string> tokens;


        while (std::getline(iss, token, ' ')) {
            tokens.push_back(token);
        }
        Client client(tokens[0],tokens[1],tokens[2]);
        clients.push_back(client);

    }

    file.close();

    return clients;
}

void Network::read_routing_tables(vector<Client> &clients, const string &filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Cannot open file: " << filename << std::endl;
        return;
    }
    std::string line;
    int index{0};
    while (std::getline(file, line)) {
        while (line != "-") {
            std::vector<std::string> currentData;
            std::istringstream iss(line);
            std::string element;
            while (iss >> element) {
                currentData.push_back(element);

            }
            clients[index].routing_table[currentData[0]]=currentData[1];
            if (!std::getline(file, line)) {
                break;
            }
        }

        index++;

    }
    file.close();
}

vector<string> Network::read_commands(const string &filename) {
    vector<string> commands;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Cannot open file: " << filename << std::endl;
        return commands;
    }
    std::string line;
    std::getline(file, line);
    while (std::getline(file, line)) {
        commands.push_back(line);
    }
    file.close();
    return commands;
}

void Network::splitMessage(std::string command, vector<string> &words){

    std::istringstream iss(command);
    std::string token;

    while (iss >> token) {

        if (!token.empty()) {
            if (token.front() == '#' && token.back() != '#') {

                std::string mergedToken = token;
                while (iss >> token) {
                    mergedToken += ' ' + token;
                    if (token.back() == '#') {
                        break;
                    }
                }
                words.push_back(mergedToken.substr(1, mergedToken.size() - 2));
            } else {
                if (token.front()=='#' && token.back()=='#'){
                    words.push_back(token.substr(1, token.size() - 2));
                }
                else{
                    words.push_back(token);
                }


            }
        }
    }



};

Network::~Network() {

}
