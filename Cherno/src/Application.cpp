/*master branch*/
#include <GL/glew.h> /* glew before glfw because otherwise: error gl.h included before glew.h */
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

/* Documentation for OpenGL can be found on docs.gl */

/* fun macro here, use backslash(and nothing after that) to stop compiler adding newline character automatically. */
#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

/* Error handling in OpenGL, some functions. The working pipeline is clear the error by looping first, then retrieve error after each GL function.*/
static void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] " << error << " from function:" << function << " in " << file << " :" << line << std::endl;
        return false;
    }
    return true;
}

/* Parse shader file into string */

struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
}; /* Use struct to help with ParseShader function multiple return types. */

static ShaderProgramSource ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);

    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    }; /* this is a mode telling what are we reading. */

    std::string line;
    std::stringstream ss[2]; /* allocate 2, one for vertex one for fragment. */
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
            {
                /* set mode to vertex */
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos)
            {
                /* set mode to fragment */
                type = ShaderType::FRAGMENT;
            }
        }
        else 
        {
            ss[int(type)] << line << "\n"; /* clever! Using the index to store them into corresponding array, instead of using logic to determine which branch. */
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

    /* Error handling. */
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

    /* Link shaders to the program. */
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    /* Delete intermediate files. */
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

    std::cout << glGetString(GL_VERSION) << std::endl;  /* It's always useful to print out what version of OpenGL is running on the system. */

    /* specify the data for buffer. */
    float position[] = {
        -0.5f, -0.5f, // vertex 0
         0.5f, -0.5f, // 1
         0.5f,  0.5f, // 2
        -0.5f,  0.5f, // 3
    };

    /* create an index buffer. */
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    /*create VAO*/
    unsigned int vao;
    GLCall(glGenVertexArrays(1, &vao));
    GLCall(glBindVertexArray(vao));

    /* Create buffer outside of game loop, it's like predefined for game loop GL to call during game. */
    unsigned int buffer;
    GLCall(glGenBuffers(1, &buffer)); /* OpenGL generates a buffer(an integer type is used) and giving it an ID. */
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer)); /* Now that we created that buffer, we need to select that buffer. */
    GLCall(glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), &position, GL_STATIC_DRAW)); /* creates and initializes a buffer object's data store */

    /* Utilize index buffer. */
    unsigned int ibo; /* index buffer object. */
    GLCall(glGenBuffers(1, &ibo));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), &indices, GL_STATIC_DRAW)); /* You can change indices type to be others to save bytes. */

    /* tell OpenGL how to interpret data in buffer. */
    GLCall(glEnableVertexAttribArray(0));
    GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), 0)); /* define an array of generic vertex attribute data, it also links current buffer with VAO. */

    /* create shader */

    ShaderProgramSource source = ParseShader("res/shader/basic.shader");
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    GLCall(glUseProgram(shader)); /* bind our shader program. */

    /*add uniform*/
    GLCall(int location = glGetUniformLocation(shader, "u_Color"));
    ASSERT(location != -1); /*make sure uniform is located. But sometimes unused shader will be deleted by gl so you cant locate them, that's why you dont need to assert every uniform location.*/
    GLCall(glUniform4f(location, 0.2f, 0.3f, 0.8f, 1.0f));

    /*initialize everything by bind nothing.*/
    GLCall(glUseProgram(0));
    GLCall(glBindVertexArray(0));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

    float r = 0.0f;
    float g = 0.0f;
    float b = 1.0f;
    float increment = 0.01;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        /* Draw call on buffer pipeline. (with error handling.) */
        /*first bind shader, set up uniform*/
        GLCall(glUseProgram(shader));
        GLCall(glUniform4f(location, r, g, b, 1.0f));
        /*second bind vertex buffer, then set up layout of that vertex buffer.
        the reason to set up layout each time before drawing is our object might be changing.
        vertex array object VAO is the object containing all these states, 
        theoretically, just bind VAO for the sake of doing everything.*/
        GLCall(glBindVertexArray(vao));
        /*third bind our index buffer*/
        //GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
        /*finally draw elements*/
        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr)); /* GL can only accept unsigned integer. */

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

    glfwTerminate();
    return 0;
}