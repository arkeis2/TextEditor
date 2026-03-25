#pragma once

#include <string>
#include <vector>
#include <deque>
#include <fstream>
#include <iostream>
using namespace std;

class MemRep {
public:

    vector<std::string> buffer;

    // stores last added indexes for undo
    deque<int> indexDeque; 
    MemRep(fstream *fd, size_t undoDequeSize);
    ~MemRep();
    void appendLine(std::string line);
    void clearBuffer();
    bool undoLast();
    void displayBuffer();

private:
    size_t dequeSize;

};