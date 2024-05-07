#pragma once

#include <boost/filesystem.hpp>

/**
 * Helper function to get a filepath of the root repo, useful for loading in files
 */
boost::filesystem::path getRepoRoot();