/**
 * File: graph-algorithms.cpp
 * --------------------------
 * Presents some classic graph algorithms that rely on a simplified
 * version of the graphs discussed in Chapter 18.
 */

#include <string>
#include <iostream>
#include <cmath>
#include "graphs.h"
#include "graph-display.h"
#include "graph-constants.h"
#include "priorityqueue.h"
#include "console.h"
#include "simpio.h"
#include "gevents.h"
using namespace std;

enum userChoice {
    kQuit = 0, kShortestPath = 1, kMinimumSpanningTree = 2
};

/**
 * Constant: kLightGray
 * --------------------
 * Defines an incredibly faint shade of gray.
 */
static const string kLightGray = "#eeeeee";

/**
 * Function: distanceBetween
 * -------------------------
 * Computes the distance (in pixels) between the two supplied
 * nodes (each of which knows its location within the graph).
 */
static double distanceBetween(const node *n, const node *m) {
    int dx = n->x - m->x;
    int dy = n->y - m->y;
    return sqrt(dx * dx + dy * dy);
}

/**
 * Function: buildRandomGraph
 * --------------------------
 * Populates a presumbly empty graph with the supplied number of nodes.
 * As each new node is installed into the graph, the new
 * node is bidirectionally connected to each of the previously
 * installed nodes with a probability that depends on the distance
 * between them (the closer they are, the more likely they'll be connected).
 */
static void buildRandomGraph(graph& g, int numNodes) {
    for (int i = 0; i < numNodes; i++) {
        // allocate new node and initialize non-arc data right away
        node *n = new node;
        n->name = integerToString(i);
        n->x = randomInteger(kInset, kWindowWidth - kInset);
        n->y = randomInteger(kInset, kWindowHeight - kInset);
        // now flip biased coins to decide whether or not you're connecting new node to existing ones
        for (node *m: g.nodes) { // for each of the nodes already in the graph
            double distance = distanceBetween(n, m);
            double p = pow(1 - distance/kMaxDistance, 6);
            if (!randomChance(p)) continue;

            // connecting n to m? it's bidirectional
            arc *forward = new arc;
            arc *backward = new arc;

            // wire them to source and destination node, and seed each with Euclidean distance between them
            forward->from = backward->to = n;
            backward->from = forward->to = m;
            forward->cost = backward->cost = distance;

            // update arcs for n and m, and redundantly add them to graph's arcs set
            n->arcs.add(forward);
            m->arcs.add(backward);
            g.arcs.add(forward);
            g.arcs.add(backward);
        }

        g.nodes.add(n);
        g.index.put(n->name, n);
    }
}

/**
 * Function: selectNode
 * --------------------
 * Repeatedly prompts the user to click in the graphics window on
 * a node, which the caveat that the user not select the one supplied
 * via exclude.
 */
static const node *selectNode(GraphDisplay& display, const node *exclude = NULL) {
    string prompt = exclude == NULL ? "Please select a node." : "Please select another node.";
    cout << prompt << endl;
    while (true) {
        GMouseEvent me = waitForEvent(MOUSE_EVENT);
        if (me.getEventType() != MOUSE_CLICKED) continue;
        const node *found = display.findNodeAt(me.getX(), me.getY());
        if (found != NULL && found != exclude) {
            display.updateNode(found, "Red", true);
            return found;
        }
        if (found == NULL) {
            cout << "You missed! Please try again." << endl;
        } else {
            cout << "You already clicked on that one, so select another." << endl;
        }
    }
}

/**
 * Function: disposeGraph
 * ----------------------
 * Accepts a reference to a graph and disposes of all nodes and
 * arcs, which are assumed to be dynamically allocated.  Note that
 * addresses stored in the index field are just copies of the
 * addresses stored in the nodes field.
 */
static void disposeGraph(graph& g) {
    for (node *n: g.nodes) delete n;
    for (arc *a: g.arcs) delete a;
}

/**
 * Function: computePathCost
 * -------------------------
 * Computes and returns the total cost of the supplied
 * path, which is just the sum of its constituent arc costs.
 */
static double computePathCost(const Vector<const arc *>& path) {
    double cost = 0.0;
    for (const arc *a: path) cost += a->cost;
    return cost;
}

/**
 * Function: findShortestPath
 * --------------------------
 * Follows Dijkstra's algorithm to determine the shortest path connecting start and
 * finish in the surrounding graph.
 */
static Vector<const arc *> findShortestPath(GraphDisplay& display,
                                            const node *start, const node *finish) {
    if (start == finish)
        error("findShortestPath should only be called on two different endpoints.");

    PriorityQueue<Vector<const arc *>> pq;
    Map<const node *, double> shortestPathLengths;
    shortestPathLengths[start] = 0.0;

    for (const arc *a: start->arcs) {
        Vector<const arc *> path;
        path += a;
        pq.enqueue(path, computePathCost(path));
    }

    while (!pq.isEmpty()) {
        Vector<const arc *> best = pq.dequeue();
        const node *intermediate = best[best.size() - 1]->to;
        if (shortestPathLengths.containsKey(intermediate)) continue;
        if (intermediate == finish) return best;
        double cost = computePathCost(best);
        shortestPathLengths[intermediate] = cost;
        display.highlightPath(best, "Blue");
        cout << "Exploring the partial path displayed in blue (cost: " << cost << ")" << endl;
        display.highlightPath(best, kLightGray);

        for (const arc *a: intermediate->arcs) {
            if (shortestPathLengths.containsKey(a->to)) continue;
            best += a;
            pq.enqueue(best, computePathCost(best));
            best.remove(best.size() - 1);
        }
    }

    return Vector<const arc *>();
}

