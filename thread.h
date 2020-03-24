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

namespace cm
{

//! \brief A simple barrier using a busy wait.
class SpinLockBarrier
{
public:
    //! \brief SpinLockBarrier
    //! \param p_nThreads - The number of participating threads.
    explicit SpinLockBarrier(unsigned p_nThreads);
    SpinLockBarrier(const SpinLockBarrier &) = delete;
    SpinLockBarrier & operator=(const SpinLockBarrier &) = delete;
    ~SpinLockBarrier() = default;

    //! \brief The current thread blocks until all \a m_nThreads have arrived at the same point.
    void arrive_and_wait() noexcept;
    //! \brief The current thread is counted as having arrived at the barrier, then drops out,
    //! thereby no longer being counted as participating.
    //! NOTE: The user is required to ensure on the call-side that the thread no longer arrives at the barrier afterwards.
    void arrive_and_drop() noexcept;

protected:
    std::atomic_uint m_nThreads;
    std::atomic_uint m_counter;
    std::atomic_ulong m_numResets{0};
};

//! \brief A synchronization barrier.
//! Implementation of std::experimental::barrier.
class Barrier : public SpinLockBarrier
{
public:
    //! \brief Barrier
    //! \param p_nThreads - The number of participating threads.
    explicit Barrier(unsigned p_nThreads);
    Barrier(const Barrier &) = delete;
    Barrier & operator=(const Barrier &) = delete;
    ~Barrier() = default;

    //! \brief The current thread blocks until all \a m_nThreads have arrived at the same point.
    void arrive_and_wait() noexcept;
    //! \brief The current thread is counted as having arrived at the barrier, then drops out,
    //! thereby no longer being counted as participating.
    //! NOTE: The user is required to ensure on the call-side that the thread no longer arrives at the barrier afterwards.
    void arrive_and_drop() noexcept;

protected:
    std::condition_variable m_condvar{};
    mutable std::mutex m_mutex{};
};

//! \brief A synchronization barrier with an optional callable that is called after each synchronization.
//! Implementation of std::experimental::flex_barrier.
//! The signature of the callable is std::ptrdiff_t (void), where the return value signifies the new number of participating
//! threads. If the return is -1, the number remains unchanged.
class FlexBarrier : public Barrier
{
public:
    using comp_func = std::function<std::ptrdiff_t()>;

    //! \brief FlexBarrier
    //! \param p_nThreads - The number of participating threads.
    explicit FlexBarrier(unsigned p_nThreads);
    //! \brief FlexBarrier
    //! \param p_nThreads - The number of participating threads.
    //! \param p_func - Custom callable invoked at each synchronization.
    FlexBarrier(unsigned p_nThreads, comp_func p_func);
    FlexBarrier(const FlexBarrier &) = delete;
    FlexBarrier & operator=(const FlexBarrier &) = delete;
    ~FlexBarrier() = default;

    //! \brief The current thread blocks until all \a m_nThreads have arrived at the same point.
    void arrive_and_wait() noexcept;
    //! \brief The current thread is counted as having arrived at the barrier, then drops out,
    //! thereby no longer being counted as participating.
    //! NOTE: The user is required to ensure on the call-side that the thread no longer arrives at the barrier afterwards.
    void arrive_and_drop() noexcept;

    unsigned numThreads() const { return m_nThreads; }

protected:
    comp_func m_func;

private:
    void release() noexcept;
};

} // namespace cm

#endif // CM_THREAD_H
