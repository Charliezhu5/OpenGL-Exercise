#include "testHue.h"

#include "GLerrorHandler.h"
#include "imGUI/imgui.h"

namespace test {
	TestHue::TestHue()
		: m_phase(0), m_Inc(0), m_ClearColor {1.0f, 0.0f, 0.0f, 1.0f} /*default color*/
	{

	}

	TestHue::~TestHue()
	{

	}

	void TestHue::OnUpdate(float deltaTime)
	{
		if (m_phase % 6 == 0)
			if (m_ClearColor[1] < 1.0f)
				m_ClearColor[1] += m_Inc;
			else
				m_phase++;
		if (m_phase % 6 == 1)
			if (m_ClearColor[0] > 0.0f)
				m_ClearColor[0] -= m_Inc;
			else
				m_phase++;
		if (m_phase % 6 == 2)
			if (m_ClearColor[2] < 1.0f)
				m_ClearColor[2] += m_Inc;
			else
				m_phase++;
		if (m_phase % 6 == 3)
			if (m_ClearColor[1] > 0.0f)
				m_ClearColor[1] -= m_Inc;
			else
				m_phase++;
		if (m_phase % 6 == 4)
			if (m_ClearColor[0] < 1.0f)
				m_ClearColor[0] += m_Inc;
			else
				m_phase++;
		if (m_phase % 6 == 5)
			if (m_ClearColor[2] > 0.0f)
				m_ClearColor[2] -= m_Inc;
			else
				m_phase++;
	}

	void TestHue::OnRender()
	{
		GLCall(glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));
	}

	void TestHue::OnImGuiRender()
	{
		ImGui::SliderFloat("Speed", &m_Inc, 0.0f, 0.10f, "%.3f", 1.0F);
		ImGui::Text("m_phase = %d = phase %d", m_phase, m_phase % 6);
	}
}