/** \file thread.cpp
 * \author Andrej Leban
 * \date 3/2020
 */

#include "thread.h"

namespace cm
{

Barrier::Barrier(uint p_nThreads) : m_nThreads(p_nThreads), m_counter(m_nThreads.load()), m_condvar() {}

void Barrier::arrive_and_wait() noexcept
{
    if (m_nThreads == 0)
    {
        return;
    }

    if (--m_counter == 0)
    {
        m_condvar.notify_all();
        m_counter = m_nThreads.load();
        return;
    }

    std::unique_lock lk(m_mutex);
    m_condvar.wait(lk, [this]() { return m_counter == 0; });
    lk.unlock();
}

void Barrier::arrive_and_drop() noexcept
{
    if (m_nThreads == 0)
    {
        return;
    }

    --m_counter;
    --m_nThreads;

    if (m_counter == 0)
    {
        m_condvar.notify_all();
        m_counter = m_nThreads.load();
    }
}


} // namespace cm
