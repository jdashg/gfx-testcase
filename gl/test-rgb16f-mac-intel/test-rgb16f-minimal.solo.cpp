// clang++ --std=c++14 -framework OpenGL -DGL_SILENCE_DEPRECATION

// https://bugzilla.mozilla.org/show_bug.cgi?id=1525480

#include <OpenGL/OpenGL.h>
#include <OpenGL/gl3.h>
#include <cassert>
#include <cstdio>
#include <string>
#include <vector>

void CreateGlContext() {
  const CGLPixelFormatAttribute attribs[] = {
    kCGLPFAAccelerated,
    kCGLPFANoRecovery,
    kCGLPFAOpenGLProfile, CGLPixelFormatAttribute(kCGLOGLPVersion_3_2_Core),
    kCGLPFAAllowOfflineRenderers,
    CGLPixelFormatAttribute(0)
  };

  CGLPixelFormatObj pixel_format = nullptr;
  GLint pixel_format_count = 0;
  CGLChoosePixelFormat(attribs, &pixel_format, &pixel_format_count);

  // -

  CGLContextObj context = nullptr;
  CGLCreateContext(pixel_format, nullptr, &context);
  assert(context);
  CGLDestroyPixelFormat(pixel_format);
  CGLSetCurrentContext(context);

  printf("GL_VERSION: %s\n", glGetString(GL_VERSION));
  printf("GL_RENDERER: %s\n", glGetString(GL_RENDERER));
}

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

  glEnable(GL_PROGRAM_POINT_SIZE);
  glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

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
  glClearColor(10000, 10000, 9999, 5000);
  glClear(GL_COLOR_BUFFER_BIT);
  {
    float p[4] = {};
    glReadPixels(0, 0, 1, 1, GL_RGBA, GL_FLOAT, p);
    printf("%f %f %f %f\n", p[0], p[1], p[2], p[3]);
    assert(p[0] == 10000);
    assert(p[1] == 10000);
    //assert(p[2] == 10000);
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
out vec4 webgl_FragColor;
void main() {
  (webgl_FragColor = vec4(0.0, 0.0, 0.0, 0.0));
  vec4 color = texture(u_sampler, vec2(0));
  (webgl_FragColor = color / vec4(20000));
}
)";
  fs_tex_src = R"(#version 410
out vec4 webgl_FragColor;
uniform sampler2D webgl_cb6b29086417f5a2;
uniform vec4 webgl_8a47bd46608ce4b3;
in vec2 webgl_a3c804d63a6999e3;
void main(){
(webgl_FragColor = vec4(0.0, 0.0, 0.0, 0.0));
vec4 webgl_54d478077cd5722c = texture(webgl_cb6b29086417f5a2, vec2(0.0, 0.0));
(webgl_FragColor = (webgl_54d478077cd5722c / 20000.0));
}
)";
  const auto prog = make_program(vs_src, fs_tex_src);
  glUseProgram(prog);
  assert(!glGetError());

  glBindFramebuffer(GL_FRAMEBUFFER, backbuffer_fb);
  glDepthMask(GL_TRUE);
  glStencilMaskSeparate(GL_FRONT, UINT32_MAX);
  glClearColor(0,0,0,0);
  glClearDepth(1);
  glClearStencil(0);
  assert(!glGetError());
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  glDepthMask(GL_TRUE);
  glStencilMaskSeparate(GL_FRONT, UINT32_MAX);
  glClearColor(10000, 10000, 10000, 10000);
  glClearDepth(1);
  glClearStencil(0);
  glBindFramebuffer(GL_FRAMEBUFFER, backbuffer_fb);
  glDrawArrays(GL_POINTS, 0, 1);
  assert(!glGetError());

  {
    uint32_t p = 0;
    glBindFramebuffer(GL_FRAMEBUFFER, backbuffer_fb);
    glReadPixels(0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &p);
    printf("%08x\n", p);
    //assert(p == 0x80604020);
    //assert(p == 0x00808080);
    assert((p & 0xff000000) == 0x00000000);
  }

  // Ignore teardown.
  assert(!glGetError());
  printf("Done.\n");
  return 0;
}
