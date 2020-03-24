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

    std::thread::id resetting_thread = std::numeric_limits<std::thread::id>::max();

    if (--m_counter == 0)
    {
        m_condvar.notify_all();
        resetting_thread = std::this_thread::get_id();
    }
    else
    {
        std::unique_lock lk(m_mutex);
        m_condvar.wait(lk, [this]() { return m_counter == 0; });
    }

    if(std::this_thread::get_id() == resetting_thread)
    {
        m_counter =  m_nThreads.load();
    }
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
        m_counter = m_nThreads.load(std::memory_order_seq_cst);
    }
}


} // namespace cm
