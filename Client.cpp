//
// Created by alperen on 27.09.2023.
//

#include "Client.h"

Client::Client(string const& _id, string const& _ip, string const& _mac) {
    client_id = _id;
    client_ip = _ip;
    client_mac = _mac;
}

ostream &operator<<(ostream &os, const Client &client) {
    os << "client_id: " << client.client_id << " client_ip: " << client.client_ip << " client_mac: "
       << client.client_mac << endl;
    return os;
}

Client::~Client() {
    while(!incoming_queue.empty()){
        while(!incoming_queue.front().empty()){
            delete incoming_queue.front().top();
            incoming_queue.front().pop();
        }
        incoming_queue.pop();
    }
    while(!outgoing_queue.empty()){
        while(!outgoing_queue.front().empty()){
            delete outgoing_queue.front().top();
            outgoing_queue.front().pop();
        }
        outgoing_queue.pop();
    }
    // TODO: Free any dynamically allocated memory if necessary.
}