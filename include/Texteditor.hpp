#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <cstddef>
#include <sstream>
#include <vector>
#include <deque>
#include <map>
#include <regex>
#include "MemRep.hpp"

using namespace std;

enum CmdCode {
  QUIT,
  WRITE,
  CLEAR,
  UNDO,
  DISPLAY,
  STARTAPPEND,
  NOOP
};


enum State {
  APPEND,
  INSERT,
  COMMAND
};

CmdCode GetCmdCode(string CmdCode);

class TextEditor {
  public:
    ofstream *fd;
    MemRep *fileRep;
    string filename;
    int currentLine;
    State state;

  int enterCmd(int count, int key);
  static int enterCmdHandler(int count, int key);
  
  TextEditor(string fname, size_t undoDequeSize);

  void inputLoop();
  ~TextEditor();

  private:
    size_t pointerLine;
    size_t pointerRow;

    int getNumArg(string cmd);
    void init_readline();

};
