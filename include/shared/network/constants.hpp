#pragma once

// These values are intrinsicly linked.
// The buffer size needed is directly related to how large incoming packets are expected to be,
// and the size of incoming packets is mostly determined by the max number of objects that can
// be sent in a packet. I have not heavily tuned these numbers at all. I just kind of picked
// values and these seem to work fine.

// Anecdotaly I think we could lower the buffer size by a bit (or increase objects per update) because 
// I think I was able to send 350 objects in one packet and not overflow the buffer,
// but I'm just leaving it here to be safe because maybe it would break if there were 350 objects that 
// took up more memory than the average object.

// How many bytes are in the buffer for incoming packets
#define NETWORK_BUFFER_SIZE 1'000'000

// How many objects we send in one LoadGameState packet
// If there are more objects that need to be sent, then
// they are split up into multiple LoadGameState packets
#define OBJECTS_PER_UPDATE 100
