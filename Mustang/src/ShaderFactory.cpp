#include "ShaderFactory.h"

#include "Utility.h"
#include "Shader.h"
#include "Logger.h"

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

void ShaderFactory::Init()
{
	handle_cap = 1;
	factory = new std::unordered_map<ShaderHandle, ShaderElement>();
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
	ShaderHandle handle = handle_cap++;
	factory->emplace(handle, ShaderElement(vertex_shader, fragment_shader));
	return handle;
}

void ShaderFactory::Bind(ShaderHandle handle)
{
	if (!factory)
	{
		null_factory();
		return;
	}
	auto iter = factory->find(handle);
	if (iter != factory->end())
		iter->second.shader->Bind();
	else
		Logger::LogWarning("Failed to bind shader. Handle (" + std::to_string(handle) + ") does not exist in ShaderFactory.");
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
