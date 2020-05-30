/*master branch*/
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"
#include "vertexBuffer.h"
#include "indexBuffer.h"
#include "vertexArray.h"
/* Parse shader file into string */

struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);

    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
            {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos)
            {
                type = ShaderType::FRAGMENT;
            }
        }
        else 
        {
            ss[int(type)] << line << "\n";
        }
    }
    return { ss[0].str(), ss[1].str() };
}

/* A shader function. */
static int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char)); /* this is very interesting. */
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Compilation of " << (type == GL_VERTEX_SHADER ? "vertex shader" : "fragment shader")
            << " failed!!!" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1); /* basically a vsync.*/

    if (glewInit() != GLEW_OK)
        std::cout << "glewInit error!!\n";

    std::cout << glGetString(GL_VERSION) << std::endl;
    { /*this scope is used to prevent opengl error check infinite loop.*/
        float position[] = {
            -0.5f, -0.5f, // vertex 0
             0.5f, -0.5f, // 1
             0.5f,  0.5f, // 2
            -0.5f,  0.5f, // 3
        };

        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        //unsigned int vao;
        //GLCall(glGenVertexArrays(1, &vao));
        //GLCall(glBindVertexArray(vao));

        VertexArray va;
        VertexBuffer vb(position, 4 * 2 * sizeof(float));
        VertexBufferLayout layout;
        layout.Push<float>(2);
        va.AddBuffer(vb, layout);

        IndexBuffer ib(indices, 6); /*Encapsulation day 1.*/

        ShaderProgramSource source = ParseShader("res/shader/basic.shader");
        unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
        GLCall(glUseProgram(shader)); /* bind our shader program. */

        GLCall(int location = glGetUniformLocation(shader, "u_Color"));
        ASSERT(location != -1); /*make sure uniform is located. But sometimes unused shader will be deleted by gl so you cant locate them, that's why you dont need to assert every uniform location.*/
        GLCall(glUniform4f(location, 0.2f, 0.3f, 0.8f, 1.0f));

        GLCall(glUseProgram(0));
        GLCall(glBindVertexArray(0));
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

        float r = 0.0f;
        float g = 0.0f;
        float b = 1.0f;
        float increment = 0.01f;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            glClear(GL_COLOR_BUFFER_BIT);

            GLCall(glUseProgram(shader));
            GLCall(glUniform4f(location, r, g, b, 1.0f));

            va.Bind();
            ib.Bind();

            GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr)); /* GL can only accept unsigned integer. You can put glDraw inside IndexBuffer, but it will be tricky to deal with complex models with different materials.*/

            if (r > 1.0f)
                increment = -0.03f;
            if (r < 0.0f)
                increment = 0.03f;

            r += increment;
            b -= increment;
            g = r;

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }

        glDeleteProgram(shader); /* delete shader program before exit, house cleaning. */
    }
    glfwTerminate();
    return 0;
}