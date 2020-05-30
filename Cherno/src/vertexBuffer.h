#pragma once

class VertexBuffer
{
private:
	unsigned int m_RendererID; /*OpenGL assign ID(an integer id) to objects, like vertex buffer VAO shader, etc.*/
public:
	VertexBuffer(const void* data, unsigned int size);
	~VertexBuffer();

	void Bind() const;
	void Unbind() const;
};