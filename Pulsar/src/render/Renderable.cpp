#include "Renderable.h"

#include "Macros.h"
#include "registry/UniformLexiconRegistry.h"

BatchModel::BatchModel(VertexLayout layout, VertexLayoutMask layoutMask, ShaderHandle shader)
	: layout(layout), layoutMask(layoutMask), shader(shader)
{
}

bool BatchModel::operator==(const BatchModel& m) const
{
	return layout == m.layout && layoutMask == m.layoutMask && shader == m.shader;
}


size_t std::hash<BatchModel>::operator()(const BatchModel& model) const
{
	size_t h1 = std::hash<ShaderHandle>{}(model.shader);
	size_t h2 = std::hash<ShaderHandle>{}(model.layout);
	size_t h3 = std::hash<ShaderHandle>{}(model.layoutMask);
	return h1 ^ (h2 << 1) ^ (h3 << 2);
}

namespace Render
{
	VertexBufferCounter VertexBufferLayoutCount(const Renderable& render)
	{
		return render.vertexCount * StrideCountOf(render.model.layout, render.model.layoutMask);
	}

	VertexBufferCounter VertexBufferLayoutCount(const VertexBufferCounter& num_vertices, const VertexLayout& layout, const VertexLayoutMask& mask)
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
			PULSAR_TRY(glEnableVertexAttribArray(num_attribs));
			auto shift = 2 * num_attribs;
			unsigned char attrib = ((layout & (3 << shift)) >> shift) + 1;
			PULSAR_TRY(glVertexAttribPointer(num_attribs, attrib, GL_FLOAT, GL_FALSE, Render::StrideCountOf(layout, mask) * sizeof(GLfloat), (const GLvoid*)offset));
			offset += attrib * sizeof(GLfloat);
			num_attribs++;
		}
	}
}

Renderable::Renderable(BatchModel model, TextureHandle texture_handle, UniformLexiconHandle uniform_lexicon)
	: model(model), textureHandle(texture_handle), uniformLexicon(uniform_lexicon), vertexBufferData(nullptr), vertexCount(0), indexBufferData(nullptr), indexCount(0)
{
}

Renderable::Renderable(Renderable&& other) noexcept
	: model(other.model), textureHandle(other.textureHandle), uniformLexicon(other.uniformLexicon),
	vertexBufferData(other.vertexBufferData), vertexCount(other.vertexCount), indexBufferData(other.indexBufferData), indexCount(other.indexCount)
{
	other.vertexBufferData = nullptr;
	other.indexBufferData = nullptr;
}

Renderable::Renderable(const Renderable& other)
	: model(other.model), textureHandle(other.textureHandle), uniformLexicon(other.uniformLexicon),
	vertexBufferData(nullptr), vertexCount(other.vertexCount), indexBufferData(nullptr), indexCount(other.indexCount)
{
	if (other.vertexBufferData)
	{
		auto buffer_size = Render::VertexBufferLayoutCount(other);
		vertexBufferData = new GLfloat[buffer_size];
		memcpy_s(vertexBufferData, buffer_size * sizeof(GLfloat), other.vertexBufferData, buffer_size * sizeof(GLfloat));
	}
	if (other.indexBufferData)
	{
		indexBufferData = new GLuint[indexCount];
		memcpy_s(indexBufferData, indexCount * sizeof(GLuint), other.indexBufferData, indexCount * sizeof(GLuint));
	}
}

Renderable& Renderable::operator=(const Renderable& other)
{
	if (this == &other)
		return *this;
	model = other.model;
	textureHandle = other.textureHandle;
	uniformLexicon = other.uniformLexicon;
	
	if (vertexBufferData)
	{
		delete[] vertexBufferData;
		vertexBufferData = nullptr;
	}
	vertexCount = other.vertexCount;
	if (other.vertexBufferData)
	{
		auto buffer_size = Render::VertexBufferLayoutCount(other);
		vertexBufferData = new GLfloat[buffer_size];
		memcpy_s(vertexBufferData, buffer_size * sizeof(GLfloat), other.vertexBufferData, buffer_size * sizeof(GLfloat));
	}

	if (indexBufferData)
	{
		delete[] indexBufferData;
		indexBufferData = nullptr;
	}
	indexCount = other.indexCount;
	if (other.indexBufferData)
	{
		indexBufferData = new GLuint[indexCount];
		memcpy_s(indexBufferData, indexCount * sizeof(GLuint), other.indexBufferData, indexCount * sizeof(GLuint));
	}
	return *this;
}

Renderable& Renderable::operator=(Renderable&& other) noexcept
{
	if (this == &other)
		return *this;
	model = other.model;
	textureHandle = other.textureHandle;
	uniformLexicon = other.uniformLexicon;
	if (vertexBufferData)
		delete[] vertexBufferData;
	vertexBufferData = other.vertexBufferData;
	vertexCount = other.vertexCount;
	if (indexBufferData)
		delete[] indexBufferData;
	indexBufferData = other.indexBufferData;
	indexCount = other.indexCount;

	other.vertexBufferData = nullptr;
	other.indexBufferData = nullptr;
	return *this;
}

Renderable::~Renderable()
{
	if (vertexBufferData)
	{
		delete[] vertexBufferData;
		vertexBufferData = nullptr;
	}
	if (indexBufferData)
	{
		delete[] indexBufferData;
		indexBufferData = nullptr;
	}
}

bool Renderable::AttachVertexBuffer(toml::v3::array* vertex_array, size_t size)
{
	if (size > USHRT_MAX)
		return false;
	if (vertexBufferData)
		delete[] vertexBufferData;
	if (size == 0)
		return true;
	vertexBufferData = new GLfloat[size];
	for (int i = 0; i < size; i++)
	{
		auto _double = vertex_array->get_as<double>(i);
		if (!_double)
		{
			if (vertexBufferData)
			{
				delete[] vertexBufferData;
				vertexBufferData = nullptr;
			}
			return false;
		}
		vertexBufferData[i] = static_cast<GLfloat>(_double->get());
	}
	return true;
}

bool Renderable::AttachIndexBuffer(toml::v3::array* index_array, size_t size)
{
	if (size > USHRT_MAX)
		return false;
	if (indexBufferData)
		delete[] indexBufferData;
	if (size == 0)
		return true;
	indexBufferData = new GLuint[size];
	for (int i = 0; i < size; i++)
	{
		auto _int = index_array->get_as<int64_t>(i);
		if (!_int)
		{
			if (indexBufferData)
			{
				delete[] indexBufferData;
				indexBufferData = nullptr;
			}
			return false;
		}
		indexBufferData[i] = static_cast<GLuint>(_int->get());
	}
	return true;
}
