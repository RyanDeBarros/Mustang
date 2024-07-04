#pragma once

#include <glm/glm.hpp>
#include <functional>

struct Transform2D
{
	glm::vec2 position = glm::vec2(0.0f, 0.0f);
	glm::float32 rotation = 0.0f;
	glm::vec2 scale = glm::vec2(1.0f, 1.0f);

	Transform2D operator^(const Transform2D& transform) const;
	static Transform2D RelTo(const Transform2D& global, const Transform2D& parent);
	static Transform2D AbsTo(const Transform2D& local, const Transform2D& parent);
};

namespace Transform
{
	extern glm::mat3 ToMatrix(const Transform2D& tr);
	extern glm::mat3 ToInverseMatrix(const Transform2D& tr);
	extern glm::mat3x2 ToCondensedMatrix(const Transform2D& tr);
	extern Transform2D Inverse(const Transform2D& tr);
	extern glm::mat2 Rotation(const glm::float32& r);
}

class Transformable2D
{
public:
	virtual Transform2D GetTransform() const = 0;
	virtual void SetTransform(const Transform2D&) = 0;
	virtual glm::vec2 GetPosition() const = 0;
	virtual void OperatePosition(const std::function<void(glm::vec2& position)>&) = 0;
	virtual glm::float32 GetRotation() const = 0;
	virtual void OperateRotation(const std::function<void(glm::float32& rotation)>&) = 0;
	virtual glm::vec2 GetScale() const = 0;
	virtual void OperateScale(const std::function<void(glm::vec2& scale)>&) = 0;
	
	inline void SetPosition(const float& x, const float& y) { OperatePosition([&x, &y](glm::vec2& position) { position = { x, y }; }); }
	inline void SetPosition(const glm::vec2& pos) { OperatePosition([&pos](glm::vec2& position) { position = pos; }); }
	inline void SetRotation(float r) { OperateRotation([&r](glm::float32& rotation) { rotation = r; }); }
	inline void SetScale(float sx, float sy) { OperateScale([&sx, &sy](glm::vec2& scale) { scale = { sx, sy }; }); }
	inline void SetScale(const glm::vec2& sc) { OperateScale([&sc](glm::vec2& scale) { scale = sc; }); }
};

class TransformProxy2D : public Transformable2D
{
	Transform2D m_Transform;
public:
	TransformProxy2D(const Transform2D& tr) : m_Transform(tr) {}
	inline Transform2D GetTransform() const override { return m_Transform; }
	inline void SetTransform(const Transform2D& tr) override { m_Transform = tr; }
	inline glm::vec2 GetPosition() const override { return m_Transform.position; }
	inline void OperatePosition(const std::function<void(glm::vec2& position)>& op) override { op(m_Transform.position); }
	inline glm::float32 GetRotation() const override { return m_Transform.rotation; }
	inline void OperateRotation(const std::function<void(glm::float32& rotation)>& op) override { op(m_Transform.rotation); }
	inline glm::vec2 GetScale() const override { return m_Transform.scale; }
	inline void OperateScale(const std::function<void(glm::vec2& scale)>& op) override { op(m_Transform.scale); }
};

class LocalTransformer2D
{
	Transform2D* const m_Parent;
	Transformable2D* const m_Child;
	Transform2D m_Local;

public:
	LocalTransformer2D(Transform2D* const parent, Transformable2D* const child);

	void SetLocalTransform(const Transform2D& tr);
	inline Transform2D GetLocalTransform() const { return m_Local; }
	void SetLocalPosition(const glm::vec2& pos);
	void OperateLocalPosition(const std::function<void(glm::vec2& position)>&);
	inline glm::vec2 GetLocalPosition() const { return m_Local.position; }
	void SetLocalRotation(const glm::float32& rot);
	void OperateLocalRotation(const std::function<void(glm::float32& rotation)>&);
	inline glm::float32 GetLocalRotation() const { return m_Local.rotation; }
	void SetLocalScale(const glm::vec2& sc);
	void OperateLocalScale(const std::function<void(glm::vec2& scale)>&);
	inline glm::vec2 GetLocalScale() const { return m_Local.scale; }

	void SyncGlobalWithLocal();
	void SyncLocalWithGlobal();
	void SyncGlobalWithParentPosition();
	void SyncGlobalWithParentRotation();
	void SyncGlobalWithParentScale();
	void SyncLocalWithParentPosition();
	void SyncLocalWithParentRotation();
	void SyncLocalWithParentScale();
	
	void SetGlobalTransform(const Transform2D& tr);
	inline Transform2D GetGlobalTransform() const { return m_Child->GetTransform(); }
	void SetGlobalPosition(const glm::vec2& pos);
	void OperateGlobalPosition(const std::function<void(glm::vec2& position)>&);
	inline glm::vec2 GetGlobalPosition() const { return m_Child->GetTransform().position; }
	void SetGlobalRotation(const glm::float32& rot);
	void OperateGlobalRotation(const std::function<void(glm::float32& rotation)>&);
	inline glm::float32 GetGlobalRotation() const { return m_Child->GetTransform().rotation; }
	void SetGlobalScale(const glm::vec2& sc);
	void OperateGlobalScale(const std::function<void(glm::vec2& scale)>&);
	inline glm::vec2 GetGlobalScale() const { return m_Child->GetTransform().scale; }
};
