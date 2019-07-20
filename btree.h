/** \file btree.h
 * \author Andrej Leban
 * \date 1/2019
 */

#ifndef BTREE_H
#define BTREE_H

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <vector>

#include <common/numeric.h>

namespace cm
{

//! \brief The bTree class
//! An implementation of a fixed-depth binary tree
//! Requires \p Node to have a default value signifying an empty(leaf) node
//! BFS indexing, matching the underlying array container
template <typename Node>
class bTree
{
public:
    using value_type = Node;

    //! \brief bTree
    //! \param depth - number of sublevels, [0, inf)
    bTree(size_t depth)
        : m_depth(depth)
        , m_data(numElems(m_depth))
    {}

    //! \paragraph Index-based operations
    //! NOTE: these are much faster!
    void insert(size_t ind, Node node);
    void remove(size_t ind);
    size_t size() const { return m_data.size(); }

    auto begin() { return m_data.begin(); }
    auto end() { return m_data.end(); }

    //! \paragraph Conversions
    Node & operator[](size_t ind) { return m_data[ind]; }
    const Node & operator[](size_t ind) const { return m_data[ind]; }
    size_t current(const Node & node) const;

    size_t goUp(size_t ind) const;
    size_t goDownLeft(size_t ind) const;
    size_t goDownRight(size_t ind) const;

    virtual size_t numElems(size_t depth) const;

    //! \paragraph Node-based operations
    // vector will naturally throw when out of bounds or parent of root, so no possibility of an invalid ref
    Node & root();
    Node & parent(const Node & node);
    Node & leftchild(const Node & node);
    Node & rightchild(const Node & node);

    //! \brief copySubTree
    //! \param indS: source index
    //! \param indT: target index
    //! \return target indices copied to
    //! copies whole subtree from source index to target index
    //! Warning: indices must be on the same level
    std::vector<size_t> copySubTree(size_t indS, size_t indT);
    void copySubTree(size_t indS, size_t indT, std::vector<size_t> & target_indices);

protected:
    size_t m_depth;
    std::vector<Node> m_data;
};

//! \brief The recombinantBTree class
//! A binary tree where the inner nodes spring from two parents
template <typename Node>
class recombinantBTree : public bTree<Node>
{
public:
    //! \brief recombinantBTree
    //! \param depth - number of sublevels, [0, inf)
    recombinantBTree(size_t depth);

    size_t numElems(size_t depth) const override;

    static size_t level(size_t ind);
    static size_t level_size(size_t ind);

    size_t goUpLeft(size_t ind) const;
    size_t goUpRight(size_t ind) const;
    size_t goDownLeft(size_t ind) const;
    size_t goDownRight(size_t ind) const;

    // alias for compatibility
    size_t goUp(size_t ind) const { return goUpLeft(ind); }

    //! \paragraph additional Node-based operations
    Node & parentLeft(const Node & node);
    Node & parentRight(const Node & node);

