#pragma once

#include <unordered_set>

#include <glm/glm.hpp>

#include "Transform.h"
#include "factory/Shader.h"

class LayerView2D
{
	Transform2D m_ViewTransform;
	glm::mat3 m_ProjectionMatrix;
	glm::mat3 m_VP;
	mutable std::unordered_set<ShaderHandle> m_AppliedShaders;

public:
	LayerView2D(float pLeft, float pRight, float pBottom, float pTop);
	
	void ViewSetTransform(const Transform2D&);
	void ViewSetPosition(const float& x, const float& y);
	void ViewSetRotation(const float& r);
	void ViewSetScale(const float& sx, const float& sy);

private:
	friend class CanvasLayer;
	void PassVPUniform(const ShaderHandle&) const;
	void UpdateVP();
};
