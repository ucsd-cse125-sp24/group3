#pragma once

// How many bytes are in the buffer for incoming packets.
// NOTE: We think the current size of the buffer will be less
// than this due to some sort of size limit.
#define NETWORK_BUFFER_SIZE 1'000'000

// How many objects we send in one LoadGameState packet
// If there are more objects that need to be sent, then
// they are split up into multiple LoadGameState packets
#define OBJECTS_PER_UPDATE 70 
