/** \file tools.h
 * \author Andrej Leban
 * \date 9/2019
 */

#ifndef TOOLS_H
#define TOOLS_H

#include <iostream>

namespace cm
{

//! \brief Simple compile-time compiler version printer.
inline void compiler_version()
{

#ifdef __clang__
    std::cout <<"Clang " << __clang_major__ << "." << __clang_minor__ << '\n';
#else
    std::cout <<"GCC " << __GNUC__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__ << '\n';
#endif

}

} // namespace cm
#endif // TOOLS_H
