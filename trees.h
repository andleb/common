/** \file trees.h
 * \author Andrej Leban
 * \date 1/2019
 *
 * Array-based Tree classes.
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

// TODO: * be careful: number of *: 1-based, level etc.: 0-based
//       * interface

namespace cm
{

//! \brief An implementation of a fixed-depth binary tree.
//! Requires \p Node to have a default value signifying an empty(leaf) node.
//! BFS indexing, matching the underlying array container.
template <typename Node>
class bTree
{
public:
    using value_type = Node;

    //! \brief bTree
    //! \param depth - number of sub-levels, [0, inf)
    //! Root is level 0!
    bTree(size_t depth);
    virtual ~bTree() = default;

    //! @name Index-based operations
    //! NOTE: these should be much faster!
    //!@{

    //! \brief Insert \p node at \p ind.
    //! \param ind
    //! \param node
    void insert(size_t ind, Node node);
    //! \brief Insert the node at \p ind.
    //! \param ind
    void remove(size_t ind);
    //! \brief The total number of elements in the tree.
    size_t totalElems() const;
    //! \brief operator []
    //! \param ind
    //! \return
    Node & operator[](size_t ind);
    //! \brief operator [] const
    //! \param ind
    //! \return
    const Node & operator[](size_t ind) const;
    ///@}

    //! \brief The begin iterator of the underlying structure.
    auto begin();
    //! \brief The end iterator of the underlying structure.
    auto end();
    //! \brief Returns the array index of the \p node provided.
    //! \param node
    //! \return
    size_t node2Ind(const Node & node) const;

    //! \brief Number of elements up to and including(!) the given level.
    //! \param level
    //! \return
    // called from constructor, so not virtual.
    size_t numElems(size_t level) const;
    //! \brief The number of total levels - the depth of the tree + 1
    //! (constructor is 0-indexed).
    size_t numLevels() const;

    // For each tree it must be possible to go up, hence pure virtual.
    // Since the geometry will almost always differ (else why subclass?), it is made pure
    // in contrast to simple functions such as insert above that can simply get inherited.
    virtual size_t goUp(size_t ind) const = 0;
    virtual size_t goDownLeft(size_t ind) const;
    virtual size_t goDownRight(size_t ind) const;

    //! @name Node-based operations
    //! These look up the node, so slower (\f$\mathcal{O}(n)\f$) than index-based operations.
    ///@{
    // std::vector will naturally throw when out of bounds or parent of root, so no possibility of an invalid ref
    //! \brief root
    virtual Node & root();
    virtual const Node & root() const;
    //! \brief Returns the parent node.
    // Uses goUp
    // Implemented in terms of the const overload, so no pure.
    virtual Node & parent(const Node & node);
    virtual const Node & parent(const Node & node) const = 0;
    //! \brief leftchild
    // Uses goDownLeft
    virtual Node & leftchild(const Node & node);
    virtual const Node & leftchild(const Node & node) const;
    //! \brief rightchild
    // Uses goDownRight
    virtual Node & rightchild(const Node & node);
    virtual const Node & rightchild(const Node & node) const;
    ///@}

    //! \brief Copies whole sub-tree from source index to target index.
    //! Warning: indices must be on the same level!
    //! \param indS: source index
    //! \param indT: target index
    //! \return Target indices copied to.
    // No harm in these getting inherited, worst-case scenario is overwriting of same nodes.
    std::vector<size_t> copySubTree(size_t indS, size_t indT);

protected:
    //! \brief Used by child classes
    //! \param depth
    //! \param num_elements
    bTree(size_t depth, size_t num_elements);

    // In-place implementation, called by the above.
    // Only relevant from the 1st sub-level down.
    void copySubTree(size_t indS, size_t indT, std::vector<size_t> & target_indices);

    size_t m_depth;
    std::vector<Node> m_data;
};


//! \brief A binary tree where the inner nodes spring from two parents.
template <typename Node>
class recombinantBTree : public bTree<Node>
{
public:
    //! \brief recombinantBTree
    //! \param depth - number of sub-levels, [0, inf)
    //! Root is level 0!
    recombinantBTree(size_t depth);

    //! @name Geometry
    ///@{
    //! \brief Get the level from the index.
    //! \param ind - The array index.
    static size_t level(size_t ind);
    //! \brief level_size
    //! \param ind - The array index.
    static size_t level_size(size_t ind);
    //! \brief left_boundary - inclusive!
    static size_t left_boundary(size_t level);
    //! \brief right_boundary - inclusive.
    static size_t right_boundary(size_t level);
    ///@}

    // called from constructor, so not virtual.
    size_t numElems(size_t level) const;

    // alias for compatibility - goUpLeft
    // affects parentLeft below
    virtual size_t goUp(size_t ind) const override;
    virtual size_t goUpLeft(size_t ind) const;
    virtual size_t goUpRight(size_t ind) const;
    virtual size_t goDownLeft(size_t ind) const override;
    virtual size_t goDownRight(size_t ind) const override;

    //! @name Additional node-based operations
    //! These look up the node, so slower than index-based operations.
    ///@{

    //! \brief By convention, left parent.
    //! Implemented for the sake of consistent interface.
    virtual const Node & parent(const Node & node) const override;
    //! \brief parentLeft
    Node & parentLeft(const Node & node);
    const Node & parentLeft(const Node & node) const;
    //! \brief parentRight
    Node & parentRight(const Node & node);
    const Node & parentRight(const Node & node) const;
    // NOTE: the rest, e.g. *child are taken care of by the base class & having implemented
    // the go* virtuals

    ///@}

    //! \brief copies whole sub-tree from source index to target index,
    //! keeping the values for the shared nodes from the initial left descend.
    //! Warning: indices must be on the same level.
    //! \param indS: source index
    //! \param indT: target index
    //! \return A vector of copied indices in order of copying
    std::vector<size_t> copySubTreeLeft(size_t indS, size_t indT);

    //! \brief copies whole sub-tree from source index to target index,
    //! setting the values for the shared nodes from final right descend.
    //! This means a left target can serve as a source for a node to its right later on!
    //! Warning: indices must be on the same level.
    //! \param indS: source index
    //! \param indT: target index
    //! \return A vector of copied indices in order of copying
    std::vector<size_t> copySubTreeRight(size_t indS, size_t indT);

protected:
    using super = bTree<Node>;

private:
    // deprecated recursive implementations.
    std::unique_ptr<std::unordered_set<size_t>> copySubTreeLeft(size_t indS, size_t indT, std::vector<size_t> & target_indices,
                                                                std::unique_ptr<std::unordered_set<size_t>> pSeen = nullptr);
    // doesn't need a list of visited nodes as it doesn't care about overwriting.
    void copySubTreeRight(size_t indS, size_t indT, std::vector<size_t> & target_indices);
};


//! \brief A binary tree where the inner nodes spring from three parents.
//! Similar interface to recombinantBTree, but not a subclass of the latter.
template <typename Node>
class recombinantTTree : public bTree<Node>
{
public:
    //! \brief recombinantTTree
    //! \param depth - number of sub-levels, [0, inf)
    //! Root is level 0!
    recombinantTTree(size_t depth);

    //! @name Geometry
    ///@{
    //! \brief Get the level from the index.
    //! \param ind - The array index.
    static size_t level(size_t ind);
    //! \brief level_size
    //! \param ind - The array index.
    static size_t level_size(size_t ind);
    //! \brief left_boundary - inclusive!
    static size_t left_boundary(size_t level);
    //! \brief right_boundary - inclusive.
    static size_t right_boundary(size_t level);
    ///@}

    // called from constructor, so not virtual.
    size_t numElems(size_t level) const;

    // alias for compatibility - goUpLeft
    // affects parentLeft below
    virtual size_t goUp(size_t ind) const override;
    virtual size_t goUpLeft(size_t ind) const;
    virtual size_t goUpCenter(size_t ind) const;
    virtual size_t goUpRight(size_t ind) const;
    virtual size_t goDownLeft(size_t ind) const override;
    virtual size_t goDownCenter(size_t ind) const override;
    virtual size_t goDownRight(size_t ind) const override;

    //! @name Additional node-based operations
    //! These look up the node, so slower than index-based operations.
    ///@{

    //! \brief By convention, left parent.
    //! Implemented for the sake of consistent interface.
    virtual const Node & parent(const Node & node) const override;
    //! \brief parentLeft
    Node & parentLeft(const Node & node);
    const Node & parentLeft(const Node & node) const;
    //! \brief parentCenter
    Node & parentCenter(const Node & node);
    const Node & parentCenter(const Node & node) const;
    //! \brief parentRight
    Node & parentRight(const Node & node);
    const Node & parentRight(const Node & node) const;
    //! \brief centerchild
    Node & centerchild(const Node & node);
    const Node & centerchild(const Node & node) const;
    // NOTE: the rest, e.g. *child are taken care of by the base class & having implemented
    // the go* virtuals

    ///@}

    //! \brief copies whole sub-tree from source index to target index,
    //! keeping the values for the shared nodes from the initial left descend.
    //! Warning: indices must be on the same level.
    //! \param indS: source index
    //! \param indT: target index
    //! \return A vector of copied indices in order of copying
    std::vector<size_t> copySubTreeSource(size_t indS, size_t indT);

    //! \brief copies whole sub-tree from source index to target index,
    //! setting the values for the shared nodes from final right descend.
    //! This means a left target can serve as a source for a node to its right later on!
    //! Warning: indices must be on the same level.
    //! \param indS: source index
    //! \param indT: target index
    //! \return A vector of copied indices in order of copying
    std::vector<size_t> copySubTreeTarget(size_t indS, size_t indT);

protected:
    using super = bTree<Node>;
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// bTree

template <typename Node>
bTree<Node>::bTree(size_t depth) : m_depth(depth), m_data(numElems(m_depth))
{}

template <typename Node>
bTree<Node>::bTree(size_t depth, size_t num_elements) : m_depth(depth), m_data(num_elements)
{}

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
size_t bTree<Node>::totalElems() const
{
    return m_data.size();
}

template <typename Node>
Node & bTree<Node>::operator[](size_t ind)
{
    return m_data[ind];
}

template <typename Node>
const Node & bTree<Node>::operator[](size_t ind) const
{
    return m_data[ind];
}

template <typename Node>
auto bTree<Node>::begin()
{
    return m_data.begin();
}

template <typename Node>
auto bTree<Node>::end()
{
    return m_data.end();
}

template <typename Node>
size_t bTree<Node>::node2Ind(const Node & node) const
{
    typename decltype(m_data)::const_iterator el;
    if ((el = std::find(m_data.begin(), m_data.end(), node)) != m_data.end())
    {
        return std::distance(m_data.begin(), el);
    }

    throw std::range_error("Node not in tree");
}

template <typename Node>
size_t bTree<Node>::numElems(size_t level) const
{
    return static_cast<size_t>(std::pow(2, level + 1) - 1);
}

template <typename Node>
size_t bTree<Node>::numLevels() const
{
    return m_depth + 1;
}

template <typename Node>
size_t bTree<Node>::goUp(size_t ind) const
{
    if (ind == 0)
    {
        return 0;
    }

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
Node & bTree<Node>::root()
{
    return const_cast<Node &>(const_cast<const bTree<Node> *>(this)->root());
}

template <typename Node>
const Node & bTree<Node>::root() const
{
    return m_data[0];
}

template <typename Node>
Node & bTree<Node>::parent(const Node & node)

{
    return const_cast<Node &>(const_cast<const bTree<Node> *>(this)->parent(node));
}

template <typename Node>
const Node & bTree<Node>::parent(const Node & node) const
{
    return m_data[goUp(node2Ind(node))];
}


template <typename Node>
Node & bTree<Node>::leftchild(const Node & node)
{
    return const_cast<Node &>(const_cast<const bTree<Node> *>(this)->leftchild(node));
}

template <typename Node>
const Node & bTree<Node>::leftchild(const Node & node) const
{
    return m_data[goDownLeft(node2Ind(node))];
}

template <typename Node>
Node & bTree<Node>::rightchild(const Node & node)
{
    return const_cast<Node &>(const_cast<const bTree<Node> *>(this)->rightchild(node));
}

template <typename Node>
const Node & bTree<Node>::rightchild(const Node & node) const
{
    return m_data[goDownRight(node2Ind(node))];
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

        // go right
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
// recombinantBTree
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename Node>
recombinantBTree<Node>::recombinantBTree(size_t depth) : bTree<Node>(depth, numElems(depth))
{}

template <typename Node>
size_t recombinantBTree<Node>::level(size_t ind)
{
    // analytic solution
    return static_cast<size_t>(std::round(std::sqrt(1 + 2 * ind) - 1));
}

template <typename Node>
size_t recombinantBTree<Node>::level_size(size_t ind)
{
    return level(ind) + 1;
}

template <typename Node>
size_t recombinantBTree<Node>::left_boundary(size_t level)
{
    return arithm_sum(level, 1ul, 1ul);
}

template <typename Node>
size_t recombinantBTree<Node>::right_boundary(size_t level)
{
    return arithm_sum(level + 1, 1ul, 1ul) - 1;
}

template <typename Node>
size_t recombinantBTree<Node>::numElems(size_t level) const
{
    // arithmetic sum w 0-based indexing
    return arithm_sum(level + 1, 1ul, 1ul);
}

template <typename Node>
size_t recombinantBTree<Node>::goUp(size_t ind) const
{
    return goUpLeft(ind);
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
const Node & recombinantBTree<Node>::parent(const Node & node) const
{
    return parentLeft(node);
}

template <typename Node>
Node & recombinantBTree<Node>::parentLeft(const Node & node)
{
    return const_cast<Node &>(const_cast<const recombinantBTree<Node> *>(this)->parentLeft(node));
}

template <typename Node>
const Node & recombinantBTree<Node>::parentLeft(const Node & node) const
{
    return super::m_data[goUpLeft(super::node2Ind(node))];
}

template <typename Node>
Node & recombinantBTree<Node>::parentRight(const Node & node)
{
    return const_cast<Node &>(const_cast<const recombinantBTree<Node> *>(this)->parentRight(node));
}

template <typename Node>
const Node & recombinantBTree<Node>::parentRight(const Node & node) const
{
    return super::m_data[goUpRight(super::node2Ind(node))];
}

template <typename Node>
std::vector<size_t> recombinantBTree<Node>::copySubTreeLeft(size_t indS, size_t indT)
{
    std::vector<size_t> ret{};

    if (!(level(indS) == level(indT)))
    {
        throw std::range_error("Source and target nodes must be on the same level!");
    }

    // non-recursive implementation:
    // this is equal to setting the last value on the level to the value preceding it for all the levels below
    // and including the initial
    size_t l = level(indS);
    size_t last = right_boundary(l);

    while (last < super::m_data.size())
    {
        super::m_data[last] = super::m_data[last - 1];

        // proceed to the next level
        ++l;
        last = right_boundary(l);
    }

    return ret;
}

template <typename Node>
std::vector<size_t> recombinantBTree<Node>::copySubTreeRight(size_t indS, size_t indT)
{
    std::vector<size_t> ret{};

    if (!(level(indS) == level(indT)))
    {
        throw std::range_error("Source and target nodes must be on the same level!");
    }

    // non-recursive implementation
    // this is equal to setting the whole level to the left value below source for all the levels
    // below and including the initial
    size_t l = level(indS);
    size_t offset = indS - left_boundary(l);
    size_t source = indS;

    while (source < super::m_data.size())
    {
        auto start = super::m_data.begin() + source + 1;
        // fill to the end of the level - std::algorithms use a half-open bracket
        auto end = super::m_data.begin() + left_boundary(l + 1);

        std::fill(start, end, super::m_data[source]);

        // proceed to the next level
        ++l;
        source = left_boundary(l) + offset;
    }

    return ret;
}

template <typename Node>
std::unique_ptr<std::unordered_set<size_t>>
recombinantBTree<Node>::copySubTreeLeft(size_t indS, size_t indT, std::vector<size_t> & target_indices,
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
    if (sourceLeft < super::m_data.size() && sourceLeft > 0 && targetLeft < super::m_data.size() && targetLeft > 0 &&
        pSeen->find(targetLeft) == pSeen->end())
    {
        pSeen = copySubTreeLeft(sourceLeft, targetLeft, target_indices, std::move(pSeen));

        // go right
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

template <typename Node>
void recombinantBTree<Node>::copySubTreeRight(size_t indS, size_t indT, std::vector<size_t> & target_indices)
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

        // go right
        size_t sourceRight = goDownRight(indS);
        size_t targetRight = goDownRight(indT);

        if (sourceRight < super::m_data.size() && sourceRight > 0 && targetRight < super::m_data.size() && targetRight > 0)
        {
            copySubTreeRight(sourceRight, targetRight, target_indices);
        }
    }

    // return - goes up a level
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// recombinantTTree
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


template <typename Node>
recombinantTTree<Node>::recombinantTTree(size_t depth) : bTree<Node>(depth, numElems(depth))
{}

template <typename Node>
size_t recombinantTTree<Node>::level(size_t ind)
{
    // analytic derivation
    return static_cast<size_t>(std::floor(std::sqrt(ind)));
}

template <typename Node>
size_t recombinantTTree<Node>::level_size(size_t ind)
{
    return 1 + 2 * level(ind);
}

template <typename Node>
size_t recombinantTTree<Node>::left_boundary(size_t level)
{
    return level * level;
}

template <typename Node>
size_t recombinantTTree<Node>::right_boundary(size_t level)
{
    return (level + 1) * (level + 1) - 1;
}

template <typename Node>
size_t recombinantTTree<Node>::numElems(size_t level) const
{
    return (1 + level) * (1 + level);
}

template <typename Node>
size_t recombinantTTree<Node>::goUp(size_t ind) const
{
    return goUpLeft(ind);
}

template <typename Node>
size_t recombinantTTree<Node>::goUpLeft(size_t ind) const
{
    // first 2 cannot go up left
    if (ind == left_boundary(level(ind)) || (ind == left_boundary(level(ind)) + 1))
    {
        throw std::range_error("The node corresponding to the index provided is on the left boundary!");
    }

    return ind - level_size(ind);
}

template <typename Node>
size_t recombinantTTree<Node>::goUpCenter(size_t ind) const
{
    // first & last cannot go up straight
    if (ind == left_boundary(level(ind)) || ind == right_boundary(level(ind)))
    {
        throw std::range_error("The node corresponding to the index provided is on the boundary & cannot go up!");
    }
    return ind - level_size(ind) + 1;
}

template <typename Node>
size_t recombinantTTree<Node>::goUpRight(size_t ind) const
{
    // last 2 cannot go up right
    if (ind == right_boundary(level(ind)) || (ind == right_boundary(level(ind)) - 1))
    {
        throw std::range_error("The node corresponding to the index provided is on the right boundary!");
    }
    return ind - level_size(ind) + 2;
}

template <typename Node>
size_t recombinantTTree<Node>::goDownLeft(size_t ind) const
{
    return ind + level_size(ind);
}

template <typename Node>
size_t recombinantTTree<Node>::goDownCenter(size_t ind) const
{
    return ind + level_size(ind) + 1;
}

template <typename Node>
size_t recombinantTTree<Node>::goDownRight(size_t ind) const
{
    return ind + level_size(ind) + 2;
}

template <typename Node>
const Node & recombinantTTree<Node>::parent(const Node & node) const
{
    return parentLeft(node);
}

template <typename Node>
Node & recombinantTTree<Node>::parentLeft(const Node & node)
{
    return const_cast<Node &>(const_cast<const recombinantTTree<Node> *>(this)->parentLeft(node));
}

template <typename Node>
const Node & recombinantTTree<Node>::parentLeft(const Node & node) const
{
    return super::m_data[goUpLeft(super::node2Ind(node))];
}

template <typename Node>
Node & recombinantTTree<Node>::parentCenter(const Node & node)
{
    return const_cast<Node &>(const_cast<const recombinantTTree<Node> *>(this)->parentCenter(node));
}

template <typename Node>
const Node & recombinantTTree<Node>::parentCenter(const Node & node) const
{
    super::m_data[goUpCenter(super::node2Ind(node))];
}

template <typename Node>
Node & recombinantTTree<Node>::parentRight(const Node & node)
{
    return const_cast<Node &>(const_cast<const recombinantTTree<Node> *>(this)->parentRight(node));
}

template <typename Node>
const Node & recombinantTTree<Node>::parentRight(const Node & node) const
{
    super::m_data[goUpRight(super::node2Ind(node))];
}

template <typename Node>
Node & recombinantTTree<Node>::centerchild(const Node & node)
{
    return const_cast<Node &>(const_cast<const recombinantTTree<Node> *>(this)->centerchild(node));
}

template <typename Node>
const Node & recombinantTTree<Node>::centerchild(const Node & node) const
{
    super::m_data[goDownCenter(super::node2Ind(node))];
}

template<typename Node>
std::vector<size_t> recombinantTTree<Node>::copySubTreeSource(size_t indS, size_t indT)
{
    std::vector<size_t> ret{};

    if (!(level(indS) == level(indT)))
    {
        throw std::range_error("Source and target nodes must be on the same level!");
    }

    // TODO: see notes

//    // non-recursive implementation:
//    // this is equal to setting the last value on the level to the value preceding it for all the levels below
//    // and including the initial
//    size_t l = level(indS);
//    size_t last = right_boundary(l);

//    while (last < super::m_data.size())
//    {
//        super::m_data[last] = super::m_data[last - 1];

//        // proceed to the next level
//        ++l;
//        last = right_boundary(l);
//    }

    return ret;
}
template<typename Node>
std::vector<size_t> recombinantTTree<Node>::copySubTreeTarget(size_t indS, size_t indT)
{
    // TODO: see notes
}

} // namespace cm

#endif // BTREE_H
