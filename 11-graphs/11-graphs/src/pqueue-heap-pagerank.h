#pragma once
#include <string>


struct phil {
    int index;
    double val;
};


class HeapPQueuePR {
public:
    HeapPQueuePR();
    ~HeapPQueuePR();
	
    static HeapPQueuePR *merge(HeapPQueuePR *one, HeapPQueuePR *two);
	
    void enqueue(const phil& elem);
    phil extractMin();
    const phil& peek() const;
    int size() const { return logSize; }
    
private:
    // provide data methods and helper methods to
    // help realize the binary heap-backed PQueue
    phil *heap;
    int heapMaxSize;
    int logSize;
    void heapify(const int& index);
    void resize(const int& newLen);
};


