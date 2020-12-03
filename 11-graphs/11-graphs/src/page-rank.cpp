/**
 * File: page-rank.cpp
 * --------------------------
 * Implements Page-Rank algorithm using graphs
 *
 */

#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include "graphs.h"
#include "graph-display.h"
#include "graph-constants.h"
#include "priorityqueue.h"
#include "console.h"
#include "simpio.h"
#include "gevents.h"
#include "set.h"
#include "hashset.h"
#include "grid.h"
#include "pqueue.h"
#include <cmath>
//#include "pqueue-heap-pagerank.h"
using namespace std;

// makes a hashset based on each line in fileName
HashSet<string> buildEntities(const string& fileName) {
    ifstream stream;
    stream.open(fileName.c_str());
    string line;
    HashSet<string> set;
    while (getline(stream, line)) {
        set.add(line);
    }
    return set;
}

// uses the hashset and a file to construct a graph with entites as nodes
// connections are bidirectional
graph buildGraph(const string& fileName, const HashSet<string>& set) {
    ifstream stream;
    stream.open(fileName.c_str());
    string line;
    graph g;
    while (getline(stream, line)) {
        // vector of words in line
        // prevents cases in which false entities are deliniated by space
        Vector<string> lineVec;
        int loc = line.find(" "); //reduces redundant lookups
        while (loc>0) {
            lineVec.add(line.substr(0, loc));
            line = line.substr(loc+1);
            loc = line.find(" ");
        }
        lineVec.add(line);
        cout << lineVec << endl;

        Set<string> subset;
        for (string str : lineVec) if (set.contains(str)) subset.add(str);

        // ensures all the found entities are in graph
        for (string str : subset) {
            if (!g.index.containsKey(str)) {
                node * n = new node();
                n->name = str;
                g.index.put(str, n);
                g.nodes.add(n);
            }
        }

        Set<string> remaining = subset;
        for (string one : subset) {
            for (string two : remaining) {
                if (two!=one) {
                    arc *forward = new arc;
                    arc *backward = new arc;

                    forward->from = backward->to = g.index.get(one);
                    backward->from = forward->to = g.index.get(two);
                    forward->cost = backward->cost = 1;

                    g.index.get(one)->arcs.add(forward);
                    g.index.get(two)->arcs.add(backward);
                    g.arcs.add(forward);
                    g.arcs.add(backward);
                }
            }
            remaining.remove(one);
        }

        cout << endl;
    }
    return g;
}

// builds a graph using the wikipedia reference structure
// references are directional
graph buildWikipediaGraph(const string& fileName, const HashSet<string>& set) {
    ifstream stream;
    stream.open(fileName.c_str());
    string line;
    graph g;
    for (string str : set) {
        if (!g.index.containsKey(str)) {
            node * n = new node();
            n->name = str;
            g.index.put(str, n);
            g.nodes.add(n);
        }
    }
    int count = 1;
    while(getline(stream, line)) {
        node * n = g.index.get(line);
        if (n==nullptr) { // this will never happen when processSet isn't run
            getline(stream, line);
            continue;
        }
        if (getline(stream, line)) {

        } else {
            break;
        }

        Vector<string> tokens;
        stringstream check(line);

        string item;
        if (getline(check, item, ','))
            tokens.add(item);
        while(getline(check, item, ',')){
            if (item.length()>1)
                tokens.add(item.substr(1));
            else
                tokens.add(item);
        }

        for (string str : tokens) {
            // adds a connection if this node refers to another node
            if (set.contains(str)) {
                arc *forward = new arc;

                forward->from = n;
                forward->to = g.index.get(str);
                forward->cost = 1;

                n->arcs.add(forward);
                g.arcs.add(forward);
            }
        }
    };
    return g;
}

