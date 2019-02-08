#include "var.h"

#include <string>
map<string, string> var::localVar;

bool var::checkVarInput(char * command) {
  if (strcmp(command, "set") == 0) {
    firstblank = inputStr.find_first_of(" ");
    if (firstblank == string::npos || firstblank == inputStr.length() - 1) {
      cmdName = inputStr;
      varName.assign("null");
      varValue.assign("null");
      cout << "lost var name and value" << endl;
      return false;
    }
    else {
      cmdName = inputStr.substr(0, firstblank);
      inputStr.erase(0, firstblank + 1);
      secondblank = inputStr.find_first_of(" ");
      if (secondblank == string::npos || secondblank == inputStr.length() - 1) {
        varName = inputStr;
        varValue.assign("null");
        cout << "lost var value" << endl;
        return false;
      }
      else {
        varName = inputStr.substr(0, secondblank);
        inputStr.erase(0, secondblank + 1);
        varValue = inputStr;
      }
    }
    return true;
  }

  if (strcmp(command, "export") == 0 || strcmp(command, "inc") == 0) {
    firstblank = inputStr.find_first_of(" ");
    if (firstblank == string::npos || firstblank == inputStr.length() - 1) {
      cout << "You should provide the var name!" << endl;
      return false;
    }
    else {
      cmdName = inputStr.substr(0, firstblank);
      inputStr.erase(0, firstblank + 1);
      varName = inputStr;
      secondblank = inputStr.find_first_of(" ");
      if (secondblank == inputStr.length() - 1) {
        inputStr.erase(secondblank, inputStr.length());
        varName = inputStr;
        return true;
      }
      if (secondblank != string::npos) {
        cout << "More than one arg!" << endl;
        return false;
      }
    }
  }
  return true;
}
bool var::checkVarName() {
  size_t found =
      varName.find_first_not_of("AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz1234567890_");
  if (found != string::npos) {
    cout << "Invalid var name!" << endl;
    return false;
  }
  return true;
}

void var::setVar() {
  localVar[varName] = varValue;
}

void var::exportVar() {
  if (setenv(varName.c_str(), localVar[varName].c_str(), 1) != 0) {
    perror("export failure");
    return;
  }
  else {
    cout << "export success!" << endl;
  }
}

void var::incVar() {
  size_t num = localVar[varName].find_first_not_of("1234567890");
  if (num != string::npos) {
    const char * zero = "0";  //reset to 0
    int numvalue = atoi(zero);
    numvalue++;
    stringstream ss;  //convert int to string
    ss << numvalue;
    localVar[varName] = ss.str();
    //    cout << "localVar[" << varName << "]: " << localVar[varName] << endl;
  }
  else {
    //convert string to int;
    stringstream temp_ss(localVar[varName]);
    int numvalue = 0;
    temp_ss >> numvalue;
    numvalue++;
    //convert int to string
    stringstream ss;
    ss << numvalue;
    localVar[varName] = ss.str();
    // cout << "localVar[" << varName << "]: " << localVar[varName] << endl;
  }
}

bool var::replaceVar(char ** args) {
  for (int i = 0; args[i] != NULL; i++) {
    string argsi(args[i]);
    size_t found = argsi.find("$");
    int k = 1;

    while (found != string::npos) {
      cout << "found $ in args" << endl;
      do {
        string sub = argsi.substr(found + 1, found + k);
        map<string, string>::iterator it = localVar.find(sub);
        if (it != localVar.end()) {  //found the variable in map
          argsi.replace(found, k + 1, it->second);
          args[i] = strdup(argsi.c_str());  //replace the varname
          cout << "variable replaced!" << endl;
          found = argsi.find("$");
          break;
        }
        k++;
      } while (found + k < argsi.length() && args[i][found + k] != '$');
      if (!(found + k < argsi.length() && args[i][found + k] != '$')) {
        cout << "no match variable" << endl;
        return false;
      }
    }
  }

  return true;
}