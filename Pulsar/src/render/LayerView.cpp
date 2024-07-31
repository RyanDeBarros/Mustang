#include "LayerView.h"

#include <glm/gtc/matrix_transform.hpp>

#include "factory/ShaderFactory.h"

LayerView2D::LayerView2D(float pLeft, float pRight, float pBottom, float pTop)
	: m_ProjectionMatrix(glm::ortho<float>(pLeft, pRight, pBottom, pTop)), m_VP(), m_Transform(std::make_shared<LayerViewTransformable2D>())
{
	m_Transform->SetLayerView(this);
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
	m_VP = m_ProjectionMatrix * Transform::ToInverseMatrix(m_Transform->GetTransform());
	shaderCache.clear();
}
