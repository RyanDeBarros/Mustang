#pragma once

#include <GL/glew.h>
#include <toml/toml.hpp>

#include "registry/Shader.h"

typedef unsigned short VertexLayoutMask;
typedef unsigned int VertexLayout;
typedef unsigned short VertexBufferCounter;
typedef unsigned short Stride;

struct BatchModel
{
	VertexLayout layout;
	VertexLayoutMask layoutMask;
	ShaderHandle shader;

	BatchModel(VertexLayout layout = 0, VertexLayoutMask layoutMask = 0, ShaderHandle shader = ShaderRegistry::HANDLE_CAP);
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
	extern VertexBufferCounter VertexBufferLayoutCount(const Renderable&);
	extern VertexBufferCounter VertexBufferLayoutCount(const VertexBufferCounter&, const VertexLayout&, const VertexLayoutMask&);
	extern Stride StrideCountOf(const VertexLayout&, const VertexLayoutMask&);
	extern void _AttribLayout(const VertexLayout&, const VertexLayoutMask&);
}

struct Renderable
{
	BatchModel model;
	TextureHandle textureHandle;
	UniformLexiconHandle uniformLexicon;

	Renderable(BatchModel model = BatchModel(), TextureHandle texture_handle = 0, UniformLexiconHandle uniform_lexicon = 0);
	Renderable(Renderable&& other) noexcept;
	Renderable(const Renderable& other);
	Renderable& operator=(const Renderable& other);
	Renderable& operator=(Renderable&& other) noexcept;
	~Renderable();

	VertexBufferCounter vertexCount;
	VertexBufferCounter indexCount;
	GLfloat* vertexBufferData;
	GLuint* indexBufferData;
	
	bool AttachVertexBuffer(toml::v3::array* vertex_array, size_t size);
	bool AttachIndexBuffer(toml::v3::array* index_array, size_t size);
};
