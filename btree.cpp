/** \file btree.cpp
 * \author Andrej Leban
 * \date 1/2019
 */

#include <cmath>
#include <stdexcept>
#include <numeric>

#include "btree.h"

namespace cm {

size_t numElems(size_t depth)
{
    return static_cast<size_t>(std::pow(2, depth + 1) - 1);
}

} // namespace cm
