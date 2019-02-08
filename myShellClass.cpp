#include "myShellClass.h"

void myShell::run_shell() {
  var myVar;
  while (1) {
    if (cin.eof()) {
      cout << endl;
      return;
    }
    string cmdStr;
    print_prompt();
    getline(cin, cmdStr);
    myVar.inputStr = cmdStr;
    if (strcmp(cmdStr.c_str(), "") == 0) {
      continue;
    }
    else if (strcmp(cmdStr.c_str(), "exit") == 0) {
      return;
    }
    else {
      ExecuteCmd(cmdStr, myVar);
    }
  }
}

void myShell::print_prompt() {
  cout << "myShell:" << currWorkDir << "$ ";
}

void myShell::ExecuteCmd(string cmdStr, var myVar) {
  parse Args(cmdStr);
  Args.parseString(" \t");  //parse the input string devided by space or tab

  pid_t wpid;
  int status;

  ///////////// replace the value if $ found ///////////
  bool isReplaced = myVar.replaceVar(Args.args);
  if (isReplaced == false) {
    cout << "variable not found" << endl;
    return;
  }
  //////////// check whether is builtin ///////////////
  if (strcmp(Args.args[0], "cd") == 0) {
    changeDir(Args.args);
    return;
  }
  if (strcmp(Args.args[0], "set") == 0 || strcmp(Args.args[0], "export") == 0 ||
      strcmp(Args.args[0], "inc") == 0) {
    bool isVarInputValid = myVar.checkVarInput(Args.args[0]);
    if (isVarInputValid == false) {
      return;
    }
    bool isVarnameValid = myVar.checkVarName();
    if (isVarnameValid == false) {
      cout << "Invalid variable name!" << endl;
      return;
    }
    bool isBuiltInCmd = builtIn(Args.args, myVar);
    if (isBuiltInCmd == true) {
      return;
    }
  }

  ///////////// if pipe found, divide args //////////////
  size_t pipe_found = cmdStr.find("|");
  int pipe_num = 0;
  if (pipe_found != string::npos) {
    while (pipe_found != string::npos) {
      pipe_num++;
      pipe_found = cmdStr.find("|", pipe_found + 1);
    }
  }
  Args.divide_args();  //seperate the input string into sub command and arguments
  int pipefd[10];
  int m = 0;

  if (pipe(pipefd) < 0) {
    perror("pipe1");
    exit(EXIT_FAILURE);
  }
  if (pipe(pipefd + 2) < 0) {
    perror("pipe2");
    exit(EXIT_FAILURE);
  }

  if (pipe(pipefd + 4) < 0) {
    perror("pipe3");
    exit(EXIT_FAILURE);
  }
  if (pipe(pipefd + 6) < 0) {
    perror("pipe4");
    exit(EXIT_FAILURE);
  }
  if (pipe(pipefd + 8) < 0) {
    perror("pipe5");
    exit(EXIT_FAILURE);
  }

  Args.sub_args.push_back(NULL);
  while (Args.sub_args[m] != NULL) {  //while loop for each process
    int read = (m - 1) * 2;
    int write = m * 2;
    pid_t cpid = fork();

    if (cpid == -1) {
      perror("fork");
      exit(EXIT_FAILURE);
    }
    else if (cpid == 0) {
      if (pipe_num != 0) {
        if (Args.sub_args[m + 1] != NULL) {  // not the last progress
          if (dup2(pipefd[write + 1], 1) < 0) {
            perror("dup2 stdout error");
            exit(EXIT_FAILURE);
          }
        }
        if (m != 0) {  //not the first process
          if (dup2(pipefd[read], 0) < 0) {
            perror("dup2 stdin error");
            exit(EXIT_FAILURE);
          }
        }
        for (int i = 0; i < 10; i++) {
          close(pipefd[i]);
        }
      }

      getFullPath(Args.sub_args[m], myVar);

      /////// redirection begin //////
      redirection myRedirect;
      int r = myRedirect.find_dir(Args.sub_args[m]);  //find which arg is < > 2>
      if (r != -1) {
        bool isinit = myRedirect.initFile(Args.sub_args[m], r);
        if (isinit == false) {
          cout << "redirection initfile error!" << endl;
          return;
        }
        bool isredir = myRedirect.isredirect();
        if (isredir == false) {
          return;
        }
        for (; Args.sub_args[m][r] != NULL; r++) {
          Args.sub_args[m][r] = NULL;
        }
      }
      //////  redirection end  ///////

      execve(Args.sub_args[m][0], Args.sub_args[m], environ);
      perror("execve");
      exit(EXIT_FAILURE);
    }
    else {  //parent
      do {
        wpid = waitpid(cpid, &status, WUNTRACED | WCONTINUED);
        if (wpid == -1) {
          perror("waitpid");
          exit(EXIT_FAILURE);
        }

        if (WIFEXITED(status)) {
          printf("Program exited with status %d\n", WEXITSTATUS(status));
        }
        else if (WIFSIGNALED(status)) {
          printf("Program was killed by signal %d\n", WTERMSIG(status));
        }
      } while (!WIFEXITED(status) && !WIFSIGNALED(status));
      m++;

      if (pipe_num != 0) {
        close(pipefd[2 * m - 1]);  //close writeEnd and keep readEnd
      }
    }
  }
  if (pipe_num != 0) {
    for (int i = 0; i < 10; i++) {
      close(pipefd[i]);
    }
  }
}

