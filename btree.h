/** \file btree.h
 * \author Andrej Leban
 * \date 1/2019
 */

#ifndef BTREE_H
#define BTREE_H

#include <algorithm>
#include <stdexcept>
#include <deque>
#include <vector>

namespace cm {

// FREE FUNCTIONS

size_t numElems(size_t depth);

//! \brief The bTree class
//! An implementation of a fixed-depth binary tree
//! Requires \p Node to have a default value signifying an empty(leaf) node
//! BFS indexing, matching the underlying array container

template<typename Node>
class bTree
{
public:
    //! \brief bTree
    //! \param depth - number of sublevels, [0, inf)
    bTree(size_t depth): m_depth(depth), m_data(numElems(m_depth)) {}

    void insert(size_t ind, Node node);
    void remove(size_t ind);
    size_t size() const { return m_data.size(); }

    Node & operator[](size_t ind) { return m_data[ind]; }
    const Node & operator[](size_t ind) const { return m_data[ind]; }
    auto begin() {return m_data.begin();}
    auto end() {return m_data.end();}

    static size_t goUp(size_t ind);
    static size_t goDownLeft(size_t ind);
    static size_t goDownRight(size_t ind);

    // vector will naturally throw when out of bounds or parent of root, so no possibility of an invalid ref
    Node & parent(const Node & node);
    Node & leftchild(const Node & node);
    Node & rightchild(const Node & node);

    //! \brief copySubTree
    //! \param indS
    //! \param indT
    //! \return target indices copied to
    //! copies whole subtree from source index to target index
    //! indices must be on the same level
    std::vector<size_t> copySubTree(size_t indS, size_t indT);
    void copySubTree(size_t indS, size_t indT, std::vector<size_t> & target_indices);

private:
    size_t m_depth;
    std::vector<Node> m_data;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename Node>
void bTree<Node>::insert(size_t ind, Node node)
{
    m_data[ind] = node;
}

template<typename Node>
void bTree<Node>::remove(size_t ind)
{
    m_data[ind] = Node{};
}

template<typename Node>
size_t bTree<Node>::goUp(size_t ind)
{
    return (ind - 1) / 2;
}

template<typename Node>
size_t bTree<Node>::goDownLeft(size_t ind)
{
    return 2 * ind + 1;
}

template<typename Node>
size_t bTree<Node>::goDownRight(size_t ind)
{
    return 2 * ind + 2;
}

template<typename Node>
Node & bTree<Node>::parent(const Node & node)
{
    typename decltype(m_data)::iterator el;
    if((el=std::find(m_data.begin(), m_data.end(), node)) != m_data.end())
    {
        auto ind = std::distance(m_data.begin(), el);
        auto newInd = goUp(ind);
        return m_data[newInd];
    }

    throw std::range_error("Node not in tree");
}

template<typename Node>
Node & bTree<Node>::leftchild(const Node & node)
{
    typename decltype(m_data)::iterator el;
    if((el=std::find(m_data.begin(), m_data.end(), node)) != m_data.end())
    {
        auto ind = std::distance(m_data.begin(), el);
        auto newInd = goDownLeft(ind);
        return m_data[newInd];
    }

    throw std::range_error("Node not in tree");
}

template<typename Node>
Node & bTree<Node>::rightchild(const Node & node)
{
    typename decltype(m_data)::iterator el;
    if((el=std::find(m_data.begin(), m_data.end(), node)) != m_data.end())
    {
        auto ind = std::distance(m_data.begin(), el);
        auto newInd = goDownRight(ind);
        return m_data[newInd];
    }

    throw std::range_error("Node not in tree");
}

template<typename Node>
std::vector<size_t> bTree<Node>::copySubTree(size_t indS, size_t indT)
{
    std::vector<size_t> ret {};
    copySubTree(indS, indT, ret);
    return ret;
}

template<typename Node>
void bTree<Node>::copySubTree(size_t indS, size_t indT, std::vector<size_t> & target_indices)
{
    m_data[indT] = m_data[indS];
    target_indices.push_back(indT);

    // left-first depth-first
    size_t sourceLeft = goDownLeft(indS);
    size_t targetLeft = goDownLeft(indT);

    // go left
    if( sourceLeft < m_data.size() &&
        sourceLeft > 0  &&
        targetLeft < m_data.size() &&
        targetLeft > 0
       )
    {
        copySubTree(sourceLeft, targetLeft, target_indices);

        //go right
        size_t sourceRight= goDownRight(indS);
        size_t targetRight= goDownRight(indT);

        if( sourceRight < m_data.size() &&
            sourceRight > 0  &&
            targetRight < m_data.size() &&
            targetRight > 0
           )
        {
            copySubTree(sourceRight, targetRight, target_indices);
        }
    }

    // return - goes up a level
}

} // namespace cm

#endif // BTREE_H
