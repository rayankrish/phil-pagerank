#include "pqueue-heap-pagerank.h"
#include <iostream>
using namespace std;

HeapPQueuePR::HeapPQueuePR() {
    heap = new phil[4];
    heapMaxSize = 4;
    logSize = 0;
}
HeapPQueuePR::~HeapPQueuePR() {
    delete(heap);
} //should heap be deleted

// returns the smallest value at the root
const phil& HeapPQueuePR::peek() const {
    return heap[0];
}

// switches the pointers for two phil pointers
void swap(phil *a, phil *b) {
    phil temp = *a;
    *a = *b;
    *b = temp;
}

// trickles down the values for a given index
void HeapPQueuePR::heapify(const int& index) {
    // manages trickle down
    int ci = index;
    while (ci*2<logSize) {
        if (ci*2+1<logSize && (heap[ci*2].val<heap[ci].val || heap[ci*2+1].val<heap[ci].val)) {
            if (heap[ci*2].val < heap[ci*2+1].val) {
                swap(heap[ci], heap[ci*2]);
                ci = ci*2;
            } else {
                swap(heap[ci], heap[ci*2+1]);
                ci = ci*2+1;
            }
        } else if (heap[ci*2].val<heap[ci].val) {
            swap(heap[ci], heap[ci*2]);
            ci = ci*2;
        } else {
            break;
        }
    }
}

// creates a new array and copies elements at the desired size
void HeapPQueuePR::resize(const int& newLen) {
    phil *newHeap = new phil[newLen];
    for (int i = 0 ; i < logSize; i++) {
        newHeap[i] = heap[i];
    }
    delete [] heap;
    heap = newHeap;
    heapMaxSize = newLen;
}

// switches the root and the end, then heapifies
// to return the min value
phil HeapPQueuePR::extractMin() {
    phil temp = heap[0];
    heap[0] = heap[logSize-1];
    phil max = phil({-1, INT_MAX});
    heap[logSize-1] = max; //was ""
    logSize--;
    heapify(0);

    return temp;
}

// adds an element and bubbles it up
void HeapPQueuePR::enqueue(const phil& item) {
    if (logSize+1 >= heapMaxSize/2) { // check if should double for amortized at full or half
        resize(heapMaxSize*2);
    }
    heap[logSize] = item;
    int currIndex = logSize;
    logSize++;

    // manages bubble up
    while (currIndex != 0 && heap[currIndex/2].val > heap[currIndex].val) {
        swap(heap[currIndex], heap[currIndex/2]);
        currIndex = currIndex/2;
    }
}

// copies elements from pqs across, then heapifies each level progressively
HeapPQueuePR *HeapPQueuePR::merge(HeapPQueuePR * one, HeapPQueuePR * two) {
    // resizes one, adds all of two one.
    one->resize(one->heapMaxSize+two->heapMaxSize);
    for (int i = 0; i < two->logSize; i++) {
        one->heap[one->logSize] = two->heap[i];
        one->logSize++;
    }

    int start = (one->logSize)/2;
    while (start>0) {
        for (int i = start; i < start*2+1; i++) {
            one->heapify(i);
        }
        start/=2;
    }
    one->heapify(0);

    return one;
}
