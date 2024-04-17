#pragma once

#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/variant.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

// Helper macro to reduce boilerplate in making boost::serialize-able structs
#define DEF_SERIALIZE \
    friend class boost::serialization::access; \
    template<class Archive> \
    void serialize