void myShell::getFullPath(char ** args, var myVar) {
  string cmdName(args[0]);  //convert char* to string
  string cmdName_fullpath;
  bool isCmdFound = false;
  if (cmdName.find("/") == string::npos) {  //which means inout cmd is not full path
    string dirListStr = myVar.localVar["ECE551PATH"];
    parse path(dirListStr);
    path.parseString(":");
    for (int i = 0; path.args[i] != NULL; i++) {
      string currDir(path.args[i]);  //convert char * dirListChar[i] to string
      isCmdFound = searchDir(cmdName.c_str(), currDir.c_str());
      if (isCmdFound == true) {
        cmdName_fullpath = currDir + '/' + cmdName;  //add the full path together
        args[0] = strdup(cmdName_fullpath.c_str());  //convert the string full_path to args[0]
        return;
      }
    }
    if (isCmdFound == false) {
      cerr << "Command " << cmdName << " not fount" << endl;
      exit(EXIT_FAILURE);
    }
  }
}

bool myShell::searchDir(const char * cmdName, const char * dirName) {
  DIR * dir = opendir(dirName);
  struct dirent * filename;
  if (dir == NULL) {
    perror("Cannot open directory");
    exit(EXIT_FAILURE);
  }
  while ((filename = readdir(dir)) != NULL) {
    if (strcmp(filename->d_name, cmdName) == 0) {
      closedir(dir);
      return true;
    }
  }
  closedir(dir);
  return false;
}

bool myShell::builtIn(char ** args, var myVar) {
  if (strcmp(args[0], "set") == 0) {
    myVar.setVar();
  }
  else if (strcmp(args[0], "export") == 0) {
    myVar.exportVar();
  }
  else if (strcmp(args[0], "inc") == 0) {
    myVar.incVar();
  }
  else {
    return false;
  }
  return true;
}

void myShell::changeDir(char ** args) {
  if (args[1] == NULL) {  // if command cd has no other arguments
    if (chdir(getenv("HOME")) != 0) {
      perror("Cannot change to home dir");
      exit(EXIT_FAILURE);
    }
    else {
      string homedir = getenv("HOME");
      currWorkDir = homedir;
    }
  }
  else if (args[1] != NULL && args[2] == NULL) {  //cd has path followed
    string path(args[1]);
    const char * toPath = path.c_str();
    if (chdir(toPath) != 0) {
      perror("-bash");
      return;
    }
    else {
      char * temp = get_current_dir_name();
      string currdir(temp);
      currWorkDir = currdir;
      free(temp);
    }
  }
  else if (args[1] != NULL && args[2] != NULL) {
    cout << "-bash: cd: too many arguments" << endl;
    return;
  }
  else {
    perror("cd error");
    return;
  }
}