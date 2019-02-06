#include "init.h"

#include <OpenGL/OpenGL.h>
#include <OpenGL/gl3.h>
#include <cassert>
#include <cstdio>

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
