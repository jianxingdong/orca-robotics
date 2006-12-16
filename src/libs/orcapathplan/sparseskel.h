/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ORCAPATHPLAN_SPARSE_SKEL_H
#define ORCAPATHPLAN_SPARSE_SKEL_H

#include <orcapathplan/pathplanutils.h>

namespace orcapathplan {

class SparseSkelArc;
class ContiguousSparseSkel;
class SparseSkelNode;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//!
//! @brief A sparse skeleton.
//!
//! Rather than planning over every single cell along the path defined by the skeleton, 
//! SparseSkel stores only a sparse set of waypoints.  Planning is therefore much faster.
//!
//! @author Alex Brooks
//!
class SparseSkel {

public:

    //!
    //! This constructor builds the SparseSkel.
    //! WARNING: this may require significant processing.
    //!
    SparseSkel( const orcaogmap::OgMap &ogMap,
                double                  traversabilityThreshhold,
                const FloatMap         &navMapSkel,
                const Cell2DVector     &skel,
                const FloatMap         &distGrid );

    ~SparseSkel();

    // access
    const std::vector<ContiguousSparseSkel*> &contiguousSkels() const
        { return contiguousSkels_; }
    const orcaogmap::OgMap &ogMap() const { return ogMap_; }

    int numNodes() const;
    double traversabilityThreshhold() const { return traversabilityThreshhold_; } 

private:

    //
    // 'wps' is the set of waypoints: end-points or junctions of the skel.
    //
    void build( const FloatMap &navMapSkel,
                Cell2DList     &skel,
                const FloatMap &distGrid,
                Cell2DList     &wps );

    // If the map has disjoint sections, we may require more than one
    // separate ContiguousSparseSkel.
    std::vector<ContiguousSparseSkel*> contiguousSkels_;

    const orcaogmap::OgMap &ogMap_;
    const double traversabilityThreshhold_;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//
// If the map has disjoint sections, we may require more than one
// separate ContiguousSparseSkel.
//
// Not intended to be instantiated by anything other than SparseSkel.
//
class ContiguousSparseSkel
{
public:

    ContiguousSparseSkel( SparseSkel &parent,
                          Cell2DList &wps,
                          Cell2DList &skel );
    ~ContiguousSparseSkel();

    // access
    const std::vector<SparseSkelNode*> &nodes() const
        { return nodes_; }

    const orcaogmap::OgMap &ogMap() const { return parent_.ogMap(); }
    double traversabilityThreshhold() const { return parent_.traversabilityThreshhold(); }

    bool isSane() const;

private:

    // This function grows outwards along the skel from 'fromNode',
    // finding and adding all directly-connected neighbours.  It will
    // modify its arguments, adding to 'ungrownCells',
    // and removing from and 'skel'
    void growNode( SparseSkelNode *fromNode,
                   Cell2DList     &ungrownCells,
                   Cell2DList     &wps,
                   Cell2DList     &skel );

    // Search through the skel to find the next node in this
    // direction. findNeighbourNode deletes all visited
    // members of skel.  Visited members of skel get put in
    // cellsEnRoute.
    bool findNeighbourNode( const SparseSkelNode *fromNode,
                            const Cell2D         &startCell,
                            Cell2D               &neighbourPos,
                            Cell2DVector         &cellsEnRoute,
                            Cell2DList           &wps,
                            Cell2DList           &skel );

    SparseSkelNode* findNode( const Cell2D &pos );

//     bool selectCell( const Cell2D &fromCell,
//                      Cell2D       &selected,
//                      Cell2DList   &wps,
//                      Cell2DList   &skel,
//                      int          &i );

    // merge adjacent nodes.
    void optimise();

    // return true if it's ok to merge.
    bool canMerge( SparseSkelNode *slave, SparseSkelNode *master );
    // replace all references to slave with refs to master.
    void merge( SparseSkelNode *slave, SparseSkelNode *master );

    // Returns the node that it arced to (possibly after creating it)
    // Note that cellsEnRoute doesn't include either end-point.
    SparseSkelNode *createArc( SparseSkelNode *fromNode, 
                               Cell2D         &neighbourNodePos,
                               Cell2DVector   &cellsEnRoute );

    std::vector<SparseSkelNode*>  nodes_;
    SparseSkel                   &parent_;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//
// Not intended to be instantiated by anything other than SparseSkel.
//
class SparseSkelNode {

public:

    SparseSkelNode( const Cell2D &nodePos )
        : pos(nodePos) {}
    ~SparseSkelNode();

    Cell2D                       pos;
    std::vector<SparseSkelArc*>  arcs;

    // Temporary used for searching
    mutable float                nodeCost;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//
// Not intended to be instantiated by anything other than SparseSkel.
//
class SparseSkelArc {

public:

    SparseSkelArc( SparseSkelNode *initToNode, float initCost )
        : toNode(initToNode), cost(initCost) {}

    SparseSkelNode *toNode;
    float           cost;

};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Non-member functions
void findShortestPath( const ContiguousSparseSkel &cSkel,
                       const SparseSkelNode *start,
                       const SparseSkelNode *goal,
                       std::vector<const SparseSkelNode *> &path );

inline float distance( const Cell2D &a, const Cell2D &b )
{
#ifdef __QNX__
    return std::hypotf( a.x()-b.x(), a.y()-b.y() );
#else
    return hypotf( a.x()-b.x(), a.y()-b.y() );
#endif
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// helper function for debugging.
void printCellList( const Cell2DList &cellList );

}

#endif
