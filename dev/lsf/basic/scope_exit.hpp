// File:        scope_exit.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-30 by leoxiang

#pragma once

#include <functional>
#include <utility>
#include <type_traits>
#include "lsf/basic/macro.hpp"
#include "lsf/basic/noncopyable.hpp"
#include "lsf/util/log.hpp"
#include "lsf/util/backtrace.hpp"

namespace lsf {
namespace basic {
namespace detail {

////////////////////////////////////////////////////////////
// ScopeExit
template<typename FuncType>
class ScopeExit : public lsf::basic::NonCopyable {
public:
    // constructor, use std::forward for perfect forwarding
    inline explicit ScopeExit(FuncType&& func) : _func(std::move(func)) {}
    inline explicit ScopeExit(FuncType const& func) : _func(func) {}

    // move constructor, move all content
    inline ScopeExit(ScopeExit&& rhs) : _func(std::move(rhs._func)) {}

    // destructor, if not dismiss then call func
    inline ~ScopeExit() {
        try {
            if (!_is_dismiss) _func();
        }
        catch (std::exception& e) {
            LSF_LOG_FATAL_WITH_STACK("scope throws, %s", e.what());
        }
    }

    inline void Dismiss() { _is_dismiss = true; }

private:
    FuncType _func;
    bool _is_dismiss = false;
};

////////////////////////////////////////////////////////////
// ScopeExitCreator
class ScopeExitCreator {
public:
    template<typename FuncType>
    inline detail::ScopeExit<typename std::decay<FuncType>::type> operator<<(FuncType&& func) {
        return detail::ScopeExit<typename std::decay<FuncType>::type>(std::forward<FuncType>(func));
    }
};

} // end namespace detail

////////////////////////////////////////////////////////////
// Factory Creator
template<typename FuncType>
inline detail::ScopeExit<typename std::decay<FuncType>::type> MakeScopeExit(FuncType&& func) {
    return detail::ScopeExit<typename std::decay<FuncType>::type>(std::forward<FuncType>(func));
}

} // end namespace basic
} // end namespace lsf

#define LSF_SCOPE_EXIT() auto LSF_TOKEN_CAT(scope_exit_, __LINE__) = lsf::basic::detail::ScopeExitCreator() << [&]
#define LSF_SCOPE_EXIT_ARGS(args...) auto LSF_TOKEN_CAT(scope_exit_, __LINE__) = lsf::basic::ScopeExitCreator() << [##args]

// vim:ts=4:sw=4:et:ft=cpp:
