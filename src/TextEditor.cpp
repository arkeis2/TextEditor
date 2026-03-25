#include <iostream>
#include <fstream>
#include <string>
#include <cstddef>
#include <sstream>
#include <vector>
#include <deque>
#include <map>
#include <regex>
#include<format>
#include <readline/readline.h>
#include <readline/history.h>
#include "../include/MemRep.hpp"
#include "../include/Texteditor.hpp"


using namespace std;

static TextEditor* instance = nullptr;

map<State,  function<string(int)>> InputMap = {
  {State::APPEND, [](int line) {return "(append at line " + to_string(line) + ") ";}},
  {State::INSERT, [](int line) {return "(insert at line " + to_string(line) + ") ";}},
  {State::COMMAND, [](int x) {(void)x; return "(cmd) ";}}
};

map<string, CmdCode> CmdCodeMap = {
  {"q", CmdCode::QUIT},
  {"w", CmdCode::WRITE},
  {"c", CmdCode::CLEAR},
  {"u", CmdCode::UNDO},
  {"d", CmdCode::DISPLAY},
  {"a", CmdCode::STARTAPPEND},
  {"i", CmdCode::STARTINSERT}
};

CmdCode GetCmdCode(string CmdCode) {
  static const std::regex regexUndo(R"(^u(?:\s+([0-9]+))?$)");
  static const std::regex regexInsert(R"(^i(?:\s+([0-9]+))?$)");
  smatch match;
  if (regex_match(CmdCode, match, regexUndo)) {
    return CmdCode::UNDO;
  }
  if (regex_match(CmdCode, match, regexInsert)) {
    return CmdCode::STARTINSERT;
  }
  if (CmdCodeMap.count(CmdCode)) return CmdCodeMap[CmdCode];
  return CmdCode::NOOP;
}

int TextEditor::enterCmd(int count, int key) {
  (void) count;
  (void)key;
  state = State::COMMAND; 
  cout << endl;
  rl_replace_line("\n", 0);   // clear current input
  rl_on_new_line();         // move to new line
  //rl_redisplay();           // redraw prompt
  rl_done = 1;
  return 0;
}

int TextEditor::enterCmdHandler(int count, int key) {
  return instance->enterCmd(count, key);
}

void TextEditor::init_readline() {
  rl_bind_key(':', enterCmdHandler);
}

TextEditor::TextEditor(string fname, size_t undoDequeSize) {
  fstream *fdr = new fstream(fname);
  fileRep = new MemRep(fdr, undoDequeSize);
  pointerLine = fileRep->buffer.size();
  fdr->close();
  filename = fname;
  state = State::APPEND;
}

void TextEditor::inputLoop() {
  instance = this;

  init_readline();
  string line;
  int newPointerLine;
  while (true) {
    char textInput[64];
    strcpy(textInput, (InputMap[state](pointerLine)).c_str()); 
    char* input = readline(textInput);
    if (!input) break; //EOF

    string line(input);
    free(input);
    
    if (!line.length()) {
      continue;
    }
    if (state == State::COMMAND) {
      string cmd = line;
      CmdCode code = GetCmdCode(cmd);
      switch (code)
      {
      case CmdCode::WRITE:
        fd = new ofstream(filename, fstream::out | fstream::trunc);
        if (fd == NULL) return ;
        for (size_t i = 0; i < fileRep->buffer.size(); i++) {
          *fd << fileRep->buffer[i] << endl;
        }
        fd->flush();
        fd->close();
        free(fd);
        break;

      case CmdCode::QUIT:
        delete fileRep;
        return ;

      case CmdCode::UNDO:
        // extract number of lines to undo if provided
        {
          int numLinesToUndo = getNumArg(cmd);
          if (numLinesToUndo == -1) numLinesToUndo = 1;
          int count = 0;
          for (int i = 0; i < numLinesToUndo; i++) {
            if (!fileRep->undoLast()) {
              cout << "No more actions to undo" << endl;
              break;
            }
            count++;
          }
          cout << "Undid " << count << " action(s)" << endl;
          pointerLine -= count;
        }
        break;
        
      case CmdCode::CLEAR:
        fileRep->clearBuffer();
        break;

      case CmdCode::STARTAPPEND:
        state = State::APPEND;
        break;

      case CmdCode::DISPLAY:
        fileRep->displayBuffer();
        break;

      case CmdCode::STARTINSERT:
        newPointerLine = getNumArg(cmd);
        if (newPointerLine != -1) pointerLine = newPointerLine;
        if ((size_t) newPointerLine > fileRep->buffer.size() + 1) {
          pointerLine = fileRep->buffer.size() + 1;
        }
        state = State::INSERT;
        break;
      
      default:
        break;
      }
    }
    else if (state == State::APPEND) {
      fileRep->appendLine(line);
      pointerLine ++;
    }
    else if (state == State::INSERT) {
      fileRep->insertLine(line, pointerLine);
      pointerLine++;
    }
  }
}

TextEditor::~TextEditor(){}

int TextEditor::getNumArg(string cmd) {
  static const std::regex regexUndo(R"(^(u|i)(?:\s+([0-9]+))?$)");
  int numArg = -1; // default to 1 line
  smatch match;
  if (regex_match(cmd, match, regexUndo)) {
    if (match[2].matched) {
      numArg = stoi(match[2].str());
    }
  }
  return numArg;
}
