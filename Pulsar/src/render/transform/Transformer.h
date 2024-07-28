#pragma once

#include "Transform.h"
#include "Transformable.h"

class Transformer2D : public Transformable2D
{
	// TODO consider using weak ptrs here and throughout project. TransformableProxy2D can then also extend std::enable_shared_from_this, and define copy constructor/assignment operator PROVIDED that weak ptr can be used here. It can extend it privately, and have an inline protected method weak_ref().
	std::weak_ptr<Transformable2D> m_Parent;
	std::weak_ptr<Transformable2D> m_Child;
	Transform2D m_Local;

public:
	Transformer2D(const std::weak_ptr<Transformable2D>& parent, const std::weak_ptr<Transformable2D>& child, bool discard_old_transform = true);
	Transformer2D(const std::weak_ptr<Transformable2D>& parent, std::weak_ptr<Transformable2D>&& child, bool discard_old_transform = true);
	Transformer2D(std::weak_ptr<Transformable2D>&& parent, const std::weak_ptr<Transformable2D>& child, bool discard_old_transform = true);
	Transformer2D(std::weak_ptr<Transformable2D>&& parent, std::weak_ptr<Transformable2D>&& child, bool discard_old_transform = true);
	Transformer2D(const std::weak_ptr<Transformable2D>& parent, const Transform2D& local, bool discard_old_transform = true);
	Transformer2D(std::weak_ptr<Transformable2D>&& parent, const Transform2D& local, bool discard_old_transform = true);
	Transformer2D(const Transformer2D&);
	Transformer2D(Transformer2D&&) noexcept;
	Transformer2D& operator=(const Transformer2D&);
	Transformer2D& operator=(Transformer2D&&) noexcept;

	inline Transform2D GetTransform() const override { WEAK_LOCK_CHECK(m_Parent, p) return p->GetTransform(); }
	inline glm::vec2 GetPosition() const override { WEAK_LOCK_CHECK(m_Parent, p) return p->GetPosition(); }
	inline glm::float32 GetRotation() const override { WEAK_LOCK_CHECK(m_Parent, p) return p->GetRotation(); }
	inline glm::vec2 GetScale() const override { WEAK_LOCK_CHECK(m_Parent, p) return p->GetScale(); }

	// TODO buffer versions, at least for MultiTransformer, so that parent sync can be delayed. also consider parent-independent functions that don't sync children with transforms on parent.
	inline void OperateTransform(const std::function<void(Transform2D& position)>& op) { WEAK_LOCK_CHECK(m_Parent, p) p->OperateTransform(op); SyncGlobalWithParent(); }
	inline void OperatePosition(const std::function<void(glm::vec2& position)>& op) { WEAK_LOCK_CHECK(m_Parent, p) p->OperatePosition(op); SyncGlobalWithParentPosition(); }
	inline void OperateRotation(const std::function<void(glm::float32& rotation)>& op) { WEAK_LOCK_CHECK(m_Parent, p) p->OperateRotation(op); SyncGlobalWithParentRotation(); }
	inline void OperateScale(const std::function<void(glm::vec2& scale)>& op) { WEAK_LOCK_CHECK(m_Parent, p) p->OperateScale(op); SyncGlobalWithParentScale(); }

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

	void SetGlobalTransform(const Transform2D& tr);
	inline Transform2D GetGlobalTransform() const { WEAK_LOCK_CHECK(m_Child, c) return c->GetTransform(); }
	void SetGlobalPosition(const glm::vec2& pos);
	void OperateGlobalPosition(const std::function<void(glm::vec2& position)>&);
	inline glm::vec2 GetGlobalPosition() const { WEAK_LOCK_CHECK(m_Child, c) return c->GetPosition(); }
	void SetGlobalRotation(const glm::float32& rot);
	void OperateGlobalRotation(const std::function<void(glm::float32& rotation)>&);
	inline glm::float32 GetGlobalRotation() const { WEAK_LOCK_CHECK(m_Child, c) return c->GetRotation(); }
	void SetGlobalScale(const glm::vec2& sc);
	void OperateGlobalScale(const std::function<void(glm::vec2& scale)>&);
	inline glm::vec2 GetGlobalScale() const { WEAK_LOCK_CHECK(m_Child, c) return c->GetScale(); }

private:
	void SyncGlobalWithParent();
	void SyncGlobalWithParentPosition();
	void SyncGlobalWithParentRotation();
	void SyncGlobalWithParentScale();
	void SyncLocalWithParent();
	void SyncLocalWithParentPosition();
	void SyncLocalWithParentRotation();
	void SyncLocalWithParentScale();

};