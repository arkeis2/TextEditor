#include <vector>
#include <deque>
#include <string>
#include <fstream>
#include <iostream>
#include "../include/MemRep.hpp"
using namespace std;

MemRep::MemRep(fstream *fd, size_t undoDequeSize) {
  string line;
  dequeSize = undoDequeSize;
  while(getline(*fd, line)) {
    buffer.push_back(line);
  }
}
void MemRep::appendLine(string line) {
  if (!line.length()) line = "\n";
  buffer.push_back(line);
  if (indexDeque.size() == dequeSize) {
    indexDeque.pop_front(); // delete oldest elem
  }
  indexDeque.push_back(buffer.size()-1);
}

void MemRep::insertLine(string line, int pointerLine) {
  if (!line.length()) line = "\n";
  buffer.insert(buffer.begin() + pointerLine, line);
} 

void MemRep::clearBuffer() {
  buffer.clear();
}
bool MemRep::undoLast() {
  if (indexDeque.empty()) return false;
  int index = indexDeque.back();
  indexDeque.pop_back();
  buffer.erase(buffer.begin()+index);
  return true;
}
void MemRep::displayBuffer() {
  for (size_t i = 0; i < buffer.size(); i++) {
    cout << buffer[i] << endl;
  }
}
MemRep::~MemRep(){}