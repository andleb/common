/** \file stack.h
 * \author Andrej Leban
 * \date 4/2018
 */

#ifndef STACK_H
#define STACK_H

#include <stack>

namespace cm {

//! \brief The Stack class
//! A std::stack that exposes the underlying container

template< class T, class Container = std::deque<T>>
class Stack : public std::stack<T,Container>
{
public:
    using super = std::stack<T,Container>;
    using super::stack;
    ~Stack() = default;
    Stack( const Stack & ) = default;
    Stack( Stack && ) noexcept = default;
    using super::operator=;

    Stack & operator=( const Stack & p_rhs )
    {
        super::operator=( static_cast<const super &>( p_rhs) );
        m_c = super::c;
        return *this;
    }

    Stack & operator=( Stack && p_rhs ) noexcept
    {
        super::operator=( std::forward<super>( p_rhs) );
        m_c = super::c;
        return *this;
    }

    // expose container
    Container & m_c = super::c;
};

} // namespace cm

#endif // STACK_H
