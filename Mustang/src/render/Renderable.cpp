#include "render/Renderable.h"

#include "Utility.h"

BatchModel::BatchModel(ShaderHandle shader, VertexLayout layout, VertexLayoutMask layoutMask)
	: shader(shader), layout(layout), layoutMask(layoutMask)
{
}

bool BatchModel::operator==(const BatchModel& m) const
{
	return shader == m.shader && layout == m.layout && layoutMask == m.layoutMask;
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
	Renderable Empty = { BatchModel(), nullptr, 0, nullptr, 0, 0 };
	BatchModel NullModel = { 0, 0, 0 };

	PointerOffset VertexBufferLayoutCount(const Renderable& render)
	{
		return render.vertexCount * StrideCountOf(render.model.layout, render.model.layoutMask);
	}

	PointerOffset VertexBufferLayoutCount(const BufferCounter& num_vertices, const VertexLayout& layout, const VertexLayoutMask& mask)
	{
		return num_vertices * StrideCountOf(layout, mask);
	}

	unsigned short StrideCountOf(const VertexLayout& layout, const VertexLayoutMask& mask)
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
