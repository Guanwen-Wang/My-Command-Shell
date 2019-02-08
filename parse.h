#include <dirent.h>  //opendir
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>  //execve

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>

#define STR_LENGTH 100

using namespace std;

class parse
{
 public:
  string inputStr;           //input string
  char ** args;              //seperate the input string into args
  vector<char **> sub_args;  //used in pipes
  string ps;

 public:
  parse() : inputStr(NULL), args(NULL) {}
  parse(string str) : inputStr(str), ps(str) { args = new char *[STR_LENGTH]; }
  ~parse() { delete[] args; }
  //when pipe occurs, seperate the whole input string into each process
  //input. Each process input includes its own command and arguments
  void divide_args();
  //parse the input string, and seperate it into char ** args
  void parseString(const char * delimiter);
  //handle the situation when input string contains back-slash space
  void backslashSpace(const char * delimiter, size_t toRepl);
  //handle the situation when input string contains back-slash dollar
  void handleBslashDollar();

  friend class myshell;
  friend class var;
};