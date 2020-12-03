/**
 * File: graph-display.h
 * ---------------------
 * Exports a simple class that knows how to draw nodes, arcs,
 * paths, and forests within a larger graphics window.
 */

#pragma once
#include <string>
#include "graphs.h"
#include "gwindow.h"
#include "gobjects.h"

/**
 * Class: GraphDisplay
 * -------------------
 * Exports all of the directives needed to visually realize two
 * key graph algorithms: Dijkstra's shortest path algorithm, and
 * Kruskal's MST algorithm.
 */
class GraphDisplay: private GWindow {
public:
    /**
     * Constructor: GraphDisplay
     * -------------------------
     * Creates and displays and empty graphics window of the specified
     * height and width.
     */
    GraphDisplay(int width, int height);

    /**
     * Method: updateNode
     * ------------------
     * Changes the color of a previously drawn node to be that supplied.
     * By default, the changes only show up when a subsequent call to repaint
     * is made, although a repaint can be forced immediately by passing in true
     * via the third argument.
     */
    void updateNode(const node *n, const std::string& color, bool update = false);

    /**
     * Method: updateArc
     * -----------------
     * Changes the color of a previously drawn arc to be that supplied.
     * By default, the changes only show up when a subsequent call to repaint
     * is made, although a repaint can be forced immediately by passing in true
     * via the third argument.
     */
    void updateArc(const arc *a, const std::string& color, bool update = false);

    /**
     * Method: drawGraph
     * -----------------
     * Draws all of the nodes and arcs in the provided graph using the specified
     * color.  The graph is shown in its entirety just before drawGraph
     * returns.
     */
    void drawGraph(const graph& g, const std::string& color);

    /**
     * Method: highlightPath
     * ---------------------
     * Highlights all of the arcs and their incident nodes in the supplied
     * vector.
     */
    void highlightPath(const Vector<const arc *>& path, const std::string& color);

    /**
     * Method: findNodeAt
     * ------------------
     * Returns the address of a node that overlay the supplied (x, y) coordinate,
     * or NULL if there is no such node.
     */
    const node *findNodeAt(int x, int y) const;

    /**
     * Method: clear
     * -------------
     * Clears the GraphDisplay.
     */
    void clear();

private:
    Map<const node *, GOval *> nodes;
    Map<const arc *, GLine *> arcs;

    void drawNode(const node *n, const std::string& color);
    void drawArc(const arc *a, const std::string& color);
};
