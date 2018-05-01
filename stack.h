#ifndef STACK_H
#define STACK_H

#include <stack>
//namespace std
//{
//template<typename _Tp, typename _Alloc >
//  class deque;
//template<typename _Tp, typename _Sequence >
//  class stack;
//}

namespace cm {

//! \brief The Stack class
//! exposes the underlying container
template< class T, class Container = std::deque<T>>
class Stack : public std::stack<T,Container>
{
public:
    using super = std::stack<T,Container>;
    using super::stack;
    ~Stack() = default;
    Stack( const Stack & ) = default;
    Stack( Stack && ) = default;
    using super::operator=;
    Stack & operator=( const Stack & p_rhs )
    {
        super::operator=( static_cast<const super &>( p_rhs) );
        m_c = super::c;
        return *this;
    }
    Stack & operator=( Stack && p_rhs )
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
