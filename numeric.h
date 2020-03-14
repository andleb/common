/** \file numeric.h
 * \author Andrej Leban
 * \date 7/2019
 * Numeric helper functions.
 * Some are inspired by packages such as NumPy.
 */

#ifndef NUMERIC_H
#define NUMERIC_H

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <vector>

// TODO: * add axis param if you wish

namespace cm
{

//! @name Sequence generation
///@{

//! \brief Return evenly spaced numbers over a specified interval.
//! \param start: The starting value of the sequence.
//! \param stop: The end value of the sequence, unless \p endpoint is set to False.
//! \param num: Number of samples to generate. Default is 50.
//! \param endpoint: Include \p stop in the sample
//! \return a std::vector of generated samples
template <typename T>
std::vector<T> linspace(T start, T stop, size_t num = 50, bool endpoint = true);

//! \brief Return numbers spaced evenly on a geometric scale over a specified interval.
//! \param start: The starting value of the sequence.
//! \param stop: The end value of the sequence, unless \p endpoint is set to False.
//! \param num: Number of samples to generate. Default is 50.
//! \param endpoint: Include \p stop in the sample
//! \return a std::vector of generated samples
template <typename T>
std::vector<T> geomspace(T start, T stop, size_t num = 50, bool endpoint = true);

//! \brief Return numbers spaced evenly on a log scale over a specified interval.
//! \param start: The starting value of the sequence as a power of the specified \p base
//! \param stop: The end value of the sequence as a power of the specified \p base, unless \p endpoint is set to False.
//! \param num: Number of samples to generate. Default is 50.
//! \param endpoint: Include \p stop in the sample
//! \param base: base of the logarithm
//! \return a std::vector of generated samples
template <typename T>
std::vector<T> logspace(T start, T stop, size_t num, bool endpoint = true, double base = 10.0);

///@}

//! \name Progressions & Sums
///@{

//! \brief The arithmetic sum, 1-based indexing.
//! \param n: number of terms
//! \param a1: first term
//! \param d: difference between terms
//! \return The sum of an arithmetic progression after \p n terms.
template <typename T>
T arithm_sum(T n, T a1, T d);

//! \brief The geometric sum, 1-based indexing.
//! \param n: number of terms
//! \param a1: first term
//! \param d: difference between terms
//! \return The sum of a geometric progression after \p n terms.
template <typename T>
double geom_sum(T n, T a1, T d);
///@}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
std::vector<T> linspace(T start, T stop, size_t num, bool endpoint)
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

template <typename T>
std::vector<T> geomspace(T start, T stop, size_t num, bool endpoint)
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

    // round-off can floor the endpoint which we want exact
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

template <typename T>
std::vector<T> logspace(T start, T stop, size_t num, bool endpoint, double base)
{
    auto powers = linspace(start, stop, num, endpoint);
    std::vector<T> ret(num);
    std::transform(powers.begin(), powers.end(), ret.begin(), [base](auto power) { return std::pow(base, power); });
    return ret;
}

template <typename T>
T arithm_sum(T n, T a1, T d)
{
    return static_cast<T>(std::round(static_cast<double>(2 * a1 + (n - 1) * d) / 2 * n));
}

template <typename T>
double geom_sum(T n, T a1, T d)
{
    return static_cast<double>(std::round(a1 * (1.0 - std::pow(d, n)) / (1.0 - d)));
}

} // namespace cm
#endif // NUMERIC_H
