// clang++ --std=c++14 -framework OpenGL -DGL_SILENCE_DEPRECATION init.cpp

#include <OpenGL/OpenGL.h>
#include <OpenGL/gl3.h>
#include <cassert>
#include <cstdio>
#include <string>
#include <vector>

#include "init.h"

int main() {
  CreateGlContext();

  // -
  // Create a basic RGBA8 backbuffer.

  GLuint backbuffer_rb = 0;
  glGenRenderbuffers(1, &backbuffer_rb);
  glBindRenderbuffer(GL_RENDERBUFFER, backbuffer_rb);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, 1, 1);

  GLuint backbuffer_tex = 0;
  glGenTextures(1, &backbuffer_tex);
  glBindTexture(GL_TEXTURE_2D, backbuffer_tex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

  GLuint backbuffer_fb = 0;
  glGenFramebuffers(1, &backbuffer_fb);
  glBindFramebuffer(GL_FRAMEBUFFER, backbuffer_fb);
  //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, backbuffer_rb);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, backbuffer_tex, 0);
  assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
  glViewport(0, 0, 1, 1);

  // -
  // Power On Self Test:

  glClearColor(0.125, 0.25, 0.375, 0.5);
  glClear(GL_COLOR_BUFFER_BIT);

  {
    uint32_t p = 0;
    glReadPixels(0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &p);
    printf("%08x\n", p);
    assert(p == 0x80604020);
    //assert(p == 0xff604020);
  }

  // Ignore teardown.
  assert(!glGetError());
  printf("Done.\n");
  return 0;
}
