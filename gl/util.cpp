#include "util.h"

#include <OpenGL/OpenGL.h>
#include <OpenGL/gl3.h>
#include <cassert>
#include <cstdio>
#include <string>
#include <vector>

void dump_program_log(const char* const prefix, const uint32_t prog) {
  std::vector<char> log(1000, 0);
  glGetProgramInfoLog(prog, log.size(), nullptr, log.data());
  printf("%s%s\n", prefix, log.data());
}
void dump_shader_log(const char* const prefix, const uint32_t shader) {
  std::vector<char> log(1000, 0);
  glGetShaderInfoLog(shader, log.size(), nullptr, log.data());
  printf("%s%s\n", prefix, log.data());
}

uint32_t make_program(const char* const vs_src, const char* const fs_src) {
  const auto vs = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vs, 1, &vs_src, nullptr);
  glCompileShader(vs);
  const auto fs = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fs, 1, &fs_src, nullptr);
  glCompileShader(fs);

  const auto prog = glCreateProgram();
  glAttachShader(prog, vs);
  glAttachShader(prog, fs);
  glLinkProgram(prog);

  GLint ok = 0;
  glGetProgramiv(prog, GL_LINK_STATUS, &ok);
  if (!ok) {
    dump_program_log("prog: ", prog);
    dump_shader_log("vs: ", vs);
    dump_shader_log("fs: ", fs);
  }
  glDeleteShader(vs);
  glDeleteShader(fs);
  return prog;
}