// builds a Markov matrix from a graph
// bias will average the markov with a steady state
Grid<double> makeMarkov(const graph& g, const double& bias = 0.15) {
    Vector<string> node_order = g.index.keys();
    Map<string, int> nodeIndex; int count = 0;
    for (string str : g.index.keys()) {
        nodeIndex.put(str, count); count++;
    }
    Grid<double> matrix = Grid<double>(g.nodes.size(), g.nodes.size(), 0);

    for (int i = 0; i < matrix.numCols(); i++) { // i is col
        Set<arc *> connections = g.index.get(node_order[i])->arcs;
        int numConnects = connections.size();
        for (arc* arc : connections) {
            int row = nodeIndex.get(arc->to->name);
            matrix.set(row, i, matrix.get(row, i)+(1.0/numConnects));
        }
    }
    double b = (1.0/matrix.numRows()) * bias;
    for (int i = 0; i < matrix.numCols(); i++) {
        for (int j = 0; j < matrix.numRows(); j++) {
            matrix.set(j, i, (matrix.get(j, i)*(1-bias)) + b);
        }
    }
    return matrix;
}

// returns the dot product of the given row and col of nxn matrix
double dotProduct(Grid<double> grid, int row, int col) { // may need to pass by reference to save time
    double sum;
    for (int i = 0; i < grid.numRows(); i++) {
        sum += (grid.get(i, col) * grid.get(row, i));
    }
    return sum;
}

// squares matrix usind dot product function
Grid<double> squareMatrix(const Grid<double>& grid) {
    Grid<double> newGrid = Grid<double>(grid.numRows(), grid.numCols(), 0);
    for (int i = 0 ; i < grid.numRows(); i++) {
        for (int j = 0; j < grid.numCols(); j++) {
            double dp = dotProduct(grid, i, j);
            //cout << i << " " << j << dp << endl;
            newGrid.set(i, j, dp);
        }
    }
    return newGrid;
}

// multiplies left times right
Grid<double> multiplyMatrices(const Grid<double>& one, const Grid<double>& two) {
    Grid<double> prod = Grid<double>(one.numRows(), two.numCols(), 0);
    if (one.numCols()!=two.numRows()) {
        cout << "Invalid Matrix Multiplication!" << endl;
        return prod;
    }
    cout << "Doing matrix multiplication! This takes time. Your multiplication will produce " << prod.numRows() << " rows." << endl;;
    cout << "We will print one . for each 50 rows calculated and you can expect " << prod.numRows()/50 << " .s\n" << endl;
    for (int i = 0; i < prod.numRows(); i++) {
        for (int j = 0; j < prod.numCols(); j++) {
            double sum = 0;
            // dot product
            for(int k = 0; k < one.numCols(); k++) {
                sum+=(one.get(i, k) * two.get(k, j));
            }
            prod.set(i, j, sum);
        }
        if (i%50==0)
            cout << "." << endl;
    }
    cout << endl;
    return prod;
}

// iteratively multiplies grid by itself count times
// 1 returns itself, 2 squares, 3 cubes...
Grid<double> iterateMultiply(const Grid<double>& grid, const int& count) {
    Grid<double> output = grid;
    for (int i = 1; i < count; i++) {
        output = multiplyMatrices(grid, output);
    }
    return output;
}

// takes graph and grid, printing sorted first column's top 100 values (by default)
void getRank(const graph& g, const Grid<double>& grid, const int& topVals=100) {
    PriorityQueue<int> pq;
    Stack<int> colSorted;
    for (int i = 0 ; i < grid.numRows(); i++) {
        pq.add(i, grid.get(i, 0));
    }
    Vector<string> node_order = g.index.keys();
    int count = 0;
    while (!pq.isEmpty()) {
        int index = pq.dequeue();
        colSorted.push(index);
        count++;
    }
    for (int i = 1 ; i <= topVals; i++) {
        if (!colSorted.isEmpty()) {
            int index = colSorted.pop();
            cout << i <<  " - " << node_order.get(index) << "     " << grid.get(index, 0) << endl;
        } else {
            return;
        }
    }
}

// calculates the euclidean distance between the first two columns
double euclidDistance(const Grid<double>& grid) {
    double error = 0.0;
    for (int i = 0; i < grid.numRows(); i++) {
        error += pow((grid.get(i, 0)-grid.get(i, 1)), 2);
    }
    return pow(error, 0.5);
}

