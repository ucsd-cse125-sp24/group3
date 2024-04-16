#include "client/clientinfo.hpp"

#include "shared/network/constants.hpp"
#include "shared/network/packet.hpp"
#include "shared/utilities/typedefs.hpp"

ClientInfo::ClientInfo() {

}

EntityID ClientInfo::getEID() {
    return 0; // TODO: 
}

std::string ClientInfo::getName() {
    return ""; // TODO
}