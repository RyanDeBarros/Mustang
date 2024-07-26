#pragma once

#include <unordered_set>

#include <glm/glm.hpp>

#include "transform/Transformable.h"
#include "factory/Shader.h"

class LayerView2D : public Transformable2D
{
	glm::mat3 m_ProjectionMatrix;
	glm::mat3 m_VP;
	mutable std::unordered_set<ShaderHandle> shaderCache;

public:
	LayerView2D(float pLeft, float pRight, float pBottom, float pTop);

	inline virtual void OperateTransform(const std::function<void(Transform2D& position)>& op) override { op(*m_Transform); UpdateVP(); }
	inline virtual void OperatePosition(const std::function<void(glm::vec2& position)>& op) override { op(m_Transform->position); UpdateVP(); }
	inline virtual void OperateRotation(const std::function<void(glm::float32& rotation)>& op) override { op(m_Transform->rotation); UpdateVP(); }
	inline virtual void OperateScale(const std::function<void(glm::vec2& scale)>& op) override { op(m_Transform->scale); UpdateVP(); }

private:
	friend class CanvasLayer;
	void PassVPUniform(const ShaderHandle&) const;
	void UpdateVP();
};
