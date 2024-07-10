#pragma once

#include <GL/glew.h>
#include <toml/toml.hpp>

#include "Typedefs.h"
#include "factory/ShaderFactory.h"

struct BatchModel
{
	VertexLayout layout;
	VertexLayoutMask layoutMask;
	ShaderHandle shader;
	UniformLexiconHandle uniformLexicon;

	BatchModel(VertexLayout layout = 0, VertexLayoutMask layoutMask = 0, ShaderHandle shader = ShaderFactory::standard_shader, UniformLexiconHandle uniformLexicon = 0);
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
	void operator=(const Renderable& other) noexcept;
	~Renderable();

private:
	GLfloat* vertexBufferData;
	BufferCounter vertexCount;
	GLuint* indexBufferData;
	BufferCounter indexCount;
	
	bool AttachVertexBuffer(toml::v3::array* vertex_array, size_t size);
	bool AttachIndexBuffer(toml::v3::array* index_array, size_t size);

	friend class CanvasLayer;
	friend class ActorPrimitive2D;
	friend class RectRender;
	friend class DebugPolygon;
	friend class DebugPoint;
	friend class DebugPoint;
	friend enum class LOAD_STATUS loadRenderable(const char* filepath, struct Renderable& renderable, const bool& new_texture, const bool& temporary_buffer);
	friend BufferCounter Render::VertexBufferLayoutCount(const Renderable& render);
};

struct MultiRenderable
{
	BatchModel model;
	
	// TODO array of all Renderable data except for model, since that's global to the MultiRenderable
};
