#pragma once

#include <unordered_set>

#include <glm/glm.hpp>

#include "Handles.inl"
#include "transform/Transform.h"
#include "factory/Shader.h"

class LayerView2D
{
	glm::mat3 m_ProjectionMatrix;
	glm::mat3 m_VP;
	mutable std::unordered_set<ShaderHandle> shaderCache;

public:
	LayerView2D(float pLeft, float pRight, float pBottom, float pTop);
	LayerView2D(const LayerView2D&) = delete;
	LayerView2D(LayerView2D&&) = delete;

	Transform2D m_Transform;
	inline void NotifyTransform() { UpdateVP(); }

private:
	friend class CanvasLayer;
	void PassVPUniform(ShaderHandle) const;
	void UpdateVP();
};
