/** \file functional.h
 * \author Andrej Leban
 * \date 10/2019
 */

#ifndef CFUNCTIONAL_H
#define CFUNCTIONAL_H

#include <any>
#include <functional>
#include <utility>

namespace cm
{

// A modified implementation of std::accumulate which avoids
// unnecessary copies – whenever it calls the folding function,
// it moves the previously accumulated value into it instead
// of passing a copy

// Copyright I. Čukić, Functional programming in C++
//! \brief A std::accumulate that makes use of move semantics.
//! \param first iterator to first element
//! \param last  iterator to last element
//! \param init  initial value
//! \param folding_function
//! \return the accumulated values
template <typename BeginIt, typename EndIt, typename T, typename F>
T moving_accumulate(BeginIt first, const EndIt & last, T init, F folding_function);

//! \brief Wrapper around std::any for std::functions.
// Adapted from c.f.: https://stackoverflow.com/questions/45715219/store-functions-with-different-signatures-in-a-map/
template <typename Ret>
struct AnyCallable
{
    AnyCallable() = default;

    template <typename F>
    AnyCallable(F && fun);

    template <typename... Args>
    AnyCallable(std::function<Ret(Args...)> fun);

    template <typename... Args>
    Ret operator()(Args &&... args);


    std::any m_any;
};


// Specialization for void callables
template <>
struct AnyCallable<void>
{
    AnyCallable() = default;

    template <typename F>
    AnyCallable(F && fun);

    template <typename... Args>
    AnyCallable(std::function<void(Args...)> fun);

    template <typename ... Args>
    void operator()(Args &&... args);


    std::any m_any;
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


template <typename BeginIt, typename EndIt, typename T, typename F>
T moving_accumulate(BeginIt first, const EndIt & last, T init, F folding_function)
{
    for (; first != last; ++first)
    {
        // When passing the accumulated value to
        // the folding function, move from it.
        // This might make problems for types that
        // do not support assignment to moved-from values,
        // but it will work for most types without problems.
        init = folding_function(std::move(init), *first);
    }

    return init;
}


// AnyCallable
template <typename Ret>
template <typename F>
AnyCallable<Ret>::AnyCallable(F && fun) : AnyCallable(std::function(fun))
{}

template <typename Ret>
template <typename... Args>
AnyCallable<Ret>::AnyCallable(std::function<Ret(Args...)> fun) : m_any(fun)
{}

template <typename Ret>
template <typename... Args>
Ret AnyCallable<Ret>::operator()(Args &&... args)
{
    return std::invoke(std::any_cast<std::function<Ret(Args...)>>(m_any), std::forward<Args>(args)...);
}

template <typename F>
AnyCallable<void>::AnyCallable(F && fun) : AnyCallable(std::function(fun))
{}

template <typename... Args>
AnyCallable<void>::AnyCallable(std::function<void(Args...)> fun)
    : m_any(fun)
{}

template <typename... Args>
void AnyCallable<void>::operator()(Args &&... args)
{
    std::invoke(std::any_cast<std::function<void(Args...)>>(m_any), std::forward<Args>(args)...);
}

} // namespace cm

#endif // CFUNCTIONAL_H
