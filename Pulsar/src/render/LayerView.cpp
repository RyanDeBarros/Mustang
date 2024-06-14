#include "LayerView.h"

#include <glm/gtc/matrix_transform.hpp>

#include "factory/ShaderFactory.h"

LayerView2D::LayerView2D(float pLeft, float pRight, float pBottom, float pTop)
	: m_ViewTransform(), m_ProjectionMatrix(glm::ortho<float>(pLeft, pRight, pBottom, pTop)), m_VP()
{
	UpdateVP();
}

void LayerView2D::PassVPUniform(const ShaderHandle& handle) const
{
	if (shaderCache.find(handle) == shaderCache.end())
	{
		ShaderFactory::SetUniformMatrix3fv(handle, "u_VP", &m_VP[0][0]);
		shaderCache.insert(handle);
	}
}

void LayerView2D::ViewSetTransform(const Transform2D& view_transform)
{
	m_ViewTransform = view_transform;
	UpdateVP();
}

void LayerView2D::ViewSetPosition(const float& x, const float& y)
{
	m_ViewTransform.position = glm::vec2(x, y);
	UpdateVP();
}

void LayerView2D::ViewSetRotation(const float& r)
{
	m_ViewTransform.rotation = r;
	UpdateVP();
}

void LayerView2D::ViewSetScale(const float& sx, const float& sy)
{
	m_ViewTransform.scale = glm::vec2(sx, sy);
	UpdateVP();
}

void LayerView2D::UpdateVP()
{
	m_VP = m_ProjectionMatrix * Transform::ToInverseMatrix(m_ViewTransform);
	shaderCache.clear();
}
