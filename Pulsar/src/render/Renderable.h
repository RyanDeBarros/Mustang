#pragma once

#include <GL/glew.h>
#include <toml/toml.hpp>

#include "registry/ShaderRegistry.h"

typedef unsigned short VertexLayoutMask;
typedef unsigned int VertexLayout;
typedef unsigned int BufferCounter;
typedef unsigned short Stride;

struct BatchModel
{
	VertexLayout layout;
	VertexLayoutMask layoutMask;
	ShaderHandle shader;
	UniformLexiconHandle uniformLexicon;

	BatchModel(VertexLayout layout = 0, VertexLayoutMask layoutMask = 0, ShaderHandle shader = ShaderRegistry::standard_shader, UniformLexiconHandle uniformLexicon = 0);
	bool operator==(const BatchModel&) const;
};

template<>
struct std::hash<BatchModel>
{
	size_t operator()(const BatchModel& model) const;
};

struct Renderable;

namespace Render
{
	extern BufferCounter VertexBufferLayoutCount(const Renderable&);
	extern BufferCounter VertexBufferLayoutCount(const BufferCounter&, const VertexLayout&, const VertexLayoutMask&);
	extern Stride StrideCountOf(const VertexLayout&, const VertexLayoutMask&);
	extern void _AttribLayout(const VertexLayout&, const VertexLayoutMask&);
}

struct Renderable
{
	BatchModel model;
	TextureHandle textureHandle;

	Renderable(BatchModel model = BatchModel(), TextureHandle texture_handle = 0);
	Renderable(Renderable&& other) noexcept;
	Renderable(const Renderable& other);
	Renderable& operator=(const Renderable& other);
	Renderable& operator=(Renderable&& other) noexcept;
	~Renderable();

private:
	GLfloat* vertexBufferData;
	GLuint* indexBufferData;
	BufferCounter vertexCount;
	BufferCounter indexCount;
	
	bool AttachVertexBuffer(toml::v3::array* vertex_array, size_t size);
	bool AttachIndexBuffer(toml::v3::array* index_array, size_t size);

	friend class CanvasLayer;
	friend class ActorPrimitive2D;
	friend class RectRender;
	friend class DebugPolygon;
	friend class DebugPoint;
	friend class DebugPoint;
	friend struct Loader;
	friend BufferCounter Render::VertexBufferLayoutCount(const Renderable& render);
};
