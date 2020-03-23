/** \file stackcontainer.h
 * \author Andrej Leban
 * \date 4/2018
 */

#ifndef STACKCONTAINER_H
#define STACKCONTAINER_H

#include <utility>
#include <vector>

namespace cm {

//! \brief The StackContainer class
//! An array to be used as a container for std:stack.

template< typename T >
class StackContainer
{
public:
    using agreg = std::vector<T>;
    using value_type =      typename agreg::value_type;
    using reference =       typename agreg::reference;
    using const_reference = typename agreg::const_reference;
    using size_type =       typename agreg::size_type;

// Concept SequenceContainer requirements
    StackContainer() = default;
    ~StackContainer() = default;
    StackContainer(const StackContainer &) = default;
    StackContainer(StackContainer &&) noexcept = default;
    StackContainer & operator=(const StackContainer &) = default;
    StackContainer & operator=(StackContainer &&) noexcept = default;

    // reserved size is same as N! - maximum stack size, we should never exceed it
    StackContainer( size_t N ) : m_data(N), m_top(0) { m_data.shrink_to_fit();}

    StackContainer( size_t N, T t ) : m_data(N,t), m_top(N-1) { m_data.shrink_to_fit();}

    template< class InputIt >
    StackContainer( InputIt first, InputIt last ) : m_data( first, last ), m_top( m_data.size() - 1 )
    { m_data.shrink_to_fit();}

    StackContainer( std::initializer_list<T> init) : m_data( init ), m_top( m_data.size() - 1 )
    { m_data.shrink_to_fit();}

    size_t size() const { return m_data.size(); }

// a bit of a functional approach
    template< typename Ret >
    Ret shrink( Ret && ret )
    {
        m_top = m_data.size() - 1;
        m_data.shrink_to_fit();
        return std::forward<Ret> (ret);
    }

    template< typename Ret >
    Ret shrink( Ret ret )
    {
        m_top = m_data.size() - 1;
        m_data.shrink_to_fit();
        return ret;
    }

    typename agreg::iterator begin() { return m_data.begin(); }
    // semantic container extends only to the top
    typename agreg::iterator end() { return m_data.begin() + m_top; }

    typename agreg::const_iterator begin() const { return m_data.cbegin(); }
    typename agreg::const_iterator end()   const { return m_data.cbegin() + m_top; }

    StackContainer & operator=( std::initializer_list<T> init )
    {
        *this = shrink(m_data.operator=( std::move(init) ));
        return *this;
    }

    template< class... Args >
    typename agreg::iterator emplace( typename agreg::const_iterator pos, Args&&... args )
    {
        return shrink( m_data.emplace(pos, std::forward<Args>(args) ...) );
    }

    typename agreg::iterator insert( typename agreg::iterator pos, const T& value )
    {
        return shrink( m_data.insert(pos, value) );
    }

    typename agreg::iterator insert( typename agreg::iterator pos, T && value )
    {
        return shrink( m_data.insert(pos, std::forward<T> (value) ) );
    }

    typename agreg::iterator insert( typename agreg::iterator pos, size_t n, const T& value )
    {
        return shrink( m_data.insert(pos, n, value) );
    }

    template< class InputIt >
    typename agreg::iterator insert( typename agreg::const_iterator pos, InputIt first, InputIt last )
    {
        return shrink( m_data.insert(pos, first, last) );
    }

    typename agreg::iterator insert( typename agreg::const_iterator pos, std::initializer_list<T> ilist )
    {
        return shrink( m_data.insert(pos, ilist) );
    }

    typename agreg::iterator erase( typename agreg::const_iterator pos )
    {
        return shrink( m_data.erase(pos) );
    }

    typename agreg::iterator erase( typename agreg::const_iterator first, typename agreg::const_iterator last )
    {
        return shrink( m_data.erase(first, last ) );
    }

    void clear()
    {
        m_top = 0;
    }

    void assign( typename agreg::size_type count, const T& value )
    {
        shrink( m_data.assign(count, value) );
    }

    template< class InputIt >
    void assign( InputIt first, InputIt last )
    {
        shrink( m_data.assign(first, last) );
    }

    void assign( std::initializer_list<T> ilist )
    {
        shrink( m_data.assign(ilist) );
    }

    typename agreg::reference front() { return m_data[0]; }
    typename agreg::const_reference front() const { return m_data[0]; }

    typename agreg::reference       operator[]( typename agreg::size_type pos )       { return m_data[pos]; }
    typename agreg::const_reference operator[]( typename agreg::size_type pos ) const { return m_data[pos]; }

    typename agreg::reference       at( typename agreg::size_type pos )       { return m_data.at(pos); }
    typename agreg::const_reference at( typename agreg::size_type pos ) const { return m_data.at(pos); }

//std::stack requirements
    T & back() { return m_data[m_top]; }
    const T & back() const { return m_data[m_top]; }

    void push_back( const T & value ) { m_top++; m_data[m_top] = value; }
    void push_back( T&& value )       { m_top++; m_data[m_top] = std::move( value ); }

    void pop_back() { m_top--; }

private:
    std::vector<T> m_data;
    size_t m_top {0};
};

} // namespace cm

#endif // STACKCONTAINER_H
