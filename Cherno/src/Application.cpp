/*master branch*/
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "GLerrorHandler.h"
#include "Renderer.h"
#include "vertexBuffer.h"
#include "indexBuffer.h"
#include "vertexArray.h"
#include "shader.h"
#include "texture.h"

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(480, 640, "Hello World", NULL, NULL);
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
            -0.5f, -0.5f,  0.0f,  0.0f,     // vertex 0 with texture coordinates now.
             0.5f, -0.5f,  1.0f,  0.0f,     // 1
             0.5f,  0.5f,  1.0f,  1.0f,     // 2
            -0.5f,  0.5f,  0.0f,  1.0f      // 3
        };

        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        /*Enable blending*/
        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        
        VertexArray va;
        VertexBuffer vb(position, 4 * 4 * sizeof(float));
        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);
        va.AddBuffer(vb, layout);

        IndexBuffer ib(indices, 6);

        Shader shader("res/shader/basic.shader");
        shader.Bind();
        shader.SetUniform4f("u_Color", 0.0f, 0.0f, 0.0f, 1.0f);

        Texture texture("res/texture/texture.png");
        texture.Bind();
        shader.SetUniform1i("u_Texture", 0); /*this integer needs to match texture.Bind(int), to find texture to correct slot.*/

        va.Unbind();
        vb.Unbind();
        ib.Unbind();
        shader.Unbind();

        Renderer renderer;

        float r = 0.0f;
        float g = 0.0f;
        float b = 1.0f;
        float increment = 0.01f;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            renderer.Clear();
            
            shader.Bind();
            shader.SetUniform4f("u_Color", r, g, b, 1.0f);
            
            renderer.Draw(va, ib, shader);

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
    }
    glfwTerminate();
    return 0;
}