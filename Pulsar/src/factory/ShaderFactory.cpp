#include "ShaderFactory.h"

#include "Logger.h"
#include "Macros.h"

struct ShaderElement
{
	Shader* shader;
	char* vertexFilepath;
	char* fragmentFilepath;
	ShaderElement()
		: shader(nullptr), vertexFilepath(nullptr), fragmentFilepath(nullptr)
	{}
	ShaderElement(ShaderElement&& move) noexcept
		: shader(move.shader), vertexFilepath(move.vertexFilepath), fragmentFilepath(move.fragmentFilepath)
	{
		move.shader = nullptr;
		move.vertexFilepath = nullptr;
		move.fragmentFilepath = nullptr;
	}
	ShaderElement(const char* vertex_filepath, const char* fragment_filepath)
		: shader(new Shader(vertex_filepath, fragment_filepath))
	{
		size_t length = strlen(vertex_filepath);
		vertexFilepath = new char[length + 1];
		strcpy_s(vertexFilepath, length + 1, vertex_filepath);
		vertexFilepath[length] = '\0';
		length = strlen(fragment_filepath);
		fragmentFilepath = new char[length + 1];
		strcpy_s(fragmentFilepath, length + 1, fragment_filepath);
		fragmentFilepath[length] = '\0';
	}
	~ShaderElement()
	{
		if (shader)
			delete shader;
		if (vertexFilepath)
			delete[] vertexFilepath;
		if (fragmentFilepath)
			delete[] fragmentFilepath;
	}
};

static void null_factory()
{
	Logger::LogErrorFatal("ShaderFactory is not initialized. Call ShaderFactory::Init() before application loop.");
}

ShaderHandle ShaderFactory::handle_cap;
std::unordered_map<ShaderHandle, struct ShaderElement>* ShaderFactory::factory;

Shader* ShaderFactory::Get(ShaderHandle handle)
{
	if (!factory)
	{
		null_factory();
		return nullptr;
	}
	auto iter = factory->find(handle);
	if (iter != factory->end())
		return iter->second.shader;
	else
	{
		Logger::LogWarning("Shader handle (" + std::to_string(handle) + ") does not exist in ShaderFactory.");
		return nullptr;
	}
}

void ShaderFactory::Init()
{
	handle_cap = 1;
	factory = new std::unordered_map<ShaderHandle, ShaderElement>();
}

void ShaderFactory::Terminate()
{
	if (!factory)
	{
		null_factory();
		return;
	}
	factory->clear();
	delete factory;
}

ShaderHandle ShaderFactory::GetHandle(const char* vertex_shader, const char* fragment_shader)
{
	if (!factory)
	{
		null_factory();
		return 0;
	}
	for (const auto& [handle, element] : *factory)
	{
		if (strcmp(vertex_shader, element.vertexFilepath) == 0 && strcmp(fragment_shader, element.fragmentFilepath) == 0)
			return handle;
	}
	ShaderElement element(vertex_shader, fragment_shader);
	if (element.shader->IsValid())
	{
		ShaderHandle handle = handle_cap++;
		factory->emplace(handle, ShaderElement(std::move(element)));
		return handle;
	}
	else
		return 0;
}

void ShaderFactory::Bind(ShaderHandle handle)
{
	Shader* shader = Get(handle);
	if (shader)
		shader->Bind();
	else
		Logger::LogWarning("Failed to bind shader at handle (" + std::to_string(handle) + ").");
}

void ShaderFactory::Unbind()
{
	if (!factory)
	{
		null_factory();
		return;
	}
	TRY(glUseProgram(0));
}

void bad_uniform(ShaderHandle handle)
{
	Logger::LogWarning("Failed to set uniform for shader at handle (" + std::to_string(handle) + ").");
}

void ShaderFactory::SetUniform1f(ShaderHandle handle, const char* uniform_name, GLfloat v0)
{
	Shader* shader = Get(handle);
	if (shader)
	{
		TRY(glUniform1f(shader->GetUniformLocation(uniform_name), v0));
	}
	else
		bad_uniform(handle);
}

void ShaderFactory::SetUniform1iv(ShaderHandle handle, const char* uniform_name, const GLint* value, GLsizei array_count)
{
	Shader* shader = Get(handle);
	if (shader)
	{
		TRY(glUniform1iv(shader->GetUniformLocation(uniform_name), array_count, value));
	}
	else
		bad_uniform(handle);
}

void ShaderFactory::SetUniformMatrix3fv(ShaderHandle handle, const char* uniform_name, const GLfloat* value, GLsizei array_count)
{
	Shader* shader = Get(handle);
	if (shader)
	{
		TRY(glUniformMatrix3fv(shader->GetUniformLocation(uniform_name), array_count, GL_FALSE, value));
	}
	else
		bad_uniform(handle);
}