/**
 * Function: demonstrateShortestPath
 * ---------------------------------
 * Generates a random graph and then animates the computation of the
 * shortest path between two of the points within the graph.
 */
static void demonstrateShortestPath(GraphDisplay& display) {
    cout << "Generating a random graph... " << flush;
    graph g;
    buildRandomGraph(g, /* numNodes = */ randomInteger(kMinNumNodes, kMaxNumNodes));
    cout << "[done!]" << endl;
    display.drawGraph(g, kLightGray);
    const node *start = selectNode(display);
    const node *finish = selectNode(display, start);
    Vector<const arc *> shortestPath = findShortestPath(display, start, finish);
    if (shortestPath.isEmpty()) {
        cout << "Could not find a path connecting the two endpoints." << endl;
    } else {
        display.highlightPath(shortestPath, "Red");
        cout << "Displaying the shortest path in red (cost: " << computePathCost(shortestPath) << ")." << endl;
    }
    cout << "Now take the next ten seconds to observe. :)" << endl;
    pause(kObserveDelay);
    display.clear();
    disposeGraph(g);
}

/**
 * Function: generateMinimumSpanningTrees
 * --------------------------------------
 * Implements Kruskal's algorithm to construct and display the minimal
 * spanning tree with the supplied graph.
 */
static void generateMinimumSpanningTrees(GraphDisplay& display, graph& g) {
    Vector<graph> forests(g.nodes.size());
    Map<node *, int> memberships;
    int i = 0;
    for (node *n: g.nodes) {
        forests[i].nodes += n; // forest consists of a single node with no edges (a standalone tree)
        memberships[n] = i++;
    }

    PriorityQueue<arc *> pq;
    for (arc *a: g.arcs) pq.enqueue(a, a->cost); // queue edges up, lightest to heaviest

    while (!pq.isEmpty()) {
        arc *lightest = pq.dequeue();
        if (memberships[lightest->from] == memberships[lightest->to]) {
            cout << "Skipping edge (cost: " << lightest->cost << ") that would introduce a cycle." << endl;
        } else {
            cout << "Adding edge (cost: " << lightest->cost << ") to merge two forests." << endl;
            display.updateArc(lightest, "Blue", true);
            int from = memberships[lightest->from], to = memberships[lightest->to];
            forests[from].nodes += forests[to].nodes;
            forests[from].arcs += forests[to].arcs;
            forests[from].arcs += lightest;
            for (node *n: forests[to].nodes) memberships[n] = from;
        }
    }
}

/**
 * Function: demonstrateMinimumSpanningTree
 * ----------------------------------------
 * Placeholder for a function that will ultimately demonstrate either Prim's
 * or Kruskal's MST algorithm.
 */
static void demonstrateMinimumSpanningTree(GraphDisplay& display) {
    cout << "Generating a random graph... " << flush;
    graph g;
    buildRandomGraph(g, /* numNodes = */ randomInteger(kMinNumNodes, kMaxNumNodes));
    cout << "[done!]" << endl;
    display.drawGraph(g, kLightGray);
    generateMinimumSpanningTrees(display, g);
    cout << "Now take the next ten seconds to observe. :)" << endl;
    pause(kObserveDelay);
    display.clear();
    disposeGraph(g);
}

/**
 * Function: printUserOptions
 * --------------------------
 * Prints out the three options the user has. One option is to quit, a second
 * is to demonstrate Dijkstra's shortest path algorithm on a random graph.
 */
static void printUserOptions() {
    cout << "You have the following options: " << endl;
    cout << endl;
    cout << "  " << kQuit << "). Quit" << endl;
    cout << "  " << kShortestPath << "). Generate a random graph and find the shortest path between two endpoints." << endl;
    cout << "  " << kMinimumSpanningTree << "). Generate a random graph and compute the minimum spanning tree." << endl;
    cout << endl;
}

/**
 * Function: getUserChoice
 * -----------------------
 * Repeatedly prompt the user until they cooperate by selecting a valid option.
 */
static userChoice getUserChoice() {
    while (true) {
        printUserOptions();
        int choice = getInteger("What's your selection? ");
        if (choice >= kQuit && choice <= kMinimumSpanningTree) return userChoice(choice);
        cout << "I didn't understand that, so let's try again." << endl;
    }
}

/**
 * Function: main
 * --------------
 * Defines the entry point for the entire program.
 */
int main() {
    setConsoleSize(1000, 300);
    setConsoleFont("Courier-18-Bold");
    GraphDisplay window(kWindowWidth, kWindowHeight);
    while (true) {
        userChoice choice = getUserChoice();
        switch (choice) {
        case kQuit:
            return 0;
        case kShortestPath:
            demonstrateShortestPath(window);
            break;
        case kMinimumSpanningTree:
            demonstrateMinimumSpanningTree(window);
            break;
        default:
            error("Invalid command");
        }
    }
    return 0;
}
