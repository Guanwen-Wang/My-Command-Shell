#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
using namespace std;

class redirection
{
 public:
  char * inputFile;
  char * outputFile;
  char * errorFile;

 public:
  redirection() : inputFile(NULL), outputFile(NULL), errorFile(NULL) {}
  int find_dir(char ** args);
  bool initFileHelper(char ** args, int r, char * file);
  bool initFile(char ** args, int i);
  bool isredirect(void);
  void freeFiles(void);
  void filecopy(char ** des, char ** src);
};