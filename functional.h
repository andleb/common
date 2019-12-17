#ifndef CFUNCTIONAL_H
#define CFUNCTIONAL_H

#include <utility>

namespace cm {

// A modified implementation of std::accumulate which avoids
// unnecessary copies – whenever it calls the folding function,
// it moves the previously accumulated value into it instead
// of passing a copy

// Copyright I. Čukić, Functional programming in C++
template <typename BeginIt, typename EndIt, typename T, typename F>
T moving_accumulate(BeginIt first, const EndIt& last, T init,
                    F folding_function)
{
    for (; first != last; ++first) {
        // When passing the accumulated value to
        // the folding function, move from it.
        // This might make problems for types that
        // do not support assignment to moved-from values,
        // but it will work for most types without problems.
        init = folding_function(std::move(init), *first);
    }

    return init;
}


} // namespace cm

#endif // CFUNCTIONAL_H
