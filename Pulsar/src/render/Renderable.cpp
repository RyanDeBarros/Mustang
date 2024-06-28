#include "Renderable.h"

#include "Macros.h"
#include "factory/UniformLexiconFactory.h"

BatchModel::BatchModel(VertexLayout layout, VertexLayoutMask layoutMask, ShaderHandle shader, UniformLexiconHandle uniformLexicon)
	: layout(layout), layoutMask(layoutMask), shader(shader), uniformLexicon(uniformLexicon)
{
}

bool BatchModel::operator==(const BatchModel& m) const
{
	return layout == m.layout && layoutMask == m.layoutMask && shader == m.shader && UniformLexiconFactory::Shares(uniformLexicon, m.uniformLexicon);
}


size_t std::hash<BatchModel>::operator()(const BatchModel& model) const
{
	size_t h1 = std::hash<ShaderHandle>()(model.shader);
	size_t h2 = std::hash<ShaderHandle>()(model.layout);
	size_t h3 = std::hash<ShaderHandle>()(model.layoutMask);
	return h1 ^ (h2 << 1) ^ (h3 << 2);
}

namespace Render
{
	BufferCounter VertexBufferLayoutCount(const Renderable& render)
	{
		return render.vertexCount * StrideCountOf(render.model.layout, render.model.layoutMask);
	}

	BufferCounter VertexBufferLayoutCount(const BufferCounter& num_vertices, const VertexLayout& layout, const VertexLayoutMask& mask)
	{
		return num_vertices * StrideCountOf(layout, mask);
	}

	Stride StrideCountOf(const VertexLayout& layout, const VertexLayoutMask& mask)
	{
		unsigned short stride = 0;
		unsigned char num_attribs = 0;
		while (mask >> num_attribs != 0)
		{
			auto shift = 2 * num_attribs;
			stride += ((layout & (0b11 << shift)) >> shift) + 1;
			num_attribs++;
		}
		return stride;
	}

	void _AttribLayout(const VertexLayout& layout, const VertexLayoutMask& mask)
	{
		unsigned short offset = 0;
		unsigned char num_attribs = 0;
		while (mask >> num_attribs != 0)
		{
			TRY(glEnableVertexAttribArray(num_attribs));
			auto shift = 2 * num_attribs;
			unsigned char attrib = ((layout & (3 << shift)) >> shift) + 1;
			TRY(glVertexAttribPointer(num_attribs, attrib, GL_FLOAT, GL_FALSE, Render::StrideCountOf(layout, mask) * sizeof(GLfloat), (const GLvoid*)offset));
			offset += attrib * sizeof(GLfloat);
			num_attribs++;
		}
	}
}

Renderable::Renderable(BatchModel model, TextureHandle texture_handle)
	: model(model), textureHandle(texture_handle), vertexBufferData(nullptr), vertexCount(0), indexBufferData(nullptr), indexCount(0)
{
}

Renderable::Renderable(Renderable&& other) noexcept
	: model(other.model), textureHandle(other.textureHandle), vertexBufferData(other.vertexBufferData), vertexCount(other.vertexCount), indexBufferData(other.indexBufferData), indexCount(other.indexCount)
{
	other.vertexBufferData = nullptr;
	other.indexBufferData = nullptr;
}

Renderable::Renderable(const Renderable& other) noexcept
	: model(other.model), textureHandle(other.textureHandle), vertexBufferData(nullptr), vertexCount(other.vertexCount), indexBufferData(nullptr), indexCount(other.indexCount)
{
	if (other.vertexBufferData && other.indexBufferData)
	{
		auto buffer_size = Render::VertexBufferLayoutCount(other);
		vertexBufferData = new GLfloat[buffer_size];
		indexBufferData = new GLuint[indexCount];
		memcpy_s(vertexBufferData, buffer_size * sizeof(GLfloat), other.vertexBufferData, buffer_size * sizeof(GLfloat));
		memcpy_s(indexBufferData, indexCount * sizeof(GLuint), other.indexBufferData, indexCount * sizeof(GLuint));
	}
}

void Renderable::operator=(const Renderable& other) noexcept
{
	model = other.model;
	textureHandle = other.textureHandle;
	if (vertexBufferData)
		delete[] vertexBufferData;
	vertexBufferData = nullptr;
	vertexCount = other.vertexCount;
	if (indexBufferData)
		delete[] indexBufferData;
	indexBufferData = nullptr;
	indexCount = other.indexCount;
	if (other.vertexBufferData && other.indexBufferData)
	{
		auto buffer_size = Render::VertexBufferLayoutCount(other);
		vertexBufferData = new GLfloat[buffer_size];
		indexBufferData = new GLuint[indexCount];
		memcpy_s(vertexBufferData, buffer_size * sizeof(GLfloat), other.vertexBufferData, buffer_size * sizeof(GLfloat));
		memcpy_s(indexBufferData, indexCount * sizeof(GLuint), other.indexBufferData, indexCount * sizeof(GLuint));
	}
}

Renderable::~Renderable()
{
	if (vertexBufferData)
		delete[] vertexBufferData;
	if (indexBufferData)
		delete[] indexBufferData;
}

bool Renderable::AttachVertexBuffer(toml::Array vertex_array, size_t size)
{
	if (vertexBufferData)
		delete[] vertexBufferData;
	vertexBufferData = new GLfloat[size];
	for (int i = 0; i < size; i++)
	{
		auto [ok, _double] = vertex_array.getDouble(i);
		if (!ok)
		{
			if (vertexBufferData)
				delete[] vertexBufferData;
			return false;
		}
		vertexBufferData[i] = (GLfloat)_double;
	}
	return true;
}

bool Renderable::AttachIndexBuffer(toml::Array index_array, size_t size)
{
	if (indexBufferData)
		delete[] indexBufferData;
	indexBufferData = new GLuint[size];
	for (int i = 0; i < size; i++)
	{
		auto [ok, _int] = index_array.getInt(i);
		if (!ok)
		{
			if (indexBufferData)
				delete[] indexBufferData;
			return false;
		}
		indexBufferData[i] = (GLuint)_int;
	}
	return true;
}
