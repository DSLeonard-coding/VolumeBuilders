#pragma once
//
// Created by Douglas S. Leonard on 6/17/25.  All rights Reserved
// See related files for license, if any is provided.
//

#ifndef PLACEMENTNAMEREGISTRY_HH
#define PLACEMENTNAMEREGISTRY_HH
#include <mutex>
#include <string>
#include <unordered_map>

namespace DLG4::VolumeBuilders::_internals_ {
    class PlacementNameRegistry {
        static std::unordered_map<std::string, int> name_counts_;
        static std::mutex registry_mutex_;

    public
    :
        static int IncrementNameCount(const std::string &base_name) {
            std::lock_guard<std::mutex> lock(registry_mutex_);
            auto retval = ++name_counts_[base_name];
            return retval;
        }

        static int GetNameCount(const std::string &base_name) {
            std::lock_guard<std::mutex> lock(registry_mutex_);
            auto it = name_counts_.find(base_name);
            return (it != name_counts_.end()) ? it->second : 0;
        }
    };
}

#endif //PLACEMENTNAMEREGISTRY_HH
