#pragma once
#include "GLerrorHandler.h"

#include "vertexArray.h"
#include "indexBuffer.h"
#include "shader.h"

class Renderer
{
public:
    void Clear() const;
    void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
};