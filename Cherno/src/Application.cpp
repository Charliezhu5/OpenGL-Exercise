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

#include "imGUI/imgui.h"
#include "imGUI/imgui_impl_glfw.h"
#include "imGUI/imgui_impl_opengl3.h"

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    float window_width = 1600.0f;
    float window_height = 900.0f;
    window = glfwCreateWindow(window_width, window_height, "Hello World", NULL, NULL);
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
             -320.0f,  -564.0f,  0.0f,  0.0f,     // vertex 0 with texture coordinates now.
             320.0f,  -564.0f,  1.0f,  0.0f,     // 1
             320.0f,  564.0f,  1.0f,  1.0f,     // 2
             -320.0f, 564.0f,  0.0f,  1.0f      // 3
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

        /*create projection view model matrices.*/
        glm::mat4 proj = glm::ortho(0.0f, window_width, 0.0f, window_height, -1.0f, 1.0f); /*it's like 4 bundaries of window?*/
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

        Shader shader("res/shader/basic.shader");
        shader.Bind();

        Texture texture("res/texture/texture.png");
        texture.Bind();
        shader.SetUniform1i("u_Texture", 0); /*this integer needs to match texture.Bind(int), to find texture to correct slot.*/

        va.Unbind();
        vb.Unbind();
        ib.Unbind();
        shader.Unbind();

        Renderer renderer;

        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui::StyleColorsDark();
        
        glm::vec3 translationA(200, 440, 0);
        glm::vec3 translationB(500, 440, 0);

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            renderer.Clear();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            /*draw 1st joker*/
            {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), translationA);
                glm::mat4 mvp = proj * view * model;
                shader.Bind();
                shader.SetUniformMat4f("u_MVP", mvp);
                renderer.Draw(va, ib, shader);
            }

            /*draw 2nd joker*/
            {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), translationB);
                glm::mat4 mvp = proj * view * model;
                shader.Bind();
                shader.SetUniformMat4f("u_MVP", mvp);
                renderer.Draw(va, ib, shader);
            }

            {
                ImGui::SliderFloat3("TranslationA", &translationA.x, 0.0f, window_width); // Edit 1 float using a slider from 0.0f to 1.0f
                ImGui::SliderFloat3("TranslationB", &translationB.x, 0.0f, window_width);
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            }

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}