#pragma once
#include <GL/glew.h>

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

/* Error handling in OpenGL, some functions. The working pipeline is clear the error by looping first, then retrieve error after each GL function.*/
void GLClearError();

bool GLLogCall(const char* function, const char* file, int line);
