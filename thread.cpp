/** \file thread.cpp
 * \author Andrej Leban
 * \date 3/2020
 */

#include "thread.h"

namespace cm
{


SpinLockBarrier::SpinLockBarrier(unsigned p_nThreads) : m_nThreads(p_nThreads), m_counter(m_nThreads.load()) {}

void SpinLockBarrier::arrive_and_wait() noexcept
{
    if (m_nThreads == 0)
    {
        return;
    }

    size_t numResets = m_numResets;

    if (--m_counter == 0)
    {
        // launch the waiting threads
        m_counter = m_nThreads.load();
        m_numResets++;
    }
    else
    {
        while (numResets == m_numResets)
        {
            std::this_thread::yield();
        }
    }
}

void SpinLockBarrier::arrive_and_drop() noexcept
{
    if (m_nThreads == 0)
    {
        return;
    }

    --m_nThreads;

    if (--m_counter == 0)
    {
        m_counter = m_nThreads.load();
        m_numResets++;
    }
}


Barrier::Barrier(unsigned p_nThreads) : SpinLockBarrier(p_nThreads) {}

void Barrier::arrive_and_wait() noexcept
{
    if (m_nThreads == 0)
    {
        return;
    }

    size_t numResets = m_numResets;

    std::unique_lock lk(m_mutex, std::defer_lock);

    if (--m_counter == 0)
    {
        // launch the waiting threads
        m_counter = m_nThreads.load();
        m_numResets++;
        m_condvar.notify_all();
    }
    else
    {
        lk.lock();
        m_condvar.wait(lk, [this, numResets]() { return numResets != m_numResets; });
        lk.unlock();
    }
}

void Barrier::arrive_and_drop() noexcept
{
    if (m_nThreads == 0)
    {
        return;
    }

    --m_nThreads;

    if (--m_counter == 0)
    {
        m_counter = m_nThreads.load();
        m_numResets++;
        m_condvar.notify_all();
    }
}

FlexBarrier::FlexBarrier(unsigned p_nThreads) : FlexBarrier(p_nThreads, []() noexcept { return -1; }) {}

FlexBarrier::FlexBarrier(unsigned p_nThreads, FlexBarrier::comp_func p_func) : Barrier(p_nThreads), m_func(std::move(p_func)) {}

void FlexBarrier::arrive_and_wait() noexcept
{
    if (m_nThreads == 0)
    {
        return;
    }

    size_t numResets = m_numResets;

    std::unique_lock lk(m_mutex, std::defer_lock);

    if (--m_counter == 0)
    {
        release();
    }
    else
    {
        lk.lock();
        m_condvar.wait(lk, [this, numResets]() { return numResets != m_numResets; });
        lk.unlock();
    }
}

void FlexBarrier::arrive_and_drop() noexcept
{
    if (m_nThreads == 0)
    {
        return;
    }

    --m_nThreads;

    if (--m_counter == 0)
    {
        release();
    }
}

void FlexBarrier::release() noexcept
{
    std::ptrdiff_t ret = m_func();
    if(ret >= 0)
    {
        m_nThreads = static_cast<unsigned>(ret);
    }
    m_counter = m_nThreads.load();
    m_numResets++;
    m_condvar.notify_all();
}


} // namespace cm
