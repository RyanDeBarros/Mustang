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
	BufferCounter vertexCount;
	GLuint* indexBufferData;
	BufferCounter indexCount;
	TextureHandle textureHandle;
};

namespace Render
{
	extern Renderable Empty;
	extern BatchModel NullModel;
	extern inline PointerOffset VertexBufferLayoutCount(const Renderable&);
	extern inline PointerOffset VertexBufferLayoutCount(const BufferCounter&, const VertexLayout&, const VertexLayoutMask&);
	extern inline Stride StrideCountOf(const VertexLayout&, const VertexLayoutMask&);
	extern inline void _AttribLayout(const VertexLayout&, const VertexLayoutMask&);
}
