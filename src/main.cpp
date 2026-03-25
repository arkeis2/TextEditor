#include "../include/Texteditor.hpp"

int main(int argc, char* argv[]) {
    // Load a file if provided as an argument
    if (argc > 1) {
      int undoDequeSize = 5;
      if (argc > 2) undoDequeSize = atoi(argv[2]);
      TextEditor *textEditor = new TextEditor(argv[1], undoDequeSize);
      textEditor->inputLoop();
      delete textEditor;
      return 0;
    }


    return 1;
}