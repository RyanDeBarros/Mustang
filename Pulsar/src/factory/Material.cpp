#include "Material.h"

Material::Material(const std::vector<MaterialUniform>& uniform_vector)
	: m_Uniforms(uniform_vector)
{
}

Material::~Material()
{
}
