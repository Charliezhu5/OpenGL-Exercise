#include "testDancingJoker.h"

#include "GLerrorHandler.h"
#include "imGUI/imgui.h"

namespace test {
	TestDancingJoker::TestDancingJoker() : m_Translation(200, 440, 0), 
        m_Proj(glm::ortho(0.0f, window_width, 0.0f, window_height, -1.0f, 1.0f)), 
        m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)))
    {  

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
        
        m_VAO = std::make_unique<VertexArray>();

        m_VertexBuffer = std::make_unique<VertexBuffer>(position, 4 * 4 * sizeof(float));
        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);
        m_VAO->AddBuffer(*m_VertexBuffer, layout);

        m_IBO = std::make_unique<IndexBuffer>(indices, 6);

        m_Shader = std::make_unique<Shader>("res/shader/basic.shader");
        m_Shader->Bind();
        m_Texture = std::make_unique<Texture>("res/texture/joker.png");
        m_Shader->SetUniform1i("u_Texture", 0); /*this integer needs to match texture.Bind(int), to find texture to correct slot.*/
	
        m_Inc = 0;
        m_Increasing = true; 
    }

	TestDancingJoker::~TestDancingJoker()
	{
        /*no need to worry about deleting smart pointers.*/
	}

	void TestDancingJoker::OnUpdate(float deltaTime)
	{
        if (m_Increasing) {
            if (m_Translation.x < (window_width - 320)) {
                m_Translation.x += m_Inc;
            }
            else
            {
                m_Increasing = false;
            }
        }
        else
        {
            if (m_Translation.x > 320) {
                m_Translation.x -= m_Inc;
            }
            else {
                m_Increasing = true;
            }
        }
	}

	void TestDancingJoker::OnRender()
	{
		GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

        Renderer renderer; /*Renderer class is only consisted of two functions, no members, free call stack size 1.*/

        m_Texture->Bind();
        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), m_Translation);
            glm::mat4 mvp = m_Proj * m_View * model;
            m_Shader->Bind();
            m_Shader->SetUniformMat4f("u_MVP", mvp);
            renderer.Draw(*m_VAO, *m_IBO, *m_Shader);
        }
	}

	void TestDancingJoker::OnImGuiRender()
	{
        ImGui::SliderInt("Speed", &m_Inc, 0, 120, "%d");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
}