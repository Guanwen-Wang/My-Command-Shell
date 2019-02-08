#include "parse.h"

void parse::divide_args() {
  if (inputStr.find("|") != string::npos) {
    string temp_inputStr(inputStr);
    size_t find = temp_inputStr.find("|");
    while (find != string::npos) {
      string sub = temp_inputStr.substr(0, find);
      char ** temp = new char *[10];
      temp[0] = strtok(strdup(sub.c_str()), " ");
      for (int i = 1; temp[i - 1] != NULL; i++) {
        temp[i] = strtok(NULL, " ");
      }
      sub_args.push_back(temp);
      temp_inputStr.erase(0, find + 2);
      find = temp_inputStr.find("|");
    }
    char ** temp = new char *[temp_inputStr.length() + 1];
    temp[0] = strtok(strdup(temp_inputStr.c_str()), " ");
    for (int i = 1; temp[i - 1] != NULL; i++) {
      temp[i] = strtok(NULL, " ");
    }
    sub_args.push_back(temp);
  }
  else {
    sub_args.push_back(args);
  }
}

//seperate the InputStr into **args, divided by delimiter
void parse::parseString(const char * delimiter) {
  size_t toRepl = inputStr.find("\\ ");
  if (toRepl != string::npos) {
    backslashSpace(delimiter, toRepl);
    handleBslashDollar();
  }
  else {
    const char * temp = ps.c_str();
    char * InputChar = (char *)temp;
    args[0] = strtok(InputChar, delimiter);
    for (int i = 1; args[i - 1] != NULL; i++) {
      args[i] = strtok(NULL, delimiter);
    }
    if (inputStr.find_first_of("\\$") != string::npos) {
      handleBslashDollar();
    }
  }
}

void parse::backslashSpace(const char * delimiter, size_t toRepl) {
  //change "\\ " by "@"
  while (toRepl != string::npos) {
    inputStr.replace(toRepl, 2, "@");
    toRepl = inputStr.find("\\ ", toRepl + 1);
  }

  //seperate InputStr into **args
  char * InputChar = new char[inputStr.length() + 1];
  strcpy(InputChar, inputStr.c_str());
  args[0] = strtok(InputChar, delimiter);
  int i = 0;
  while (args[i] != NULL) {
    i++;
    args[i] = strtok(NULL, delimiter);
  }
  //change "@" back into " "
  for (int i = 1; args[i] != NULL; i++) {
    string temp(args[i]);
    size_t pos = temp.find("@");
    while (pos != string::npos) {
      temp.replace(pos, 1, " ");
      pos = temp.find("@", pos + 1);
    }
    char * res = new char[temp.length() + 1];
    strcpy(res, temp.c_str());
    args[i] = res;
  }
  return;
}

void parse::handleBslashDollar() {
  for (int i = 1; args[i] != NULL; i++) {
    string temp(args[i]);
    size_t found = temp.find("\\");
    if (temp.find("$") != string::npos) {
      size_t found_slashDol = temp.find("\\$");
      while (found_slashDol != string::npos) {
        temp.erase(found_slashDol, 1);
        args[i] = strdup(temp.c_str());
        found_slashDol = temp.find("\\$", found_slashDol + 1);
      }
      size_t found_bslash = temp.find("\\");
      while (found_bslash != string::npos) {
        temp.erase(found_bslash, 1);
        args[i] = strdup(temp.c_str());
        found_bslash = temp.find("\\", found_bslash + 1);
      }
    }
    //if args[i] does not contains "$" but contains "\\"
    else if (found != string::npos) {
      do {
        temp.erase(found, 1);
        args[i] = strdup(temp.c_str());
        found = temp.find("\\", found + 1);
      } while (found != string::npos);
    }
  }
}