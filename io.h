/** \file io.h
 * \author Andrej Leban
 * \date 3/2019
 */

#ifndef IO_H
#define IO_H

#include <iostream>
#include <ostream>
#include <vector>

//! \brief Overload for easy printing of std::vector contents.
//! \param os
//! \param vec
//! \return
template <class T>
std::ostream & operator<<(std::ostream & os, const std::vector<T> & vec)
{
    os << "{";
    for (auto it = vec.begin(); it != vec.end() - 1; ++it)
    {
        os << *it << ", ";
    }
    os << *(vec.end() - 1) << "}";
    return os;
}

#endif // IO_H
