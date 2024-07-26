#pragma once

#include <memory>
#include <functional>

#include "Transform.h"

class Transformable2D
{
protected:
	std::shared_ptr<Transform2D> m_Transform;

public:
	Transformable2D();
	Transformable2D(const Transform2D& transform);
	Transformable2D(const std::shared_ptr<Transform2D>& transform);
	Transformable2D(const Transformable2D&);
	Transformable2D(Transformable2D&&) noexcept;

	inline Transform2D GetTransform() const { return *m_Transform; };
	inline glm::vec2 GetPosition() const { return m_Transform->position; }
	inline glm::float32 GetRotation() const { return m_Transform->rotation; }
	inline glm::vec2 GetScale() const { return m_Transform->scale; }

	inline virtual void OperateTransform(const std::function<void(Transform2D& position)>& op) { op(*m_Transform); }
	inline virtual void OperatePosition(const std::function<void(glm::vec2& position)>& op) { op(m_Transform->position); }
	inline virtual void OperateRotation(const std::function<void(glm::float32& rotation)>& op) { op(m_Transform->rotation); }
	inline virtual void OperateScale(const std::function<void(glm::vec2& scale)>& op) { op(m_Transform->scale); }

	inline void SetTransform(const Transform2D& tr) { OperateTransform([&tr](Transform2D& transform) { transform = tr; }); }
	inline void SetPosition(const float& x, const float& y) { OperatePosition([&x, &y](glm::vec2& position) { position = { x, y }; }); }
	inline void SetPosition(const glm::vec2& pos) { OperatePosition([&pos](glm::vec2& position) { position = pos; }); }
	inline void SetRotation(float r) { OperateRotation([&r](glm::float32& rotation) { rotation = r; }); }
	inline void SetScale(float sx, float sy) { OperateScale([&sx, &sy](glm::vec2& scale) { scale = { sx, sy }; }); }
	inline void SetScale(const glm::vec2& sc) { OperateScale([&sc](glm::vec2& scale) { scale = sc; }); }

	inline virtual bool operator==(const Transformable2D& other) const { return *m_Transform == *other.m_Transform; }
};
