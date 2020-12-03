/**
 * File: graph-display.cpp
 * -----------------------
 * Presents the implementations of the nontrivial methods of the
 * GraphDisplay class.  All of the method implementations are
 * straightforward.
 */

#include "graph-display.h"
#include "graph-constants.h"
using namespace std;

GraphDisplay::GraphDisplay(int width, int height): GWindow(width, height) {
    setRepaintImmediately(false);
}

void GraphDisplay::updateNode(const node *n, const string& color, bool update) {
    if (!nodes.containsKey(n)) return;
    nodes[n]->setFillColor(color);
    nodes[n]->sendToFront();
    if (update) repaint();
}

void GraphDisplay::updateArc(const arc *a, const string& color, bool update) {
    if (!arcs.containsKey(a)) return;
    arcs[a]->setColor(color);
    arcs[a]->sendToFront();
    if (update) repaint();
}

void GraphDisplay::drawGraph(const graph& g, const string& color) {
    for (const arc *a: g.arcs) drawArc(a, color);
    for (const node *n: g.nodes) drawNode(n, color);
    repaint();
}

void GraphDisplay::highlightPath(const Vector<const arc *>& path, const string& color) {
    for (const arc *a: path) updateArc(a, color);
    updateNode(path[0]->from, "Red");
    for (const arc *a: path) updateNode(a->to, color);
    repaint();
}

const node *GraphDisplay::findNodeAt(int x, int y) const {
    for (const node *n: nodes) {
        if (nodes[n]->contains(x, y)) return n;
    }
    return NULL;
}

void GraphDisplay::clear() {
    GWindow::clear();
    for (const node *n: nodes) delete nodes[n];
    for (const arc *a: arcs) delete arcs[a];
    nodes.clear();
    arcs.clear();
}

void GraphDisplay::drawNode(const node *n, const string& color) {
    GOval *circle = new GOval(n->x - kInset, n->y - kInset, 2 * kInset, 2 * kInset);
    circle->setFilled(true);
    circle->setColor("Black");
    circle->setFillColor(color);
    add(circle);
    nodes[n] = circle;
}

void GraphDisplay::drawArc(const arc *a, const string& color) {
    GLine *line = new GLine(a->from->x, a->from->y, a->to->x, a->to->y);
    line->setColor(color);
    line->setLineWidth(2.0);
    add(line);
    arcs[a] = line;
}
