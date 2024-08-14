#pragma once

#include "Transformable.h"

class Transformer2D : public Transformable2D
{
	std::shared_ptr<Transformable2D> m_Transform;
	std::shared_ptr<PackedTransform2D> m_Packed;

public:
	Transformer2D(const Transform2D& parent = {});
	Transformer2D(const std::shared_ptr<Transformable2D>& external_parent);
	Transformer2D(std::shared_ptr<Transformable2D>&& external_parent);

	Transformer2D(const Transformer2D&);
	Transformer2D(Transformer2D&&) noexcept;
	Transformer2D& operator=(const Transformer2D&);
	Transformer2D& operator=(Transformer2D&&) noexcept;

	inline std::shared_ptr<Transformable2D> Transform() { return m_Transform; }
	inline std::weak_ptr<Transformable2D> TransformWeak() { return m_Transform; }
	inline std::weak_ptr<PackedTransform2D> Packed() const { return m_Packed; }

	inline Transform2D GetTransform() const override { return m_Transform->GetTransform(); };
	inline glm::vec2 GetPosition() const override { return m_Transform->GetPosition(); }
	inline glm::float32 GetRotation() const override { return m_Transform->GetRotation(); }
	inline glm::vec2 GetScale() const override { return m_Transform->GetScale(); }

	void OperateTransform(const std::function<void(Transform2D& position)>& op) override;
	void OperatePosition(const std::function<void(glm::vec2& position)>& op) override;
	void OperateRotation(const std::function<void(glm::float32& rotation)>& op) override;
	void OperateScale(const std::function<void(glm::vec2& scale)>& op) override;

	inline void LinkPackedParent(const std::weak_ptr<PackedTransform2D>& tr) override { m_Transform->LinkPackedParent(tr); }
	inline PackedTransform2D* GetPackedParent() override { return m_Transform->GetPackedParent(); }
};