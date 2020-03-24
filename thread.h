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
#include <thread>


namespace cm
{

class Barrier
{
public:
    explicit Barrier(uint p_nThreads=std::thread::hardware_concurrency());
    Barrier(const Barrier &) = delete;
    Barrier & operator=(const Barrier &) = delete;
    // TODO: can this be default?
    ~Barrier() = default;

    void arrive_and_wait() noexcept;
    void arrive_and_drop() noexcept;

private:
    std::atomic_uint m_nThreads;

    std::atomic_uint m_counter;
    std::condition_variable m_condvar;
    mutable std::mutex m_mutex;
};


class FlexBarrier
{
public:
    FlexBarrier() = default;
};

} // namespace cm

#endif // CM_THREAD_H
