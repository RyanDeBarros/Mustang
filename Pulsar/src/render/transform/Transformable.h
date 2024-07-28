#pragma once

#include <memory>
#include <functional>

#include "Transform.h"

class Transformable2D
{
public:
	inline virtual Transform2D GetTransform() const { return {}; }
	inline virtual glm::vec2 GetPosition() const { return {}; }
	inline virtual glm::float32 GetRotation() const { return 0.0f; }
	inline virtual glm::vec2 GetScale() const { return { 1.0f, 1.0f }; }

	inline virtual void OperateTransform(const std::function<void(Transform2D& position)>& op) {}
	inline virtual void OperatePosition(const std::function<void(glm::vec2& position)>& op) {}
	inline virtual void OperateRotation(const std::function<void(glm::float32& rotation)>& op) {}
	inline virtual void OperateScale(const std::function<void(glm::vec2& scale)>& op) {}

	inline void SetTransform(const Transform2D& tr) { OperateTransform([&tr](Transform2D& transform) { transform = tr; }); }
	inline void SetPosition(float x, float y) { OperatePosition([x, y](glm::vec2& position) { position = { x, y }; }); }
	inline void SetPosition(const glm::vec2& pos) { OperatePosition([&pos](glm::vec2& position) { position = pos; }); }
	inline void SetRotation(float r) { OperateRotation([r](glm::float32& rotation) { rotation = r; }); }
	inline void SetScale(float sx, float sy) { OperateScale([sx, sy](glm::vec2& scale) { scale = { sx, sy }; }); }
	inline void SetScale(const glm::vec2& sc) { OperateScale([&sc](glm::vec2& scale) { scale = sc; }); }
};

class TransformableProxy2D : public Transformable2D
{
protected:
	// TODO don't use shared_ptr?
	std::shared_ptr<Transform2D> m_Transform;

public:
	TransformableProxy2D(const Transform2D& transform = {});
	TransformableProxy2D(const std::shared_ptr<Transform2D>& transform);
	TransformableProxy2D(std::shared_ptr<Transform2D>&& transform);
	TransformableProxy2D(const TransformableProxy2D&);
	TransformableProxy2D(TransformableProxy2D&&) noexcept;
	TransformableProxy2D& operator=(const TransformableProxy2D&);
	TransformableProxy2D& operator=(TransformableProxy2D&&) noexcept;

	inline Transform2D GetTransform() const override { return *m_Transform; };
	inline glm::vec2 GetPosition() const override { return m_Transform->position; }
	inline glm::float32 GetRotation() const override { return m_Transform->rotation; }
	inline glm::vec2 GetScale() const override { return m_Transform->scale; }

	inline virtual void OperateTransform(const std::function<void(Transform2D& position)>& op) override { op(*m_Transform); }
	inline virtual void OperatePosition(const std::function<void(glm::vec2& position)>& op) override { op(m_Transform->position); }
	inline virtual void OperateRotation(const std::function<void(glm::float32& rotation)>& op) override { op(m_Transform->rotation); }
	inline virtual void OperateScale(const std::function<void(glm::vec2& scale)>& op) override { op(m_Transform->scale); }

	inline virtual bool operator==(const TransformableProxy2D& other) const { return *m_Transform == *other.m_Transform; }
};
