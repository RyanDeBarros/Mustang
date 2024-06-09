#pragma once

#include <GL/glew.h>
#include <toml/tomlcpp.hpp>

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

struct Renderable;

namespace Render
{
	extern BatchModel NullModel;
	extern inline BufferCounter VertexBufferLayoutCount(const Renderable&);
	extern inline BufferCounter VertexBufferLayoutCount(const BufferCounter&, const VertexLayout&, const VertexLayoutMask&);
	extern inline Stride StrideCountOf(const VertexLayout&, const VertexLayoutMask&);
	extern inline void _AttribLayout(const VertexLayout&, const VertexLayoutMask&);
}

struct Renderable
{
	BatchModel model;
	TextureHandle textureHandle;

	Renderable(BatchModel model = BatchModel(), TextureHandle texture_handle = 0);
	Renderable(Renderable&& other) noexcept;
	Renderable(const Renderable& other) noexcept;
	~Renderable();


private:
	GLfloat* vertexBufferData;
	BufferCounter vertexCount;
	GLuint* indexBufferData;
	BufferCounter indexCount;
	
	bool AttachVertexBuffer(toml::Array vertex_array, size_t size);
	bool AttachIndexBuffer(toml::Array index_array, size_t size);

	friend class CanvasLayer;
	friend class ActorPrimitive2D;
	friend enum class LOAD_STATUS loadRenderable(const char* filepath, struct Renderable& renderable);
	friend BufferCounter Render::VertexBufferLayoutCount(const Renderable& render);
};
