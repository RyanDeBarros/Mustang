#pragma once

#include "GL/glew.h"

#include "Typedefs.h"

class VertexArray
{
	GLuint m_VA, m_VB, m_IB;
public:
	VertexArray(GLfloat vertices[], BufferCounter num_vertices, VertexLayoutMask layout_mask, VertexLayout layout, GLuint indices[], BufferCounter num_indices);
	~VertexArray();

	void Bind() const;
	void Unbind() const;
};
