#include <dirent.h>  //opendir
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>  //execve

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>

#include "parse.h"
#include "redirection.h"
#include "var.h"

using namespace std;

class myShell
{
 private:
  string currWorkDir;
  //  char ** myenv;
  int num_myenv;

 public:
  myShell() {
    //Initialize myShell and set current working directory
    //Also add the environment variable ECE551PATH
    currWorkDir = getenv("PWD");
    string ece551("ECE551PATH");
    string path = getenv("PATH");
    setenv(ece551.c_str(), path.c_str(), 1);
  }
  ~myShell() {}
  /*run_shell: the start-up function
   It contains the while(1) loop to keep myShell always able to 
  start over, printing out the prompt, wainting for input.*/
  void run_shell();
  void print_prompt();  //prints out the prompy
  /* ExecureCmd: the man execute funtion, it first parse the input string
     by using space as delimiter. Then it will check whether the command
     is builtin command, like 'cd', 'set', 'export', 'inc'. Then it will 
     initialize the pipes, for fear that the input string includes the 
     pipe. If pipes are found, the original input will be seperated into 
     several sub_input, which each is the full command and arguments.
     Then it will fork to execuate each process.
*/
  void ExecuteCmd(string cmdStr, var myVar);

  //getfullpath is used to get the full path of command
  void getFullPath(char ** args, var myVar);
  //searchDir is used to search the specific file name in directory
  bool searchDir(const char * cmdName, const char * dirName);
  //buildIn is used to check whether the command is builtin ones and execute the command
  bool builtIn(char ** args, var myVar);
  //changedir is used to execute the command 'cd' to change working directory
  void changeDir(char ** args);
  friend class parse;
  friend class var;
};
