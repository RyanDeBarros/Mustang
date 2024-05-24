#pragma once

#include <GL/glew.h>

#include "Typedefs.h"

struct BatchModel
{
	ShaderHandle shader;
	// TODO MaterialHandle material;
	VertexLayout layout;
	VertexLayoutMask layoutMask;

	BatchModel()
		: shader(0), layout(0), layoutMask(0)
	{}

	bool operator==(const BatchModel& m)
	{
		// TODO material check
		return shader == m.shader && layout == m.layout && layoutMask == m.layoutMask;
	}
};

struct Renderable
{
	BatchModel m_Model;
	GLfloat* m_VertexBufferData;
	VertexCounter m_VertexCount;
	GLuint* m_IndexBufferData;
	VertexCounter m_IndexCount;
	// TODO m_TextureHandle
};

namespace Render
{
	extern Renderable Empty;
}
