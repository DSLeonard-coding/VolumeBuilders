//
// Created by Douglas S. Leonard on 6/17/25.  All rights Reserved
// See related files for license, if any is provided.
//

#include "PlacementNameRegistry.hh"

#include <mutex>
#include <string>
#include <unordered_map>

namespace DLG4 {
namespace VolumeBuilders {
    // Define the static members with proper class scope
    std::unordered_map<std::string, int> PlacementNameRegistry::name_counts_;
    std::mutex PlacementNameRegistry::registry_mutex_;

} // VolumeBuilders
} // DLG4