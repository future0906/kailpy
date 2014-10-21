#include "common.h"
#include <cstdlib>
#include <cstdio>
void _MyAssert(char* strFile, unsigned uLine)
{
  fflush(stdout);
  fprintf(stderr, "\nAssertion failed: %s, line %u\n",strFile,uLine);
  fflush(stderr);
  exit(-5);
}

