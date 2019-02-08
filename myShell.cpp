#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "myShellClass.h"

using namespace std;
extern char ** environ;

int main() {
  myShell shell;
  shell.run_shell();
  return EXIT_SUCCESS;
}
