#include "Network.h"
#include <fstream>

Network::Network() {

}

void Network::process_commands(vector<Client> &clients, vector<string> &commands, int message_limit,
                      const string &sender_port, const string &receiver_port) {
    std::vector<Client*> messageSR;
    for (int i = 1; i < commands.size(); ++i) {
        if(commands[i].size() == 4 && commands[i] == "SEND"){
            std::cout<< "-------------" << std::endl << "Command: " << commands[i] << std::endl;
            std::cout << "-------------" << std::endl;
            for (int j = 0; j < clients.size(); ++j) {
                if(!clients[j].outgoing_queue.empty()){
                    std::vector<vector<Packet*>> forPrint;
                    std::queue<std::stack<Packet*>> test(clients[j].outgoing_queue);
                    Client* receiver;
                    std::string receiverId;
                    int frame = 1;
                    while(!test.empty()) {
                        std::stack<Packet*> packetTest = test.front();
                        std::stack<Packet*> originalPacket = clients[j].outgoing_queue.front();
                        std::stack<Packet*> printPacket(originalPacket);
                        for (int l = 0; l < 3; ++l) {
                            packetTest.pop();
                        }
                        size_t dotFound = dynamic_cast<ApplicationLayerPacket*>(packetTest.top())->message_data.find('.');
                        size_t qMarkFound = dynamic_cast<ApplicationLayerPacket*>(packetTest.top())->message_data.find('?');
                        size_t eMarkFound = dynamic_cast<ApplicationLayerPacket*>(packetTest.top())->message_data.find('!');
                        Client* tempRec;
                        if(frame == 1){
                            std::string temporaryReceiverMac = dynamic_cast<PhysicalLayerPacket*>(originalPacket.top())->receiver_MAC_address;
                            for (int k = 0; k < clients.size(); ++k) {
                                if(clients[k].client_mac == temporaryReceiverMac){
                                    tempRec = &clients[k];
                                }
                            }
                        }
                        int hopCount =0;
                        Client* originalS;
                        Client* originalR;
                        std::string originalSender = dynamic_cast<ApplicationLayerPacket*>(packetTest.top())->sender_ID;
                        std::string originalReceiver =dynamic_cast<ApplicationLayerPacket*>(packetTest.top())->receiver_ID;
                        for (int k = 0; k < clients.size(); ++k) {
                            if(clients[k].client_id == originalSender){
                                originalS = &clients[k];
                            }
                            if(clients[k].client_id == originalReceiver){
                                originalR = &clients[k];
                            }
                        }

                        std::string key;
                        Client* keyClient;
                        std::cout <<"Client "<< clients[j].client_id <<  " sending frame #" << frame << " to client " <<tempRec->client_id << std::endl;
                        while(!printPacket.empty()){
                            printPacket.top()->print();
                            printPacket.pop();
                        }
                        while(key != tempRec->client_id){
                            key = originalS->routing_table[originalReceiver];
                            bool paired = false;
                            for (int k = 0; k < clients.size(); ++k) {
                                if(clients[k].client_id == key){
                                    originalS = &clients[k];
                                    paired = true;
                                }
                            }
                            if(!paired){
                                break;
                            }
                            hopCount++;
                        }
                        std::cout << "Number of hops so far: " << hopCount << std::endl;
                        std::cout << "--------" << std::endl;

                        tempRec->incoming_queue.push(originalPacket);
                        frame++;
                        if(dotFound != std::string::npos || qMarkFound != std::string::npos || eMarkFound != std::string::npos){
                            frame = 1;
                        }
                        test.pop();
                        clients[j].outgoing_queue.pop();
                    }
                }
            }

        }
        else if(commands[i].size() == 7 && commands[i] == "RECEIVE"){
            std::time_t currentTime = std::time(nullptr);
            std::tm* timeInfo = std::localtime(&currentTime);
            char buffer[80];
            std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeInfo);
            std::cout << "----------------" << std::endl << "Command: " << commands[i] << std::endl << "----------------" << std::endl;

            for (int j = 0; j < clients.size(); ++j) {
                if(!clients[j].incoming_queue.empty()){
                    std::string fullMessage;
                    std::queue<std::stack<Packet*>> test(clients[j].incoming_queue);
                    std::string senderId;
                    std::string receiverId;
                    int frame = 1;
                    while(!test.empty()) {
                        std::stack<Packet*> packetTest = test.front();
                        std::stack<Packet*> originalPacket = clients[j].incoming_queue.front();
                        for (int l = 0; l < 3; ++l) {
                            packetTest.pop();
                        }
                        size_t dotFound = dynamic_cast<ApplicationLayerPacket*>(packetTest.top())->message_data.find('.');
                        size_t qMarkFound = dynamic_cast<ApplicationLayerPacket*>(packetTest.top())->message_data.find('?');
                        size_t eMarkFound = dynamic_cast<ApplicationLayerPacket*>(packetTest.top())->message_data.find('!');
                        Client* tempRec;
                        if(frame == 1){
                            senderId = dynamic_cast<ApplicationLayerPacket*>(packetTest.top())->sender_ID;
                            receiverId = dynamic_cast<ApplicationLayerPacket*>(packetTest.top())->receiver_ID;
                            std::string temporaryReceiverMac = dynamic_cast<PhysicalLayerPacket*>(originalPacket.top())->sender_MAC_address;
                            for (int k = 0; k < clients.size(); ++k) {
                                if(clients[k].client_mac == temporaryReceiverMac){
                                    tempRec = &clients[k];
                                }
                            }

                        }
                        bool canForward = false;
                        for (int k = 0; k < clients.size(); ++k) {
                            if(clients[k].client_id == clients[j].routing_table[receiverId] || clients[j].client_id == receiverId){
                                canForward = true;
                                break;
                            }
                        }
                        if(!canForward){
                            int hopCount =0;
                            int stopper = 0;
                            Client* originalS;
                            Client* originalR;
                            std::string originalSender = dynamic_cast<ApplicationLayerPacket*>(packetTest.top())->sender_ID;
                            std::string originalReceiver =dynamic_cast<ApplicationLayerPacket*>(packetTest.top())->receiver_ID;
                            for (int k = 0; k < clients.size(); ++k) {
                                if(clients[k].client_id == originalSender){
                                    originalS = &clients[k];
                                }
                                if(clients[k].client_id == originalReceiver){
                                    originalR = &clients[k];
                                }
                            }
                            std::string key;
                            Client* keyClient;
                            while(key != clients[j].client_id && stopper <25){
                                key = originalS->routing_table[originalReceiver];
                                bool paired=false;
                                for (int k = 0; k < clients.size(); ++k) {
                                    if(clients[k].client_id == key){
                                        originalS = &clients[k];
                                        paired=true;
                                    }
                                }
                                if(!paired){
                                    break;
                                }
//                                    stopper++;
                                hopCount++;
                            }
                            std::cout << "Client " << clients[j].client_id << " receiving frame #" << frame << " from client " << tempRec->client_id << ", but intended for client " << receiverId << ". Forwarding... " <<std::endl;
                            std::cout << "Error: Unreachable destination. Packets are dropped after "<< hopCount << " hops!" << std::endl;
                            if(dotFound != std::string::npos || qMarkFound != std::string::npos || eMarkFound != std::string::npos){
                                std::cout << "--------" << std::endl;

                                clients[j].log_entries.push_back(Log(buffer, "", frame, hopCount,senderId , receiverId , false , ActivityType::MESSAGE_DROPPED));
                            }
                        }else{
                            if(clients[j].client_id != receiverId){
                                if(frame ==1){
                                    std::cout << "Client " << clients[j].client_id << " receiving a message from client " << tempRec->client_id << ", but intended for client " << receiverId << ". Forwarding..." <<std::endl;
                                }
                                dynamic_cast<PhysicalLayerPacket*>(clients[j].incoming_queue.front().top())->sender_MAC_address = clients[j].client_mac;
                                Client* temporaryRec;
                                for (int k = 0; k < clients.size(); ++k) {
                                    if(clients[k].client_id == clients[j].routing_table[receiverId]){
                                        temporaryRec = &clients[k];
                                    }
                                }
                                dynamic_cast<PhysicalLayerPacket*>(clients[j].incoming_queue.front().top())->receiver_MAC_address = temporaryRec->client_mac;
                                std::cout << "Frame #" << frame << " MAC address change: New sender MAC " << clients[j].client_mac << ", new receiver MAC " << temporaryRec->client_mac << std::endl;
                                clients[j].outgoing_queue.push(originalPacket);
                                int hopCount =0;
                                int stopper =0;
                                Client* originalS;
                                Client* originalR;
                                std::string originalSender = dynamic_cast<ApplicationLayerPacket*>(packetTest.top())->sender_ID;
                                std::string originalReceiver =dynamic_cast<ApplicationLayerPacket*>(packetTest.top())->receiver_ID;
                                for (int k = 0; k < clients.size(); ++k) {
                                    if(clients[k].client_id == originalSender){
                                        originalS = &clients[k];
                                    }
                                    if(clients[k].client_id == originalReceiver){
                                        originalR = &clients[k];
                                    }
                                }
                                std::string key;
                                Client* keyClient;
                                while(key != clients[j].client_id && stopper <25){
                                    key = originalS->routing_table[originalReceiver];
                                    bool paired=false;
                                    for (int k = 0; k < clients.size(); ++k) {
                                        if(clients[k].client_id == key){
                                            originalS = &clients[k];
                                            paired = true;
                                        }
                                    }
                                    if(!paired){
                                        break;
                                    }
                                    hopCount++;
                                }
                                if(dotFound != std::string::npos || qMarkFound != std::string::npos || eMarkFound != std::string::npos){
                                    std::cout << "--------" << std::endl;
                                    clients[j].log_entries.push_back(Log(buffer, "", frame, hopCount,senderId , receiverId , true , ActivityType::MESSAGE_FORWARDED));
                                }


                            }else if(clients[j].client_id == receiverId){
                                std::cout << "Client " << receiverId << " receiving frame #" << frame << " from client " << tempRec->client_id << ", originating from client " << senderId << std::endl;
                                for (int k = 0; k < 3; ++k) {
                                    originalPacket.top()->print();
                                    originalPacket.pop();
                                }
                                Client* originalS;
                                for (int k = 0; k < clients.size(); ++k) {
                                    if(clients[k].client_id == dynamic_cast<ApplicationLayerPacket*>(originalPacket.top())->sender_ID){
                                        originalS = &clients[k];
                                    }
                                }
                                fullMessage += dynamic_cast<ApplicationLayerPacket*>(originalPacket.top())->message_data ;
                                originalPacket.top()->print();
                                originalPacket.pop();
                                int hopCount =0;
                                int stopper =0;
                                Client* originalS1;
                                Client* originalR;
                                std::string originalSender = dynamic_cast<ApplicationLayerPacket*>(packetTest.top())->sender_ID;
                                std::string originalReceiver =dynamic_cast<ApplicationLayerPacket*>(packetTest.top())->receiver_ID;
                                for (int k = 0; k < clients.size(); ++k) {
                                    if(clients[k].client_id == originalSender){
                                        originalS1 = &clients[k];
                                    }
                                    if(clients[k].client_id == originalReceiver){
                                        originalR = &clients[k];
                                    }
                                }
                                std::string key;
                                Client* keyClient;
                                while(key != clients[j].client_id && stopper <25){
                                    key = originalS1->routing_table[originalReceiver];
                                    bool paired = false;
                                    for (int k = 0; k < clients.size(); ++k) {
                                        if(clients[k].client_id == key){
                                            originalS1 = &clients[k];
                                            paired=true;
                                        }
                                    }
                                    if(!paired){
                                        break;
                                    }
//                                        stopper++;
                                    hopCount++;
                                }
                                std::cout << "Number of hops so far: " << hopCount << std::endl;
                                std::cout << "--------" << std::endl;
                                if(dotFound != std::string::npos || qMarkFound != std::string::npos || eMarkFound != std::string::npos){
                                    std::cout <<"Client " << receiverId << " received the message \"" << fullMessage << "\" from client " << senderId <<"." <<std::endl;
                                    std::cout << "--------" << std::endl;
                                    clients[j].log_entries.push_back(Log(buffer ,fullMessage, frame, hopCount, senderId , receiverId , true , ActivityType::MESSAGE_RECEIVED));
                                    fullMessage = "";

                                }

                            }

                        }
                        frame++;
                        if(dotFound != std::string::npos || qMarkFound != std::string::npos || eMarkFound != std::string::npos){
                            frame = 1;
//                            std::cout << "--------" << std::endl;
                        }
                        test.pop();
                        clients[j].incoming_queue.pop();
                    }

                }

            }




        }
        else if(commands[i].size() > 7 && commands[i].substr(0,7) == "MESSAGE"){
            Client* sender;
            Client* receiver;
            std::string temporaryReceiverId;
            Client* temporaryReceiver;
            std::vector<int> indexVector;
            for (int j = 8; j < commands[i].size(); ++j) {
                if(commands[i][j] == ' ' && indexVector.size() <= 2){
                    indexVector.push_back(j);
                }
            }
            for (int j = 0; j < clients.size(); ++j) {
                if(commands[i].substr(8,indexVector[0]-8) == clients[j].client_id){
                    sender = &clients[j];
                }
                if(commands[i].substr(indexVector[0]+1,indexVector[1]-indexVector[0]-1) == clients[j].client_id){
                    receiver = &clients[j];
                }
            }
            temporaryReceiverId = sender->routing_table[receiver->client_id];
            for (int j = 0; j < clients.size(); ++j) {
                if(clients[j].client_id == temporaryReceiverId){
                    temporaryReceiver = &clients[j];
                }
            }

            int starting = 0;
            int ending = 0;
            for (int j = 0; j < commands[i].size(); ++j) {
                if(starting == 0 && commands[i][j] == '#'){
                    starting = j+1;
                }
                if(j > starting && commands[i][j] == '#' ){
                    ending = j;
                }
            }
            std::string multipliedSymbol = "-";
            for (int j = 0; j < commands[i].size() +8; ++j) {
                multipliedSymbol += "-";
            }
            std::cout<< multipliedSymbol << std::endl << "Command: " << commands[i] << std::endl;
            std::cout << multipliedSymbol << std::endl;

            std::string fullMessage = commands[i].substr(indexVector[1] + 2);
            fullMessage.erase(fullMessage.size()-1);
//            std::string fullMessage = commands[i].substr(starting, (ending - starting));
            std::cout << "Message to be sent: \"" << fullMessage << "\"" << std::endl << std::endl;

            std::vector<std::string> stringHolder;
            int splitCount = fullMessage.size() / message_limit;
            if(fullMessage.size() > message_limit){
                int remainder =0;
                if(fullMessage.size() % message_limit != 0){
                    remainder = fullMessage.size() % message_limit;
                    splitCount++;
                }
                int splitter = 0;
                for (int j = 0; j < splitCount-1; ++j) {
                    stringHolder.push_back(fullMessage.substr(splitter,message_limit));
                    splitter += message_limit;
                }
                if(remainder != 0 ){
                    stringHolder.push_back(fullMessage.substr(splitter, remainder));
                }else{
                    stringHolder.push_back(fullMessage.substr(splitter));
                }
            }else{
                stringHolder.push_back(fullMessage);
                if(fullMessage.size() < message_limit){
                    splitCount++;
                }
            }
            std::queue<std::stack<Packet*>> test1;

            for (int j = 0; j < stringHolder.size(); ++j) {
                PhysicalLayerPacket* layerPacket = new PhysicalLayerPacket(3 , sender->client_mac ,temporaryReceiver->client_mac);
                NetworkLayerPacket* networkLayerPacket = new NetworkLayerPacket(2,sender->client_ip, receiver->client_ip);
                TransportLayerPacket* transportLayerPacket = new TransportLayerPacket(1,sender_port,receiver_port);
                ApplicationLayerPacket* applicationLayerPacket = new ApplicationLayerPacket(0, sender->client_id,receiver->client_id, stringHolder[j]);

                std::stack<Packet*> stack;
                stack.push(applicationLayerPacket);
                stack.push(transportLayerPacket);
                stack.push(networkLayerPacket);
                stack.push(layerPacket);


                test1.push(stack);

                sender->outgoing_queue.push(stack);
            }

            std::time_t currentTime = std::time(nullptr);
            std::tm* timeInfo = std::localtime(&currentTime);
            char buffer[80];
            std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeInfo);


            Log log1(buffer , fullMessage ,splitCount , 0 ,sender->client_id , receiver->client_id , true ,ActivityType::MESSAGE_SENT);
            log1.temporaryClientId = temporaryReceiver->client_id;
            sender->log_entries.push_back(log1);

            int frame = 1;
            while(!test1.empty()) {
                std::stack<Packet*> packetter = test1.front();
                std::cout << "Frame #" << frame << std::endl;
                for (int k = 0; k < 4; ++k) {
                    Packet* packet = packetter.top();
                    packet->print();
                    packetter.pop();
                }
                std::cout << "Number of hops so far: 0" << std::endl;
                test1.pop();
                std::cout << "--------" << std::endl;
                frame++;
            }
//            std::cout << sender->outgoing_queue.size() << std::endl;
        }
        else if(commands[i].size() > 15 && commands[i].substr(0,15) == "SHOW_FRAME_INFO" ){
            std::string multipliedSymbol = "-";
            for (int j = 0; j < commands[i].size() +8; ++j) {
                multipliedSymbol += "-";
            }
            std::cout<< multipliedSymbol << std::endl << "Command: " << commands[i] << std::endl;
            std::cout << multipliedSymbol << std::endl;
            std::vector<int > spaceIndex;
            for (int j = 16; j <commands[i].size() ; ++j) {
                if(commands[i][j] == ' '){
                    spaceIndex.push_back(j);
                }
            }
            Client* client;
            std::string clientId = commands[i].substr(16,spaceIndex[0]-16);
            std::string place = commands[i].substr(spaceIndex[0]+1 , spaceIndex[1] - spaceIndex[0]-1);
            int index = std::stoi(commands[i].substr(spaceIndex[1]+1));
            for (int j = 0; j < clients.size(); ++j) {
                if(clients[j].client_id == clientId){
                    client = &clients[j];
                }
            }
            if(place == "out"){
                std::queue<std::stack<Packet*>> printQueue(client->outgoing_queue);
                if(index > 0 && printQueue.size() >= index){
                    for (int j = 0; j < index-1; ++j) {
                        printQueue.pop();
                    }
                    std::cout << "Current Frame #" << index << " on the outgoing queue of client " << client->client_id << std::endl;
                    std::vector<Packet*> packetVector;
                    while(!printQueue.front().empty()){
                        packetVector.push_back(printQueue.front().top());
                        printQueue.front().pop();
                    }
                    int hopCount =0;
                    int stopper =0;
                    Client* originalS;
                    Client* originalR;
                    std::string originalSender = dynamic_cast<ApplicationLayerPacket*>(packetVector[3])->sender_ID;
                    std::string originalReceiver =dynamic_cast<ApplicationLayerPacket*>(packetVector[3])->receiver_ID;
                    for (int k = 0; k < clients.size(); ++k) {
                        if(clients[k].client_id == originalSender){
                            originalS = &clients[k];
                        }
                        if(clients[k].client_id == originalReceiver){
                            originalR = &clients[k];
                        }
                    }
                    std::string key;
                    Client* keyClient;
                    while(key != client->client_id && client->client_id != originalSender){
                        key = originalS->routing_table[originalReceiver];
                        bool paired=false;
                        for (int k = 0; k < clients.size(); ++k) {
                            if(clients[k].client_id == key){
                                originalS = &clients[k];
                                paired = true;
                            }
                        }
                        if(!paired){
                            break;
                        }
                        hopCount++;
                    }




                    std::cout << "Carried Message: \"" <<dynamic_cast<ApplicationLayerPacket*>(packetVector[3])->message_data << "\"" << std::endl;

                    std::cout << "Layer 0 info: Sender ID: " << dynamic_cast<ApplicationLayerPacket*>(packetVector[3])->sender_ID
                    <<", Receiver ID: " << dynamic_cast<ApplicationLayerPacket*>(packetVector[3])->receiver_ID << std::endl;

                    std::cout << "Layer 1 info: Sender port number: " << dynamic_cast<TransportLayerPacket*>(packetVector[2])->sender_port_number
                              <<", Receiver port number: " << dynamic_cast<TransportLayerPacket*>(packetVector[2])->receiver_port_number << std::endl;

                    std::cout << "Layer 2 info: Sender IP address: " << dynamic_cast<NetworkLayerPacket*>(packetVector[1])->sender_IP_address
                              <<", Receiver IP address: " << dynamic_cast<NetworkLayerPacket*>(packetVector[1])->receiver_IP_address << std::endl;

                    std::cout << "Layer 3 info: Sender MAC address: " << dynamic_cast<PhysicalLayerPacket*>(packetVector[0])->sender_MAC_address
                              <<", Receiver MAC address: " << dynamic_cast<PhysicalLayerPacket*>(packetVector[0])->receiver_MAC_address << std::endl;

                    std::cout << "Number of hops so far: " << hopCount << std::endl;
                }else{
                    std::cout << "No such frame." << std::endl;
                }

            }else if(place == "in"){
                std::queue<std::stack<Packet*>> printQueue(client->incoming_queue);
                if(index > 0 && printQueue.size() >= index){
                    for (int j = 0; j < index-1; ++j) {
                        printQueue.pop();
                    }
                    std::cout << "Current Frame #" << index << " on the incoming queue of client " << client->client_id << std::endl;
                    std::vector<Packet*> packetVector;
                    while(!printQueue.front().empty()){
                        packetVector.push_back(printQueue.front().top());
                        printQueue.front().pop();
                    }
                    int hopCount =0;
                    int stopper =0;
                    Client* originalS;
                    Client* originalR;
                    std::string originalSender = dynamic_cast<ApplicationLayerPacket*>(packetVector[3])->sender_ID;
                    std::string originalReceiver =dynamic_cast<ApplicationLayerPacket*>(packetVector[3])->receiver_ID;
                    for (int k = 0; k < clients.size(); ++k) {
                        if(clients[k].client_id == originalSender){
                            originalS = &clients[k];
                        }
                        if(clients[k].client_id == originalReceiver){
                            originalR = &clients[k];
                        }
                    }
                    std::string key;
                    Client* keyClient;
                    while(key != client->client_id && stopper <25){
                        key = originalS->routing_table[originalReceiver];
                        bool paired=false;
                        for (int k = 0; k < clients.size(); ++k) {
                            if(clients[k].client_id == key){
                                originalS = &clients[k];
                                paired = true;
                            }
                        }
                        if(!paired){
                            break;
                        }
                        hopCount++;
                    }



                    std::cout << "Carried Message: \"" <<dynamic_cast<ApplicationLayerPacket*>(packetVector[3])->message_data << "\"" << std::endl;

                    std::cout << "Layer 0 info: Sender ID: " << dynamic_cast<ApplicationLayerPacket*>(packetVector[3])->sender_ID
                              <<", Receiver ID: " << dynamic_cast<ApplicationLayerPacket*>(packetVector[3])->receiver_ID << std::endl;

                    std::cout << "Layer 1 info: Sender port number: " << dynamic_cast<TransportLayerPacket*>(packetVector[2])->sender_port_number
                              <<", Receiver port number: " << dynamic_cast<TransportLayerPacket*>(packetVector[2])->receiver_port_number << std::endl;

                    std::cout << "Layer 2 info: Sender IP address: " << dynamic_cast<NetworkLayerPacket*>(packetVector[1])->sender_IP_address
                              <<", Receiver IP address: " << dynamic_cast<NetworkLayerPacket*>(packetVector[1])->receiver_IP_address << std::endl;

                    std::cout << "Layer 3 info: Sender MAC address: " << dynamic_cast<PhysicalLayerPacket*>(packetVector[0])->sender_MAC_address
                              <<", Receiver MAC address: " << dynamic_cast<PhysicalLayerPacket*>(packetVector[0])->receiver_MAC_address << std::endl;
                    std::cout << "Number of hops so far: " << hopCount << std::endl;
                }else{
                    std::cout << "No such frame." << std::endl;
                }
            }
        }
        else if(commands[i].size() > 11 && commands[i].substr(0,11) == "SHOW_Q_INFO" ){
            std::string multipliedSymbol = "-";
            for (int j = 0; j < commands[i].size() +8; ++j) {
                multipliedSymbol += "-";
            }
            std::cout<< multipliedSymbol << std::endl << "Command: " << commands[i] << std::endl;
            std::cout << multipliedSymbol << std::endl;
            std::vector<int> spaceIndex;
            for (int j = 12; j <commands[i].size() ; ++j) {
                if(commands[i][j] == ' '){
                    spaceIndex.push_back(j);
                }
            }
            Client* client;
            std::string clientId = commands[i].substr(12,spaceIndex[0]-12);
            std::string place = commands[i].substr(spaceIndex[0]+1 );
            for (int j = 0; j < clients.size(); ++j) {
                if(clients[j].client_id == clientId){
                    client = &clients[j];}
            }
            if(place == "in"){
                std::cout << "Client " << client->client_id << " Incoming Queue Status" << std::endl;
                std::cout << "Current total number of frames: " << client->incoming_queue.size() << std::endl;
            }else if(place == "out"){
                std::cout << "Client " << client->client_id << " Outgoing Queue Status" << std::endl;
                std::cout << "Current total number of frames: " << client->outgoing_queue.size() << std::endl;
            }

        }
        else if(commands[i].size() > 9 && commands[i].substr(0,9) == "PRINT_LOG" ){
            std::string multipliedSymbol = "-";
            for (int j = 0; j < commands[i].size() +8; ++j) {
                multipliedSymbol += "-";
            }
            std::cout<< multipliedSymbol << std::endl << "Command: " << commands[i] << std::endl;
            std::cout << multipliedSymbol << std::endl;
            std::string clientId = commands[i].substr(10);
            Client* client;
            for (int j = 0; j < clients.size(); ++j) {
                if(clients[j].client_id == clientId){
                    client = &clients[j];
                }
            }
            if(!client->log_entries.empty()){
                std::cout << "Client " << client->client_id <<" Logs:" << std::endl << "--------------" << std::endl;

            }
            for (int j = 0; j < client->log_entries.size(); ++j) {
                std::cout << "Log Entry #" << j+1 <<":" << std::endl;
                if(client->log_entries[j].activity_type == ActivityType::MESSAGE_SENT){
                    std::cout << "Activity: Message Sent" << std::endl << "Timestamp: "<<client->log_entries[j].timestamp
                    << std::endl<< "Number of frames: " << client->log_entries[j].number_of_frames
                    << std::endl << "Number of hops: " << client->log_entries[j].number_of_hops
                    << std::endl << "Sender ID: " <<  client->log_entries[j].sender_id
                    << std::endl << "Receiver ID: " << client->log_entries[j].receiver_id
                    << std::endl << "Success: Yes" << std::endl << "Message: \"" << client->log_entries[j].message_content << "\""
                    << std::endl;
                }
                else if(client->log_entries[j].activity_type == ActivityType::MESSAGE_FORWARDED){
                    std::cout << "Activity: Message Forwarded" << std::endl << "Timestamp: "<<client->log_entries[j].timestamp
                              << std::endl<< "Number of frames: " << client->log_entries[j].number_of_frames
                              << std::endl << "Number of hops: " << client->log_entries[j].number_of_hops
                              << std::endl << "Sender ID: " <<  client->log_entries[j].sender_id
                              << std::endl << "Receiver ID: " << client->log_entries[j].receiver_id
                              << std::endl << "Success: Yes" << std::endl;
                }
                else if(client->log_entries[j].activity_type == ActivityType::MESSAGE_RECEIVED){
                    std::cout << "Activity: Message Received" << std::endl << "Timestamp: "<<client->log_entries[j].timestamp
                              << std::endl<< "Number of frames: " << client->log_entries[j].number_of_frames
                              << std::endl << "Number of hops: " << client->log_entries[j].number_of_hops
                              << std::endl << "Sender ID: " <<  client->log_entries[j].sender_id
                              << std::endl << "Receiver ID: " << client->log_entries[j].receiver_id
                              << std::endl << "Success: Yes" << std::endl << "Message: \"" << client->log_entries[j].message_content << "\""
                              << std::endl;
                }
                else if(client->log_entries[j].activity_type == ActivityType::MESSAGE_DROPPED){
                    std::cout << "Activity: Message Dropped" << std::endl << "Timestamp: "<<client->log_entries[j].timestamp
                              << std::endl<< "Number of frames: " << client->log_entries[j].number_of_frames
                              << std::endl << "Number of hops: " << client->log_entries[j].number_of_hops
                              << std::endl << "Sender ID: " <<  client->log_entries[j].sender_id
                              << std::endl << "Receiver ID: " << client->log_entries[j].receiver_id
                              << std::endl << "Success: No" << std::endl;
                }
                if(j != client->log_entries.size()-1){
                    std::cout << "--------------" << std::endl;
                }
            }
        }
        else{

            std::string multipliedSymbol = "-";
            for (int j = 0; j < commands[i].size() +8; ++j) {
                multipliedSymbol += "-";
            }
            std::cout<< multipliedSymbol << std::endl << "Command: " << commands[i] << std::endl;
            std::cout << multipliedSymbol <<  std::endl << "Invalid command." << std::endl;


        }

    }


    // TODO: Execute the commands given as a vector of strings while utilizing the remaining arguments.
    /* Don't use any static variables, assume this method will be called over and over during testing.
     Don't forget to update the necessary member variables after processing each command. For example,
     after the MESSAGE command, the outgoing queue of the sender must have the expected frames ready to send. */
}

