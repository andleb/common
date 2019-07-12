/** \file numeric.h
 * \author Andrej Leban
 * \date 7/2019
 */

#ifndef NUMERIC_H
#define NUMERIC_H

#include <vector>

namespace cm {

// To accomodate float & int?
template <typename T>
std::vector<T> linspace(T start, T stop, size_t num, bool endpoint = true)
{
    std::vector<T> ret(num);
    T diff = stop - start;
    // add the endpoint
    if(endpoint) {diff *= (num / (num - 1));}

    for(size_t i = 0; i < num; ++i)
    {
        ret[i] = start + static_cast<T>((diff / num) * i);
    }

    return ret;
}

} // namespace cm


#endif // NUMERIC_H

