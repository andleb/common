/** \file numeric.h
 * \author Andrej Leban
 * \date 7/2019
 * Numeric helper functions
 * Some are inspired by packages such as NumPy
 */

#ifndef NUMERIC_H
#define NUMERIC_H

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <vector>

//TODO: * add axis param if you wish

namespace cm
{

//! \section Sequence generation

//! \brief linspace
//! Return evenly spaced numbers over a specified interval.
//! \param start: The starting value of the sequence.
//! \param stop: The end value of the sequence, unless \param endpoint is set to False.
//! \param num: Number of samples to generate. Default is 50.
//! \param endpoint: Include \param stop in the sample
//! \return a std::vector of generated samples
template <typename T>
std::vector<T> linspace(T start, T stop, size_t num = 50, bool endpoint = true)
{
    std::vector<T> ret(num);
    double diff = stop - start;
    // add the endpoint
    if (endpoint)
    {
        diff *= static_cast<double>(num) / (num - 1);
    }

    for (size_t i = 0; i < num; ++i)
    {
        ret[i] = start + static_cast<T>((diff / num) * i);
    }

    return ret;
}

//! \brief geomspace
//! Return numbers spaced evenly on a log scale over a specified interval.
//! \param start: The starting value of the sequence.
//! \param stop: The end value of the sequence, unless \param endpoint is set to False.
//! \param num: Number of samples to generate. Default is 50.
//! \param endpoint: Include \param stop in the sample
//! \return a std::vector of generated samples
template <typename T>
std::vector<T> geomspace(T start, T stop, size_t num = 50, bool endpoint = true)
{
    std::vector<T> ret(num);
    size_t n = endpoint ? (num - 1) : num;
    double diff = std::pow(stop / start, 1.0 / n);

    ret[0] = start;
    double ans = start;
    for (size_t i = 1; i < num - 1; ++i)
    {
        ans = (ans * diff);
        ret[i] = static_cast<T>(ans);
    }

    //roundoff can floor the endpoint which we want exact
    ans = (ans * diff);
    if (endpoint)
    {
        if (std::abs(ans - stop) > 1E-6)
        {
            throw std::range_error("Endpoint not connecting to the series calculated!");
        }
        ret[num - 1] = stop;
    }
    else
    {
        ret[num - 1] = static_cast<T>(ans);
    }

    return ret;
}

//! \brief geomspace
//! Return numbers spaced evenly on a log scale over a specified interval.
//! \param start: The starting value of the sequence as a power of the specified \param base
//! \param stop: The end value of the sequence as a power of the specified \param base, unless \param endpoint is set to False.
//! \param num: Number of samples to generate. Default is 50.
//! \param endpoint: Include \param stop in the sample
//! \param base: base of the logarithm
//! \return a std::vector of generated samples
template <typename T>
std::vector<T> logspace(T start, T stop, size_t num, bool endpoint = true, double base = 10.0)
{
    auto powers = linspace(start, stop, num, endpoint);
    std::vector<T> ret(num);
    std::transform(powers.begin(), powers.end(), ret.begin(), [base](auto power) { return std::pow(base, power); });
    return ret;
}

//! \section Progressions

//! \brief arithm_sum
//! \param n: number of terms
//! \param a1: first term
//! \param d: difference between terms
//! \return the sum of an arithmetic progression after \param n terms
long arithm_sum(long n, long a1, long d);

//! \brief geom_sum
//! \param n: number of terms
//! \param a1: first term
//! \param d: difference between terms
//! \return the sum of a geometric progression after \param n terms
long geom_sum(long n, long a1, long d);

} // namespace cm
#endif // NUMERIC_H