vector<Client> Network::read_clients(const string &filename) {
    vector<Client> clients;
    std::ifstream clientFile(filename);
    std::string  line = "";
    if(clientFile.is_open()){
        std::string firstLine;
        std::getline(clientFile, firstLine);
        while(std::getline(clientFile,line)){
            if(!line.empty()){
                int arr[2];
                int start =0;
                for (int j = 0; j < line.size()-1; ++j) {
                    if(line[j] == ' '){
                        arr[start] = j;
                        start++;
                    }
                }
                std::string clientName = line.substr(0 , arr[0]);
                std::string clientIp = line.substr(arr[0]+1 , arr[1]-2);
                std::string clientMac = line.substr(arr[1] + 1 );
            Client client(clientName , clientIp, clientMac);
            clients.push_back(client);
            }
        }
        clientFile.close();
    }

    // TODO: Read clients from the given input file and return a vector of Client instances.
    return clients;
}

void Network::read_routing_tables(vector<Client> &clients, const string &filename) {
    std::ifstream routingFile(filename);
    std::string line = "";
    if(routingFile.is_open()){
        int clientIndex = 0;
        while(std::getline(routingFile,line)){
            if(line == "-" ){
                clientIndex++;
            }else{
                int seperator = 0;
                for (int j = 0; j < line.size()-1; ++j) {
                    if(line[j] == ' '){
                        seperator = j;
                        break;}
                }
                clients[clientIndex].routing_table[line.substr(0,seperator)] = line.substr(seperator+1);
            }
        }
        routingFile.close();
    }

    // TODO: Read the routing tables from the given input file and populate the clients' routing_table member variable.
}

// Returns a list of token lists for each command
vector<string> Network::read_commands(const string &filename) {
    vector<string> commands;
    std::ifstream commandFile(filename);
    std::string line = "";
    if(commandFile.is_open()){
        while (std::getline(commandFile , line)){
            commands.push_back(line);
        }
        commandFile.close();


    }
    // TODO: Read commands from the given input file and return them as a vector of strings.
    return commands;
}

Network::~Network() {
    // TODO: Free any dynamically allocated memory if necessary.
}
