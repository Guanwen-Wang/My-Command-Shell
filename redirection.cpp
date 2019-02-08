#include "redirection.h"

int redirection::find_dir(char ** args) {
  int i;
  for (i = 0; args[i] != NULL; i++) {
    if (strcmp(args[i], "<") == 0 || strcmp(args[i], ">") == 0 || strcmp(args[i], "2>") == 0) {
      return i;
    }
  }
  return -1;
}

/*
bool redirection::initFileHelper(char ** args, int r, char * file) {
  if (args[r + 1] == NULL) {
    cout << "You should provide the input file!" << endl;
    return false;
  }
  string temp(args[r + 1]);
  file = new char[temp.length() + 1];
  strcpy(file, args[r + 1]);
  file[temp.length()] = '\0';
  return true;
}
*/

bool redirection::initFile(char ** args, int r) {
  if (strcmp(args[r], "<") == 0) {
    if (args[r + 1] == NULL) {
      cout << "You should provide the input file!" << endl;
      return false;
    }
    string temp(args[r + 1]);
    inputFile = new char[temp.length() + 1];
    strcpy(inputFile, args[r + 1]);
    inputFile[temp.length()] = '\0';
  }
  if (strcmp(args[r], ">") == 0) {
    if (args[r + 1] == NULL) {
      cout << "You should provide the input file!" << endl;
      return false;
    }
    string temp(args[r + 1]);
    outputFile = new char[temp.length() + 1];
    strcpy(outputFile, args[r + 1]);
    outputFile[temp.length()] = '\0';
  }
  if (strcmp(args[r], "2>") == 0) {
    if (args[r + 1] == NULL) {
      cout << "You should provide the input file!" << endl;
      return false;
    }
    string temp(args[r + 1]);
    errorFile = new char[temp.length() + 1];
    strcpy(errorFile, args[r + 1]);
    errorFile[temp.length()] = '\0';
  }

  return true;
}

bool redirection::isredirect(void) {
  if (inputFile != NULL) {
    int fd = open(inputFile, O_RDONLY);
    if (fd == -1) {
      cout << "cannot open redirection file" << endl;
      freeFiles();
      return false;
    }
    dup2(fd, STDIN_FILENO);  //STDIN_FILENO = 0
    close(fd);
  }
  if (outputFile != NULL) {
    int fd = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
    if (fd == -1) {
      cout << "cannot open redirection file" << endl;
      freeFiles();
      return false;
    }
    dup2(fd, STDOUT_FILENO);  //STDOUT_FILENO = 1
    close(fd);
  }
  if (errorFile != NULL) {
    int fd = open(errorFile, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
    if (fd == -1) {
      cout << "cannot open redirection file" << endl;
      freeFiles();
      return false;
    }
    dup2(fd, STDERR_FILENO);  //STDERR_FILENO = 2
    close(fd);
  }
  return true;
}

void redirection::freeFiles(void) {
  free(inputFile);
  free(outputFile);
  free(errorFile);
}