#include "shared/utilities/root_path.hpp"

#include <boost/filesystem.hpp>
#include <boost/dll/runtime_symbol_info.hpp>

boost::filesystem::path getRepoRoot() {
    return boost::dll::program_location().parent_path();
}