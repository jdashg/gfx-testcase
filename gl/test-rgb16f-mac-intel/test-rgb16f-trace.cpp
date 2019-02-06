// clang++ --std=c++14 -framework OpenGL -DGL_SILENCE_DEPRECATION ../init.cpp ../util.cpp

#include <OpenGL/OpenGL.h>
#include <OpenGL/gl3.h>
#include <cassert>
#include <cstdio>
#include <string>
#include <vector>

#include "../init.h"
#include "../util.h"

int main() {
  CreateGlContext();

#include "plugin-container.8.cpp"

  // Ignore teardown.
  assert(!glGetError());
  printf("Done.\n");
  return 0;
}
