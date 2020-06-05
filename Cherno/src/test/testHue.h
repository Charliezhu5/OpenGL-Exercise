#pragma once

#include "test.h"

namespace test {

	class TestHue : public Test
	{
	public:
		TestHue();
		~TestHue();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;

	private:
		float m_ClearColor[4];
		float m_Inc;
		int m_phase;
	};
}