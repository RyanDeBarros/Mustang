#pragma once

#include <unordered_set>

#include <glm/glm.hpp>

#include "transform/Transformable.h"
#include "factory/Shader.h"

class LayerView2D
{
	glm::mat3 m_ProjectionMatrix;
	glm::mat3 m_VP;
	mutable std::unordered_set<ShaderHandle> shaderCache;
	std::shared_ptr<class LayerViewTransformable2D> m_Transform;

public:
	LayerView2D(float pLeft, float pRight, float pBottom, float pTop);
	LayerView2D(const LayerView2D&) = delete;
	LayerView2D(LayerView2D&&) = delete;

	std::shared_ptr<class LayerViewTransformable2D> Transform() { return m_Transform; }
	std::weak_ptr<class LayerViewTransformable2D> TransformWeak() { return m_Transform; }

private:
	friend class CanvasLayer;
	friend class LayerViewTransformable2D;
	void PassVPUniform(ShaderHandle) const;
	void UpdateVP();
};

class LayerViewTransformable2D : public TransformableProxy2D
{
	LayerView2D* m_LayerView = nullptr;

public:
	LayerViewTransformable2D() : TransformableProxy2D() {}

	inline void SetLayerView(LayerView2D* layer_view) { m_LayerView; }

	inline virtual void OperateTransform(const std::function<void(Transform2D& position)>& op) override { op(m_Transform); m_LayerView->UpdateVP(); }
	inline virtual void OperatePosition(const std::function<void(glm::vec2& position)>& op) override { op(m_Transform.position); m_LayerView->UpdateVP(); }
	inline virtual void OperateRotation(const std::function<void(glm::float32& rotation)>& op) override { op(m_Transform.rotation); m_LayerView->UpdateVP(); }
	inline virtual void OperateScale(const std::function<void(glm::vec2& scale)>& op) override { op(m_Transform.scale); m_LayerView->UpdateVP(); }
};
