#pragma once
#include <string>
#include <sstream>

#include <glm/glm.hpp>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

/**
 * Helper function to easily serialize a packet/events's data into a string
 * to send over the network.
 * 
 * @param obj object that you want to serialize to send across the network.
 * This should not include any header information, it should strictly be a struct
 * for packet data.
 */
template<class Type>
std::string serialize(Type obj) {
    std::ostringstream archive_stream;
    {
        boost::archive::text_oarchive archive(archive_stream);
        archive << obj;
    }
    return archive_stream.str();
}

/**
 * Helper function to easily deserialize a string received over the network into
 * a obj.
 * 
 * @param data String representation of a serialized obj recieved across the network.
 */
template <class Type>
Type deserialize(std::string data) {
    Type parsed_info;
    {
        std::istringstream stream(data);
        boost::archive::text_iarchive archive(stream);
        archive >> parsed_info;
    }
    return parsed_info;
}

namespace  boost {
    namespace serialization {
        /**
         * Function to serialize a glm::vec3 for Boost
         */
        template<class Archive>
        void serialize(Archive & ar, glm::vec3& vec, const unsigned int version)
        {
            ar & vec.x & vec.y & vec.z;
        }
    }
}