//repeatedly squares by copying into memory rather than computing
Grid<double> logIterateMultiply(const Grid<double>& grid, const int& count, const graph& g) {
    Grid<double> one = grid;
    Grid<double> two = grid;
    cout << "Error 0: " << euclidDistance(one) << endl;
    for (int i = 0; i < count; i++) {
        one = multiplyMatrices(one, two);
        two = one;
        cout << "Error " << i+1 << ": " << euclidDistance(one) << endl;
        getRank(g, one);
    }
    return one;
}

// data processing to remove extraneous pages
void processSet(HashSet<string>& set) {
    set.remove("International Standard Book Number");
    set.remove("Philosophy");
    set.remove("Philosopher");
    set.remove("Stanford Encyclopedia of Philosophy");
    set.remove("Glossary of philosophy");
    set.remove("Index of philosophy");
    set.remove("Index of philosophy articles (Aâ€“C)");
    set.remove("Index of philosophy articles (Dâ€“H)");
    set.remove("Index of philosophy articles (Iâ€“Q)");
    set.remove("Index of philosophy articles (Râ€“Z)");
    set.remove("Index of philosophy articles (A-C)");
    set.remove("Index of philosophy articles (D-H)");
    set.remove("Index of philosophy articles (I-Q)");
    set.remove("Index of philosophy articles (R-Z)");
    set.remove("List of philosophers");
    set.remove("List of philosophers (Aâ€“C)");
    set.remove("List of philosophers (Dâ€“H)");
    set.remove("List of philosophers (Iâ€“Q)");
    set.remove("List of philosophers (Râ€“Z)");
    set.remove("List of philosophers (A-C)");
    set.remove("List of philosophers (D-H)");
    set.remove("List of philosophers (I-Q)");
    set.remove("List of philosophers (R-Z)");
    set.remove("Routledge Encyclopedia of Philosophy");
    set.remove("The Cambridge Dictionary of Philosophy");
    set.remove("The Oxford Companion to Philosophy");
    set.remove("London");
    set.remove("Chicago");
    set.remove("Dublin");
    set.remove("Bristol");
    set.remove("United Kingdom");
    set.remove("Ireland");
    set.remove("Amsterdam");
    set.remove("Netherlands");
    set.remove("Kingdom of the Netherlands");
    set.remove("Worsley, UK");
    set.remove("Woonsocket, Rhode Island");
    set.remove("Woburn, Massachusetts");
    set.remove("Winston-Salem, North Carolina");
    set.remove("Winona, Minnesota");
    set.remove("Dow Jones Industrial Average");
    set.remove("Dow Jones & Company");
    set.remove("S&P Dow Jones Indices");
    set.remove("Common stock");
    set.remove("Market capitalization");
    set.remove("Stock market index");
    set.remove("S&P 500 Index");
    set.remove("History of programming language");
    set.remove("Generational list of programming languages");
    set.remove("Programming language");
    set.remove("Comparison of programming languages");
    set.remove("Digital object identifier");
    set.remove("International Standard Serial Number");
    set.remove("ISO 3166-1");
    set.remove("Wayback Machine");
    set.remove("Sovereign state");
    set.remove("Dependent territory");
    set.remove("BBC News");
    set.remove("List of states with limited recognition");
    set.remove("International Monetary Fund");

}

void wikipedaPR() {
    //read entities.txt and make set
    HashSet<string> set = buildEntities("high-budget-names.txt");
    processSet(set);

    //read input.txt and make graph
    graph g = buildWikipediaGraph("high-budget.txt", set);

    //make markov
    Grid<double> matrix = makeMarkov(g);
    cout << "Made the Markov Matrix!!" << endl;

    // iterate on matrix by squaring it n times
    Grid<double> iterate = logIterateMultiply(matrix, 8, g);
    cout << "Finished Iteration!!" << endl;

}

int main() {

    wikipedaPR();

    return 0;
}
