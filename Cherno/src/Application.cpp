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

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

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

        /*create projection matrix.*/
        glm::mat4 proj = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);


        Shader shader("res/shader/basic.shader");
        shader.Bind();
        shader.SetUniformMat4f("u_MVP", proj);

        Texture texture("res/texture/texture.png");
        texture.Bind();
        shader.SetUniform1i("u_Texture", 0); /*this integer needs to match texture.Bind(int), to find texture to correct slot.*/

        va.Unbind();
        vb.Unbind();
        ib.Unbind();
        shader.Unbind();

        Renderer renderer;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            renderer.Clear();
            
            shader.Bind();
            
            renderer.Draw(va, ib, shader);

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
    }
    glfwTerminate();
    return 0;
}