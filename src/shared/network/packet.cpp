#include "shared/network/packet.hpp"

#include <cstdint>

namespace packet {

bool validateType(Type type) {
    switch (type) {
        case Type::ServerLobbyBroadcast:
        case Type::ClientDeclareInfo:
        case Type::ServerAssignEID:
        case Type::ServerLobbyInfo:
        case Type::ServerSendGameState:
        case Type::ClientRequestEvent:
        case Type::ServerDoEvent:
            return true;
        default:
            return false;
    }
}

}