#include <iostream>
#include "Network.h"

using namespace std;

int main(int argc, char *argv[]) {

    // Instantiate HUBBMNET
    Network* HUBBMNET = new Network();
    std::string client = "C:\\Users\\said5\\CLionProjects\\Assignment3\\sampleIO\\1_sample_from_instructions\\clients.dat";
    std::string routing = "C:\\Users\\said5\\CLionProjects\\Assignment3\\sampleIO\\1_sample_from_instructions\\routing.dat";
    std::string commands1 = "C:\\Users\\said5\\CLionProjects\\Assignment3\\sampleIO\\1_sample_from_instructions\\commands.dat";

//    std::string sdasd = "?sdafasşfşasfmlkşsalkflasf";
//    size_t dotFound = (sdasd.find('?'));
//    if(dotFound == std::string::npos){
//        std::cout << "not found " << std::endl;
//    }else{
//        std::cout << dotFound << std::endl;
//    }


    // Read from input files
    vector<Client> clients = HUBBMNET->read_clients(client);
//    for (int i = 0; i < clients.size(); ++i) {
//        std::cout << clients[i].client_id << "  " << clients[i].client_ip << "  " << clients[i].client_mac;
//        std::cout << std::endl;
//
//    }
    HUBBMNET->read_routing_tables(clients, routing);
//    for (const auto& pair : clients[0].routing_table) {
//            std::cout << "Key: " << pair.first << ", Value: " << pair.second << std::endl;
//        }
//    for (int i = 0; i < clients.size()-1; ++i) {
//        for (const auto& pair : clients[i].routing_table) {
//            std::cout << "Key: " << pair.first << ", Value: " << pair.second << std::endl;
//        }
//        std::cout << std::endl;
//    }
    vector<string> commands = HUBBMNET->read_commands(commands1);
//    for (int i = 0; i < commands.size(); ++i) {
//        std::cout << commands[i] << std::endl;
//
//    }
    HUBBMNET->process_commands(clients, commands, 20, "895", "7541");
//
//    // Get additional parameters from the cmd arguments
//    int message_limit = stoi(argv[4]);
//    string sender_port = argv[5];
//    string receiver_port = argv[6];
//
//    // Run the commands
//    HUBBMNET->process_commands(clients, commands, message_limit, sender_port, receiver_port);
//
//    // Delete HUBBMNET
//    delete HUBBMNET;

    return 0;
}


