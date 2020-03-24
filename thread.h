/** \file thread.h
 * \author Andrej Leban
 * \date 3/2020
 *
 * Concurrency primitives and helpers.
 */

#ifndef CM_THREAD_H
#define CM_THREAD_H

#include <atomic>
#include <condition_variable>
#include <functional>
#include <thread>

// TODO:    * no need for atomic counter/flag if modified under lock

namespace cm
{

class SpinLockBarrier
{
public:
    explicit SpinLockBarrier(unsigned p_nThreads);
    SpinLockBarrier(const SpinLockBarrier &) = delete;
    SpinLockBarrier & operator=(const SpinLockBarrier &) = delete;
    ~SpinLockBarrier() = default;

    void arrive_and_wait() noexcept;
    void arrive_and_drop() noexcept;

protected:
    std::atomic_uint m_nThreads;
    std::atomic_uint m_counter;
    std::atomic_ulong m_numResets{0};
};

class Barrier : public SpinLockBarrier
{
public:
    explicit Barrier(unsigned p_nThreads);
    Barrier(const Barrier &) = delete;
    Barrier & operator=(const Barrier &) = delete;
    ~Barrier() = default;

    void arrive_and_wait() noexcept;
    void arrive_and_drop() noexcept;

protected:
    std::condition_variable m_condvar{};
    mutable std::mutex m_mutex{};
};


class FlexBarrier : public Barrier
{
public:
    using comp_func = std::function<std::ptrdiff_t()>;

    explicit FlexBarrier(unsigned p_nThreads);
    FlexBarrier(unsigned p_nThreads, comp_func p_func);
    FlexBarrier(const FlexBarrier &) = delete;
    FlexBarrier & operator=(const FlexBarrier &) = delete;
    ~FlexBarrier() = default;

    void arrive_and_wait() noexcept;
    void arrive_and_drop() noexcept;

    unsigned numThreads() const { return m_nThreads; }

protected:
    comp_func m_func;

private:
    void release() noexcept;
};

} // namespace cm

#endif // CM_THREAD_H
