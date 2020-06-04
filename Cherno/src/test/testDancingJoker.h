#pragma once

#include "test.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "GLerrorHandler.h"
#include "Renderer.h"
#include "vertexBuffer.h"
#include "indexBuffer.h"
#include "vertexArray.h"
#include "shader.h"
#include "texture.h"
#include "screen.h"

#include <memory>

namespace test {

	class TestDancingJoker : public Test
	{
	public:
		TestDancingJoker();
		~TestDancingJoker();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;

	private:
		/*some smart pointers.*/
		std::unique_ptr<VertexArray> m_VAO;
		std::unique_ptr<VertexBuffer> m_VertexBuffer;
		std::unique_ptr<IndexBuffer> m_IBO;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Texture> m_Texture;

		glm::mat4 m_Proj, m_View;
		glm::vec3 m_Translation;

		bool m_Increasing;
		int m_Inc;
	};
}