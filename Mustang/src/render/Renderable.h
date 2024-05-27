#pragma once

#include <GL/glew.h>

#include "Typedefs.h"

struct BatchModel
{
	ShaderHandle shader;
	VertexLayout layout;
	VertexLayoutMask layoutMask;

	BatchModel(ShaderHandle shader = 0, VertexLayout layout = 0, VertexLayoutMask layoutMask = 0);
	bool operator==(const BatchModel&) const;
	PointerOffset layoutSize() const;
};

template<>
struct std::hash<BatchModel>
{
	size_t operator()(const BatchModel& model) const;
};

struct Renderable
{
	BatchModel model;
	GLfloat* vertexBufferData;
	VertexCounter vertexCount;
	VertexSize vertexBufferSize;
	GLuint* indexBufferData;
	VertexCounter indexCount;
	TextureHandle textureHandle;
};

namespace Render
{
	extern Renderable Empty;
}
