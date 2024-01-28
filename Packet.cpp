#include "Packet.h"
#include <iostream>
#define APPLICATION_LAYER_ID O
#define TRANSPORT_LAYER_ID 1
#define NETWORK_LAYER_ID 2
#define PHYSICAL_LAYER_ID 3

Packet::Packet(int _layer_ID) {
    layer_ID = _layer_ID;
}


ostream& operator<<(ostream& os, const Packet& packet) {
    os << "Packet layer: " << packet.layer_ID;
    return os;
}

Packet::~Packet() {
    // TODO: Free any dynamically allocated memory if necessary.
}