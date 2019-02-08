//get from http://www.cplusplus.com/reference/cstdlib/getenv/?kw=getenv
/* getenv example: getting path */
#include <stdio.h>  /* printf */
#include <stdlib.h> /* getenv */

int main() {
  char * pPath;
  pPath = getenv("HOME");
  if (pPath != NULL)
    printf("The current path is: %s\n", pPath);
  return 0;
}
