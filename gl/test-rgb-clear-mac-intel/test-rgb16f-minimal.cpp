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

  // -
  // Create a basic RGBA8 backbuffer.

  GLuint backbuffer_fb = 0;
  glGenFramebuffers(1, &backbuffer_fb);
  glBindFramebuffer(GL_FRAMEBUFFER, backbuffer_fb);

  //GLuint backbuffer_rb = 0;
  //glGenRenderbuffers(1, &backbuffer_rb);
  //glBindRenderbuffer(GL_RENDERBUFFER, backbuffer_rb);
  //glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, 1, 1);
  //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, backbuffer_rb);

  GLuint backbuffer_tex = 0;
  glGenTextures(1, &backbuffer_tex);
  glBindTexture(GL_TEXTURE_2D, backbuffer_tex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, backbuffer_tex, 0);

  assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
  glViewport(0, 0, 1, 1);
  assert(!glGetError());

  // -

  GLuint vao = 0;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  assert(!glGetError());

  GLuint half_float_tex = 0;
  glGenTextures(1, &half_float_tex);
  glBindTexture(GL_TEXTURE_2D, half_float_tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 2, 2, 0, GL_RGB, GL_HALF_FLOAT, nullptr);

  GLuint half_float_fb = 0;
  glGenFramebuffers(1, &half_float_fb);
  glBindFramebuffer(GL_FRAMEBUFFER, half_float_fb);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, half_float_tex, 0);
  assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
  assert(!glGetError());

  const bool INCUR_DRIVER_BUG = true;
  const bool WORKAROUND_DRIVER_BUG = false;

  if (INCUR_DRIVER_BUG) {
    if (WORKAROUND_DRIVER_BUG) {
      //glClearColor(0.000000001, 0, 0, 1); // no bug
      glClearColor(0, 0, 0, 0.5); // no bug
    } else {
      //glClearColor(0, 0, 0, 0); // bug
      glClearColor(0, 0, 0, 1); // bug
    }
    glClear(GL_COLOR_BUFFER_BIT);
  }
  glClearColor(10000, 10000, 10000, 5000);
  glClear(GL_COLOR_BUFFER_BIT);
  {
    float p[4] = {};
    glReadPixels(0, 0, 1, 1, GL_RGBA, GL_FLOAT, p);
    printf("%f %f %f %f\n", p[0], p[1], p[2], p[3]);
    assert(p[0] == 10000);
    assert(p[1] == 10000);
    assert(p[2] == 10000);
    assert(p[3] == 1);
  }
  glDeleteFramebuffers(1, &half_float_fb);

  const char* vs_src = R"(
#version 410
void main() {
  gl_Position = vec4(0, 0, 0, 1);
}
)";
  const char* fs_tex_src = R"(
#version 410
uniform sampler2D u_sampler;
out vec4 o_FragColor;
void main() {
  o_FragColor = texture(u_sampler, vec2(0)) / 20000.0;
}
)";
  const auto prog = make_program(vs_src, fs_tex_src);
  glUseProgram(prog);
  assert(!glGetError());

  glBindFramebuffer(GL_FRAMEBUFFER, backbuffer_fb);
  glDrawArrays(GL_POINTS, 0, 1);
  assert(!glGetError());

  {
    uint32_t p = 0;
    glBindFramebuffer(GL_FRAMEBUFFER, backbuffer_fb);
    glReadPixels(0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &p);
    printf("%08x\n", p);
    assert((p & 0xff000000) == 0x00000000);
  }

  // Ignore teardown.
  assert(!glGetError());
  printf("Done.\n");
  return 0;
}
