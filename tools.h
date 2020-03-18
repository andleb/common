/** \file tools.h
 * \author Andrej Leban
 * \date 9/2019
 *
 * Various development tools. Larger subsections might get spun off into separate files.
 */

#ifndef TOOLS_H
#define TOOLS_H

#include <iostream>

namespace cm
{

//! @name Design Pattern helpers.
//!@{

//! \brief A base class for the implementation of the Singleton pattern via public inheritance.
template <typename T>
class Singleton
{
public:
    Singleton(const Singleton &) = delete;
    Singleton(Singleton &&) = delete;
    Singleton & operator=(const Singleton &) = delete;
    Singleton & operator=(Singleton &&) = delete;

    //! \brief The Singleton instance is accessed through this.
    static T & instance();

protected:
    Singleton() = default;
    ~Singleton() = default;
};

//!@}


//! \brief Simple compile-time compiler version printer.
inline void compiler_version()
{

#ifdef __clang__
    std::cout << "Clang " << __clang_major__ << "." << __clang_minor__ << '\n';
#else
    std::cout << "GCC " << __GNUC__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__ << '\n';
#endif
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
T & Singleton<T>::instance()
{
    static T instance;
    return instance;
}


} // namespace cm
#endif // TOOLS_H
