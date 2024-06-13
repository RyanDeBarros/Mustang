#include "MaterialFactory.h"

#include "Logger.h"
#include "ShaderFactory.h"

MaterialHandle MaterialFactory::handle_cap;
std::unordered_map<TextureHandle, Material*> MaterialFactory::factory;

void MaterialFactory::Init()
{
	handle_cap = 1;
}

void MaterialFactory::Terminate()
{
	for (const auto& [handle, material] : factory)
		delete material;
	factory.clear();
}

Material* MaterialFactory::Get(MaterialHandle handle)
{
	auto iter = factory.find(handle);
	if (iter != factory.end())
		return iter->second;
	else
	{
		Logger::LogWarning("Material handle (" + std::to_string(handle) + ") does not exist in MaterialFactory.");
		return nullptr;
	}
}

MaterialHandle MaterialFactory::GetHandle(const std::vector<MaterialUniform>& uniform_vector)
{
	for (const auto& [handle, material] : factory)
	{
		if (material->equivalent(uniform_vector))
			return handle;
	}
	MaterialHandle handle = handle_cap++;
	factory.emplace(handle, new Material(uniform_vector));
	return handle;
}

void MaterialFactory::Apply(const MaterialHandle& material, const ShaderHandle& shader)
{
	// TODO optimize by caching shader/material pairs.
	if (material == 0 || shader == 0)
		return;
	Material* mat = Get(material);
	for (const auto& uniform : mat->m_Uniforms)
	{
		switch (uniform.data.index())
		{
		case 0:
			ShaderFactory::SetUniform1i(shader, uniform.name.c_str(), std::get<GLint>(uniform.data));
			break;
		case 1:
			ShaderFactory::SetUniform2iv(shader, uniform.name.c_str(), &std::get<glm::ivec2>(uniform.data)[0]);
			break;
		case 2:
			ShaderFactory::SetUniform3iv(shader, uniform.name.c_str(), &std::get<glm::ivec3>(uniform.data)[0]);
			break;
		case 3:
			ShaderFactory::SetUniform4iv(shader, uniform.name.c_str(), &std::get<glm::ivec4>(uniform.data)[0]);
			break;
		case 4:
			ShaderFactory::SetUniform1ui(shader, uniform.name.c_str(), std::get<GLuint>(uniform.data));
			break;
		case 5:
			ShaderFactory::SetUniform2uiv(shader, uniform.name.c_str(), &std::get<glm::uvec2>(uniform.data)[0]);
			break;
		case 6:
			ShaderFactory::SetUniform3uiv(shader, uniform.name.c_str(), &std::get<glm::uvec3>(uniform.data)[0]);
			break;
		case 7:
			ShaderFactory::SetUniform4uiv(shader, uniform.name.c_str(), &std::get<glm::uvec4>(uniform.data)[0]);
			break;
		case 8:
			ShaderFactory::SetUniform1f(shader, uniform.name.c_str(), std::get<GLfloat>(uniform.data));
			break;
		case 9:
			ShaderFactory::SetUniform2fv(shader, uniform.name.c_str(), &std::get<glm::vec2>(uniform.data)[0]);
			break;
		case 10:
			ShaderFactory::SetUniform3fv(shader, uniform.name.c_str(), &std::get<glm::vec3>(uniform.data)[0]);
			break;
		case 11:
			ShaderFactory::SetUniform4fv(shader, uniform.name.c_str(), &std::get<glm::vec4>(uniform.data)[0]);
			break;
		case 12:
			ShaderFactory::SetUniformMatrix2fv(shader, uniform.name.c_str(), &std::get<glm::mat2>(uniform.data)[0][0]);
			break;
		case 13:
			ShaderFactory::SetUniformMatrix3fv(shader, uniform.name.c_str(), &std::get<glm::mat3>(uniform.data)[0][0]);
			break;
		case 14:
			ShaderFactory::SetUniformMatrix4fv(shader, uniform.name.c_str(), &std::get<glm::mat4>(uniform.data)[0][0]);
			break;
		}
	}
}
