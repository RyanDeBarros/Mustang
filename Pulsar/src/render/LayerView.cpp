#include "LayerView.h"

#include <glm/gtc/matrix_transform.hpp>

#include "factory/ShaderFactory.h"
#include "transform/Transforms.h"

LayerView2D::LayerView2D(float pLeft, float pRight, float pBottom, float pTop)
	: m_ProjectionMatrix(glm::ortho<float>(pLeft, pRight, pBottom, pTop)), m_VP(), m_Transform()
{
	UpdateVP();
}

void LayerView2D::PassVPUniform(ShaderHandle handle) const
{
	if (shaderCache.find(handle) == shaderCache.end())
	{
		ShaderFactory::SetUniformMatrix3fv(handle, "u_VP", &m_VP[0][0]);
		shaderCache.insert(handle);
	}
}

void LayerView2D::UpdateVP()
{
	m_VP = m_ProjectionMatrix * Transforms::ToInverseMatrix(m_Transform);
	shaderCache.clear();
}