    //! \brief copySubTree
    //! \param indS: source index
    //! \param indT: target index
    //! \return target indices copied to
    //! copies whole subtree from source index to target index
    //! Warning: indices must be on the same level
    void copySubTree(size_t indS, size_t indT, std::vector<size_t> & target_indices);

private:
    using super = bTree<Node>;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// bTree

template <typename Node>
void bTree<Node>::insert(size_t ind, Node node)
{
    m_data[ind] = node;
}

template <typename Node>
void bTree<Node>::remove(size_t ind)
{
    m_data[ind] = Node{};
}

template <typename Node>
size_t bTree<Node>::current(const Node & node) const
{
    typename decltype(m_data)::iterator el;
    if ((el = std::find(m_data.begin(), m_data.end(), node)) != m_data.end())
    {
        return std::distance(m_data.begin(), el);
    }

    throw std::range_error("Node not in tree");
}

template <typename Node>
size_t bTree<Node>::goUp(size_t ind) const
{
    return (ind - 1) / 2;
}

template <typename Node>
size_t bTree<Node>::goDownLeft(size_t ind) const
{
    return 2 * ind + 1;
}

template <typename Node>
size_t bTree<Node>::goDownRight(size_t ind) const
{
    return 2 * ind + 2;
}

template <typename Node>
size_t bTree<Node>::numElems(size_t depth) const
{
    return static_cast<size_t>(std::pow(2, depth + 1) - 1);
}

template <typename Node>
Node & bTree<Node>::root()
{
    return m_data[0];
}

template <typename Node>
Node & bTree<Node>::parent(const Node & node)
{
    typename decltype(m_data)::iterator el;
    if ((el = std::find(m_data.begin(), m_data.end(), node)) != m_data.end())
    {
        auto ind = std::distance(m_data.begin(), el);
        auto newInd = goUp(ind);
        return m_data[newInd];
    }

    throw std::range_error("Node not in tree");
}

template <typename Node>
Node & bTree<Node>::leftchild(const Node & node)
{
    typename decltype(m_data)::iterator el;
    if ((el = std::find(m_data.begin(), m_data.end(), node)) != m_data.end())
    {
        auto ind = std::distance(m_data.begin(), el);
        auto newInd = goDownLeft(ind);
        return m_data[newInd];
    }

    throw std::range_error("Node not in tree");
}

template <typename Node>
Node & bTree<Node>::rightchild(const Node & node)
{
    typename decltype(m_data)::iterator el;
    if ((el = std::find(m_data.begin(), m_data.end(), node)) != m_data.end())
    {
        auto ind = std::distance(m_data.begin(), el);
        auto newInd = goDownRight(ind);
        return m_data[newInd];
    }

    throw std::range_error("Node not in tree");
}

template <typename Node>
std::vector<size_t> bTree<Node>::copySubTree(size_t indS, size_t indT)
{
    std::vector<size_t> ret{};
    copySubTree(indS, indT, ret);
    return ret;
}

template <typename Node>
void bTree<Node>::copySubTree(size_t indS, size_t indT, std::vector<size_t> & target_indices)
{
    m_data[indT] = m_data[indS];
    target_indices.push_back(indT);

    // left-first depth-first
    size_t sourceLeft = goDownLeft(indS);
    size_t targetLeft = goDownLeft(indT);

    // go left
    if (sourceLeft < m_data.size() && sourceLeft > 0 && targetLeft < m_data.size() && targetLeft > 0)
    {
        copySubTree(sourceLeft, targetLeft, target_indices);

        //go right
        size_t sourceRight = goDownRight(indS);
        size_t targetRight = goDownRight(indT);

        if (sourceRight < m_data.size() && sourceRight > 0 && targetRight < m_data.size() && targetRight > 0)
        {
            copySubTree(sourceRight, targetRight, target_indices);
        }
    }

    // return - goes up a level
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//recombinantBTree

template <typename Node>
recombinantBTree<Node>::recombinantBTree(size_t depth)
    : bTree<Node>(depth)
{}

template <typename Node>
size_t recombinantBTree<Node>::level(size_t ind)
{
    // analytic solution
    return static_cast<size_t>(std::round(std::sqrt(1 + 2 * ind) - 1));

    /*
    // O(ind) brute force implementation
    size_t counter = 0;
    size_t level = 0;
    size_t lsize = 1;
    size_t boundary = 1;

    while(counter < ind)
    {
        if(++counter == boundary)
        {
            level += 1;
            lsize += 1;
            boundary += lsize;
        }
    }

    return level;
    */
}

template <typename Node>
size_t recombinantBTree<Node>::level_size(size_t ind)
{
    return level(ind) + 1;
}

template <typename Node>
size_t recombinantBTree<Node>::numElems(size_t depth) const
{
    // arithmetic sum w 0-based indexing
    return static_cast<size_t>(depth * (1 + depth + 1) / 2);
}

// FIXME: bind these to inner points
template <typename Node>
size_t recombinantBTree<Node>::goUpLeft(size_t ind) const
{

    std::round(std::sqrt(1 + 2 * ind) - 1)
    return ind - level_size(ind);
}

template <typename Node>
size_t recombinantBTree<Node>::goUpRight(size_t ind) const
{
    return ind - level_size(ind) + 1;
}

template <typename Node>
size_t recombinantBTree<Node>::goDownLeft(size_t ind) const
{
    return ind + level_size(ind);
}

template <typename Node>
size_t recombinantBTree<Node>::goDownRight(size_t ind) const
{
    return ind + level_size(ind) + 1;
}

template <typename Node>
Node & recombinantBTree<Node>::parentLeft(const Node & node)
{
    return parent(node);
}

template <typename Node>
Node & recombinantBTree<Node>::parentRight(const Node & node)
{
    typename decltype(super::m_data)::iterator el;
    if ((el = std::find(super::m_data.begin(), super::m_data.end(), node)) != super::m_data.end())
    {
        auto ind = std::distance(super::m_data.begin(), el);
        auto newInd = goUpRight(ind);
        return super::m_data[newInd];
    }

    throw std::range_error("Node not in tree");
}

// FIXME: figure out if this is ok wrt going left and right
template <typename Node>
void recombinantBTree<Node>::copySubTree(size_t indS, size_t indT, std::vector<size_t> & target_indices)
{
    bTree<Node>::m_data[indT] = super::m_data[indS];
    target_indices.push_back(indT);

    // left-first depth-first
    size_t sourceLeft = goDownLeft(indS);
    size_t targetLeft = goDownLeft(indT);

    // go left
    if (sourceLeft < super::m_data.size() && sourceLeft > 0 && targetLeft < super::m_data.size() && targetLeft > 0)
    {
        copySubTree(sourceLeft, targetLeft, target_indices);

        //go right
        size_t sourceRight = goDownRight(indS);
        size_t targetRight = goDownRight(indT);

        if (sourceRight < super::m_data.size() && sourceRight > 0 && targetRight < super::m_data.size() && targetRight > 0)
        {
            copySubTree(sourceRight, targetRight, target_indices);
        }
    }
}

} // namespace cm

#endif // BTREE_H
