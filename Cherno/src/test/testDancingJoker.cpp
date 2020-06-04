#include "TestDancingJoker.h"

#include "GLerrorHandler.h"
#include "imGUI/imgui.h"

namespace test {
	TestDancingJoker::TestDancingJoker()
	{
        float position[] = {
             0.0f,  0.0f,  0.0f,  0.0f,     // vertex 0 with texture coordinates now.
             240.0f,  0.0f,  1.0f,  0.0f,     // 1
             240.0f,  424.6f,  1.0f,  1.0f,     // 2
             0.0f,  424.6f,  0.0f,  1.0f      // 3
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
        glm::mat4 proj = glm::ortho(0.0f, 1600.0f, 0.0f, 900.0f, -1.0f, 1.0f); /*it's like 4 bundaries of window?*/
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

        glm::vec3 translation(0, 0, 0);
        bool increasing{ true };
        bool yincreasing{ true };
        int inc{ 0 };
	}

	TestDancingJoker::~TestDancingJoker()
	{

	}

	void TestDancingJoker::OnUpdate(float deltaTime)
	{
        if (increasing) {
            if (translation.x < 1126) {
                translation.x += inc;
            }
            else
            {
                increasing = false;
            }
        }
        else
        {
            if (translation.x > 0) {
                translation.x -= inc;
            }
            else {
                increasing = true;
            }
        }
        if (yincreasing) {
            if (translation.y < 343) {
                translation.y += inc;
            }
            else
            {
                yincreasing = false;
            }
        }
        else
        {
            if (translation.y > 0) {
                translation.y -= inc;
            }
            else {
                yincreasing = true;
            }
        }

        glm::mat4 model = glm::translate(glm::mat4(1.0f), translation);
        glm::mat4 mvp = proj * view * model;

        shader.Bind();
        shader.SetUniformMat4f("u_MVP", mvp);
	}

	void TestDancingJoker::OnRender()
	{
        GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT));

        
	}

	void TestDancingJoker::OnImGuiRender()
	{
        ImGui::SliderInt("Speed", &inc, 0, 60, "%d");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
}