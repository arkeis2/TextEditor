#include <iostream>
#include <fstream>
#include <string>
#include <cstddef>
#include <sstream>
#include <vector>
#include <deque>
#include <map>
#include <regex>
#include <readline/readline.h>
#include <readline/history.h>
#include "../include/MemRep.hpp"
#include "../include/Texteditor.hpp"


using namespace std;

static TextEditor* instance = nullptr;

map<State, const char *> InputMap = {
  {State::APPEND, "(append) "},
  {State::INSERT, "(insert %i ) "},
  {State::COMMAND, "(cmd) "}
};

map<string, CmdCode> CmdCodeMap = {
  {"q", CmdCode::QUIT},
  {"w", CmdCode::WRITE},
  {"c", CmdCode::CLEAR},
  {"u", CmdCode::UNDO},
  {"d", CmdCode::DISPLAY},
  {"a", CmdCode::STARTAPPEND}
};

CmdCode GetCmdCode(string CmdCode) {
  static const std::regex regexUndo(R"(^u(?:\s+([0-9]+))?$)");
  smatch match;
  if (regex_match(CmdCode, match, regexUndo)) {
    return CmdCode::UNDO;
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
  fdr->close();
  filename = fname;
  state = State::APPEND;
}

void TextEditor::inputLoop() {
  instance = this;

  init_readline();
  string line;
  while (true) {
    char* input = readline(InputMap[state]);
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
          int count = 0;
          for (int i = 0; i < numLinesToUndo; i++) {
            if (!fileRep->undoLast()) {
              cout << "No more actions to undo" << endl;
              break;
            }
            count++;
          }
          cout << "Undid " << count << " action(s)" << endl;
        }
        break;
        
      case CmdCode::CLEAR:
        fileRep->clearBuffer();
        break;

      case CmdCode::STARTAPPEND:
        state = State::APPEND;
        break;

      case DISPLAY:
        fileRep->displayBuffer();
        break;
      
      default:
        break;
      }
    }
    else if (state == State::APPEND) {
      fileRep->appendLine(line);
    }
  }
}

TextEditor::~TextEditor(){}

int TextEditor::getNumArg(string cmd) {
  static const std::regex regexUndo(R"(^u(?:\s+([0-9]+))?$)");
  int numLinesToUndo = 1; // default to 1 line
  smatch match;
  if (regex_match(cmd, match, regexUndo)) {
    if (match[1].matched) {
      numLinesToUndo = stoi(match[1].str());
    }
  }
  return numLinesToUndo;
}
