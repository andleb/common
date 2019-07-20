/** \file btree.h
 * \author Andrej Leban
 * \date 1/2019
 */

#ifndef BTREE_H
#define BTREE_H

#include <algorithm>
#include <cmath>
#include <memory>
#include <stdexcept>
#include <unordered_set>
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
    //! NOTE: these should be much faster!
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
    static size_t left_boundary(size_t level);
    static size_t right_boundary(size_t level);

    size_t goUpLeft(size_t ind) const;
    size_t goUpRight(size_t ind) const;
    size_t goDownLeft(size_t ind) const;
    size_t goDownRight(size_t ind) const;

    // alias for compatibility
    // affects parentLeft below
    size_t goUp(size_t ind) const { return goUpLeft(ind); }

    //! \paragraph additional Node-based operations
    Node & parentLeft(const Node & node);
    Node & parentRight(const Node & node);

    //! \brief copySubTreeLeft
    //! \param indS: source index
    //! \param indT: target index
    //! \return A vector of copied indices in order of copying
    //! copies whole subtree from source index to target index,
    //! keeping the values for the shared nodes from the initial left descend
    //! Warning: indices must be on the same level

    std::vector<size_t> copySubTreeLeft(size_t indS, size_t indT);


    //! \brief copySubTreeRight
    //! \param indS: source index
    //! \param indT: target index
    //! \return A vector of copied indices in order of copying
    //! copies whole subtree from source index to target index,
    //! setting the values for the shared nodes from final right descend
    //! This means a left target can serve as a source for a node to its right later on!
    //! Warning: indices must be on the same level
    std::vector<size_t> copySubTreeRight(size_t indS, size_t indT);

private:
    using super = bTree<Node>;

    // recursive implementations not used
    void copySubTreeRight(size_t indS, size_t indT, std::vector<size_t> & target_indices);
    std::unique_ptr<std::unordered_set<size_t>>
    copySubTreeLeft(size_t indS, size_t indT, std::vector<size_t> & target_indices,
                    std::unique_ptr<std::unordered_set<size_t>> pSeen = nullptr);
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

template<typename Node>
size_t recombinantBTree<Node>::left_boundary(size_t level)
{
    return static_cast<size_t>(arithm_sum(static_cast<long>(level), 1, 1));
}

template<typename Node>
size_t recombinantBTree<Node>::right_boundary(size_t level)
{
    return static_cast<size_t>(arithm_sum(static_cast<long>(level + 1), 1, 1) - 1);
}

template <typename Node>
size_t recombinantBTree<Node>::numElems(size_t depth) const
{
    // arithmetic sum w 0-based indexing
    return static_cast<size_t>(depth * (1 + depth + 1) / 2);
}

template <typename Node>
size_t recombinantBTree<Node>::goUpLeft(size_t ind) const
{

    // left boundary nodes have no left parent
    if (ind == left_boundary(level(ind)))
    {
        throw std::range_error("The node corresponding to the index provided is on the left boundary!");
    }
    return ind - level_size(ind);
}

template <typename Node>
size_t recombinantBTree<Node>::goUpRight(size_t ind) const
{
    // right boundary nodes have no right parent
    // the next ind is the left boundary node of the next level
    if (ind == right_boundary(level(ind)))
    {
        throw std::range_error("The node corresponding to the index provided is on the right boundary!");
    }
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

template<typename Node>
std::vector<size_t> recombinantBTree<Node>::copySubTreeLeft(size_t indS, size_t indT)
{
    std::vector<size_t> ret{};

    // recursive implementation
//    copySubTreeLeft(indS, indT, ret, std::make_unique<std::unordered_set<size_t>> ());

    // non-recursive implementation
    if(!(level(indS) == level(indT)))
    {
        throw std::range_error("Source and target nodes must be on the same level!");
    }

    // this is equal to setting the last value on the level to the value preceding it for all the levels below
    // and including the initial
    size_t l = level(indS);
    size_t last = right_boundary(l);

    while(last < super::m_data.size())
    {
        super::m_data[last] = super::m_data[last - 1 ];

        // proceed to the next level
        ++l;
        last = right_boundary(l);
    }

    return ret;
}

template <typename Node>
std::unique_ptr<std::unordered_set<size_t>> recombinantBTree<Node>::copySubTreeLeft(size_t indS, size_t indT, std::vector<size_t> & target_indices,
                                                                                    std::unique_ptr<std::unordered_set<size_t>> pSeen)
{
    // top level
    if (!pSeen)
    {
        pSeen = std::make_unique<std::unordered_set<size_t>>();
    }

    bTree<Node>::m_data[indT] = super::m_data[indS];
    target_indices.push_back(indT);
    // shared nodes are always sources
    pSeen->insert(indS);

    // left-first depth-first
    size_t sourceLeft = goDownLeft(indS);
    size_t targetLeft = goDownLeft(indT);

    // go left
    // only go left if not visited before, else keep the value of the initial first descend
    if (sourceLeft < super::m_data.size() && sourceLeft > 0 && targetLeft < super::m_data.size() && targetLeft > 0
        && pSeen->find(targetLeft) == pSeen->end())
    {
        pSeen = copySubTreeLeft(sourceLeft, targetLeft, target_indices, std::move(pSeen));

        //go right
        size_t sourceRight = goDownRight(indS);
        size_t targetRight = goDownRight(indT);

        if (sourceRight < super::m_data.size() && sourceRight > 0 && targetRight < super::m_data.size() && targetRight > 0)
        {
            pSeen = copySubTreeLeft(sourceRight, targetRight, target_indices, std::move(pSeen));
        }
    }

    // return - goes up a level
    return pSeen;
}

template<typename Node>
std::vector<size_t> recombinantBTree<Node>::copySubTreeRight(size_t indS, size_t indT)
{
    std::vector<size_t> ret{};

    // recursive implementation
//    copySubTreeRight(indS, indT, ret);

    // non-recursive implementation
    if(!(level(indS) == level(indT)))
    {
        throw std::range_error("Source and target nodes must be on the same level!");
    }

    // this is equal to setting the whole level to the left value below source for all the levels
    // below and including the initial
    size_t l = level(indS);
    size_t offset = indS - left_boundary(l);
    size_t source = indS;

    while(source < super::m_data.size())
    {
        auto start = super::m_data.begin() + source + 1;
        // fill to the end of the level - std::algorithms use a half-open bracket
        auto end = super::m_data.begin() + left_boundary(l + 1);

        std::fill(start, end, super::m_data[source]);


        // proceed to tge next level
        ++l;
        source = left_boundary(l) + offset;
    }

    return ret;
}

template<typename Node>
void
recombinantBTree<Node>::copySubTreeRight(size_t indS, size_t indT, std::vector<size_t> & target_indices)
{
    bTree<Node>::m_data[indT] = super::m_data[indS];
    target_indices.push_back(indT);

    // left-first depth-first
    size_t sourceLeft = goDownLeft(indS);
    size_t targetLeft = goDownLeft(indT);

    // always go left
    if (sourceLeft < super::m_data.size() && sourceLeft > 0 && targetLeft < super::m_data.size() && targetLeft > 0)
    {
        copySubTreeRight(sourceLeft, targetLeft, target_indices);

        //go right
        size_t sourceRight = goDownRight(indS);
        size_t targetRight = goDownRight(indT);

        if (sourceRight < super::m_data.size() && sourceRight > 0 && targetRight < super::m_data.size() && targetRight > 0)
        {
            copySubTreeRight(sourceRight, targetRight, target_indices);
        }
    }

    // return - goes up a level
}

} // namespace cm

#endif // BTREE_H
