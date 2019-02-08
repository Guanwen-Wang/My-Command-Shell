#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <sstream>

using namespace std;

extern char ** environ;

class var
{
 public:
  string inputStr;
  string cmdName;
  string varName;
  string varValue;
  size_t firstblank;
  size_t secondblank;
  static map<string, string> localVar;

 public:
  var() : firstblank(0), secondblank(0) {
    /*copy each environment variable into map localvar*/
    int count = 0;
    while (environ[count]) {
      count++;
    }
    for (int i = 0; i < count; i++) {
      string temp(environ[i]);
      size_t equal = temp.find("=");
      string key = temp.substr(0, equal);
      string value = temp.substr(equal + 1);
      localVar[key] = value;
    }
  }
  var(string str) : inputStr(str) {}
  ~var() {}

  bool checkVarInput(char * command);
  void checkcons(void) {
    cout << "----------" << endl;
    cout << "cmdName: " << cmdName << endl;
    cout << "varName: " << varName << endl;
    cout << "varValue: " << varValue << endl;
    cout << "localVar[" << varName << "]: " << localVar[varName] << endl;
    cout << "----------" << endl;
  }
  bool checkVarName();
  void setVar();
  void exportVar();
  void incVar();
  string getValue(string varName);
  bool replaceVar(char ** args);
  friend class myShell;
  friend class parse;
};
