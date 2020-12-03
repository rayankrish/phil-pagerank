/**
 * File: graphs.h
 * --------------
 * Presents the definition of a simple graph data type and the
 * subsidiary node and arc types.
 */

#pragma once
#include <string>
#include "set.h"
#include "map.h"

/**
 * Type: node
 * ----------
 * Defines the node type that participates with the larger graph type.
 * Each node has a name, a location within a graphics window, and a
 * collection of arcs emanating away from it.
 */
struct node {
    std::string name;
    int x, y;
    Set<struct arc *> arcs;
};

/**
 * Type: arc
 * ---------
 * Defines the arc type that participates with the larger graph type.  Each
 * arc knows how much it costs to travel it, and it knows the two nodes it
 * connects.
 */
struct arc {
    double cost;
    node *from, *to;
};

/**
 * Type: graph
 * -----------
 * Defines a simple graph that knows of its nodes and its edges/arcs at all
 * times.
 */
struct graph {
    Set<node *> nodes;
    Set<arc *> arcs;
    Map<std::string, node *> index;
};
