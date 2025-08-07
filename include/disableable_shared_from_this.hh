#pragma once
#include <atomic>
#include <iostream>
#include <memory>
/**
* @file
* Created by @author Douglas S. Leonard on @date 6/23/25.  All rights Reserved
* See related files for license, if any is provided.
*
* The delayed_enable_shared_from_this  class header.
*/


//

#ifndef DELAYEDENABLESHAREDFROMTHIS_HH
#define DELAYEDENABLESHAREDFROMTHIS_HH

namespace DLG4::Utilities {

    /***
 *@class DLG4::Utilities::disableable_shared_from_this
 *@brief  A dis-armable shared_from_this()
* This solves the problem of not being able to call shared_from_this() before a shared pointer is
* attached to the argument.  Factories are not ALWAYS a practical solution.
* This allows a ctor to disable shared with this to call fluent methods non-fluently (they return nullptr) without crashing
* The enable_shared_with_this can then be re-armed  before the last return call.
*
* Just inherit from it as normal and call set_shared_from_this_enabled(false) to disable
* and set_shared_from_this_enabled(true) to re-enable before exit.
*
* Warning... you still have the limitation that you cannot CHAIN calls before the the pointer is set.
*/
    template <typename T>
    class disableable_shared_from_this : public std::enable_shared_from_this<T> {
    protected:
        // Default to true for normal shared_from_this() behavior
        std::atomic<bool> is_enabled_ {true};

        disableable_shared_from_this() = default;
        virtual ~disableable_shared_from_this() = default;

    public:
        // ReSharper disable once CppHidingFunction
        std::shared_ptr<T> shared_from_this() {
            if (is_enabled_) {
                return std::enable_shared_from_this<T>::shared_from_this();
            } else {
                return nullptr;
            }
        }

        std::shared_ptr<const T> shared_from_this() const {
            if (is_enabled_) {
                return std::enable_shared_from_this<T>::shared_from_this();
            } else {
                return nullptr;
            }
        }

        void set_shared_from_this_enabled(bool enabled) {
            is_enabled_ = enabled;
        }

    };

}

#endif //ARMSHAREDFROMTHIS_HH
