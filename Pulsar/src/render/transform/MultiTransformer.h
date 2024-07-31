#pragma once

#include "Transformable.h"
#include "Logger.h"

class MultiTransformer2D : public Transformable2D
{
	std::weak_ptr<Transformable2D> m_Parent;
	std::vector<std::weak_ptr<Transformable2D>> m_Children;
	std::vector<Transform2D> m_Locals;

public:
	MultiTransformer2D() = delete;
	MultiTransformer2D(const std::weak_ptr<Transformable2D>& parent);
	MultiTransformer2D(std::weak_ptr<Transformable2D>&& parent);
	
	MultiTransformer2D(const std::weak_ptr<Transformable2D>& parent, const std::vector<std::weak_ptr<Transformable2D>>& children, bool discard_old_transforms = true);
	MultiTransformer2D(const std::weak_ptr<Transformable2D>& parent, std::vector<std::weak_ptr<Transformable2D>>&& children, bool discard_old_transforms = true);
	MultiTransformer2D(std::weak_ptr<Transformable2D>&& parent, const std::vector<std::weak_ptr<Transformable2D>>& children, bool discard_old_transforms = true);
	MultiTransformer2D(std::weak_ptr<Transformable2D>&& parent, std::vector<std::weak_ptr<Transformable2D>>&& children, bool discard_old_transforms = true);
	
	MultiTransformer2D(const std::weak_ptr<Transformable2D>& parent, const std::vector<Transform2D>& locals, bool discard_old_transforms = true);
	MultiTransformer2D(const std::weak_ptr<Transformable2D>& parent, std::vector<Transform2D>&& locals, bool discard_old_transforms = true);
	MultiTransformer2D(std::weak_ptr<Transformable2D>&& parent, const std::vector<Transform2D>& locals, bool discard_old_transforms = true);
	MultiTransformer2D(std::weak_ptr<Transformable2D>&& parent, std::vector<Transform2D>&& locals, bool discard_old_transforms = true);
	
	MultiTransformer2D(const MultiTransformer2D&);
	MultiTransformer2D(MultiTransformer2D&&) noexcept;
	MultiTransformer2D& operator=(const MultiTransformer2D&);
	MultiTransformer2D& operator=(MultiTransformer2D&&) noexcept;

	inline void SetParent(const std::weak_ptr<Transformable2D>& parent) { m_Parent = parent; }
	inline void SetParent(std::weak_ptr<Transformable2D>&& parent) { m_Parent = std::move(parent); }

	inline Transform2D GetTransform() const override { WEAK_LOCK_CHECK(m_Parent, p) return p->GetTransform(); };
	inline glm::vec2 GetPosition() const override { WEAK_LOCK_CHECK(m_Parent, p) return p->GetPosition(); }
	inline glm::float32 GetRotation() const override { WEAK_LOCK_CHECK(m_Parent, p) return p->GetRotation(); }
	inline glm::vec2 GetScale() const override { WEAK_LOCK_CHECK(m_Parent, p) return p->GetScale(); }

	inline void OperateTransform(const std::function<void(Transform2D& position)>& op) override { WEAK_LOCK_CHECK(m_Parent, p) p->OperateTransform(op); SyncGlobalWithLocals(); }
	inline void OperatePosition(const std::function<void(glm::vec2& position)>& op) override { WEAK_LOCK_CHECK(m_Parent, p) p->OperatePosition(op); SyncGlobalWithLocalPositions(); }
	inline void OperateRotation(const std::function<void(glm::float32& rotation)>& op) override { WEAK_LOCK_CHECK(m_Parent, p) p->OperateRotation(op); SyncGlobalWithLocalRotations(); }
	inline void OperateScale(const std::function<void(glm::vec2& scale)>& op) override { WEAK_LOCK_CHECK(m_Parent, p) p->OperateScale(op); SyncGlobalWithLocalScales(); }

	void SetLocalTransforms(const Transform2D& tr) { for (size_t i = 0; i < m_Locals.size(); i++) SetLocalTransform(i, tr); }
	inline void OperateLocalTransforms(const std::function<void(Transform2D& position)>& op) { for (size_t i = 0; i < m_Locals.size(); i++) OperateLocalTransform(i, op); }
	inline std::vector<Transform2D> GetLocalTransforms() const { return m_Locals; }
	inline void SetLocalPositions(const glm::vec2& pos) { for (size_t i = 0; i < m_Locals.size(); i++) SetLocalPosition(i, pos); }
	inline void OperateLocalPositions(const std::function<void(glm::vec2& position)>& op) { for (size_t i = 0; i < m_Locals.size(); i++) OperateLocalPosition(i, op); }
	std::vector<glm::vec2> GetLocalPositions() const;
	inline void SetLocalRotations(const glm::float32& rot) { for (size_t i = 0; i < m_Locals.size(); i++) SetLocalRotation(i, rot); }
	inline void OperateLocalRotations(const std::function<void(glm::float32& rotation)>& op) { for (size_t i = 0; i < m_Locals.size(); i++) OperateLocalRotation(i, op); }
	std::vector<glm::float32> GetLocalRotations() const;
	inline void SetLocalScales(const glm::vec2& sc) { for (size_t i = 0; i < m_Locals.size(); i++) SetLocalScale(i, sc); }
	inline void OperateLocalScales(const std::function<void(glm::vec2& scale)>& op) { for (size_t i = 0; i < m_Locals.size(); i++) OperateLocalScale(i, op); }
	std::vector<glm::vec2> GetLocalScales() const;

	void SetLocalTransform(size_t i, const Transform2D& tr);
	void OperateLocalTransform(size_t i, const std::function<void(Transform2D& transform)>&);
	inline Transform2D GetLocalTransform(size_t i) const { return m_Locals[i]; }
	void SetLocalPosition(size_t i, const glm::vec2& pos);
	void OperateLocalPosition(size_t i, const std::function<void(glm::vec2& position)>&);
	inline glm::vec2 GetLocalPosition(size_t i) const { return m_Locals[i].position; }
	void SetLocalRotation(size_t i, const glm::float32& rot);
	void OperateLocalRotation(size_t i, const std::function<void(glm::float32& rotation)>&);
	inline glm::float32 GetLocalRotation(size_t i) const { return m_Locals[i].rotation; }
	void SetLocalScale(size_t i, const glm::vec2& sc);
	void OperateLocalScale(size_t i, const std::function<void(glm::vec2& scale)>&);
	inline glm::vec2 GetLocalScale(size_t i) const { return m_Locals[i].scale; }

	inline void SyncGlobalWithLocals() { for (size_t i = 0; i < m_Locals.size(); i++) SyncGlobalWithLocal(i); }
	inline void SyncGlobalWithLocalPositions() { for (size_t i = 0; i < m_Locals.size(); i++) SyncGlobalWithLocalPosition(i); }
	inline void SyncGlobalWithLocalRotations() { for (size_t i = 0; i < m_Locals.size(); i++) SyncGlobalWithLocalRotation(i); }
	inline void SyncGlobalWithLocalScales() { for (size_t i = 0; i < m_Locals.size(); i++) SyncGlobalWithLocalScale(i); }
	inline void SyncLocalWithGlobals() { for (size_t i = 0; i < m_Locals.size(); i++) SyncLocalWithGlobal(i); }
	inline void SyncLocalWithGlobalPositions() { for (size_t i = 0; i < m_Locals.size(); i++) SyncLocalWithGlobalPosition(i); }
	inline void SyncLocalWithGlobalRotations() { for (size_t i = 0; i < m_Locals.size(); i++) SyncLocalWithGlobalRotation(i); }
	inline void SyncLocalWithGlobalScales() { for (size_t i = 0; i < m_Locals.size(); i++) SyncLocalWithGlobalScale(i); }

	void SyncGlobalWithLocal(size_t i);
	void SyncGlobalWithLocalPosition(size_t i);
	void SyncGlobalWithLocalRotation(size_t i);
	void SyncGlobalWithLocalScale(size_t i);
	void SyncLocalWithGlobal(size_t i);
	void SyncLocalWithGlobalPosition(size_t i);
	void SyncLocalWithGlobalRotation(size_t i);
	void SyncLocalWithGlobalScale(size_t i);

	void SetGlobalTransforms(const Transform2D& tr) { for (size_t i = 0; i < m_Locals.size(); i++) SetGlobalTransform(i, tr); }
	void OperateGlobalTransforms(const std::function<void(Transform2D& transform)>& op) { for (size_t i = 0; i < m_Locals.size(); i++) OperateGlobalTransform(i, op); }
	std::vector<Transform2D> GetGlobalTransforms() const;
	void SetGlobalPositions(const glm::vec2& pos) { for (size_t i = 0; i < m_Locals.size(); i++) SetGlobalPosition(i, pos); }
	void OperateGlobalPositions(const std::function<void(glm::vec2& position)>& op) { for (size_t i = 0; i < m_Locals.size(); i++) OperateGlobalPosition(i, op); }
	std::vector<glm::vec2> GetGlobalPositions() const;
	void SetGlobalRotations(const glm::float32& rot) { for (size_t i = 0; i < m_Locals.size(); i++) SetGlobalRotation(i, rot); }
	void OperateGlobalRotations(const std::function<void(glm::float32& rotation)>& op) { for (size_t i = 0; i < m_Locals.size(); i++) OperateGlobalRotation(i, op); }
	std::vector<glm::float32> GetGlobalRotations() const;
	void SetGlobalScales(const glm::vec2& sc) { for (size_t i = 0; i < m_Locals.size(); i++) SetGlobalScale(i, sc); }
	void OperateGlobalScales(const std::function<void(glm::vec2& scale)>& op) { for (size_t i = 0; i < m_Locals.size(); i++) OperateGlobalScale(i, op); }
	std::vector<glm::vec2> GetGlobalScales() const;

	void SetGlobalTransform(size_t i, const Transform2D& tr);
	void OperateGlobalTransform(size_t i, const std::function<void(Transform2D& transform)>&);
	inline Transform2D GetGlobalTransform(size_t i) const { WEAK_LOCK_CHECK(m_Children[i], c) return c->GetTransform(); }
	void SetGlobalPosition(size_t i, const glm::vec2& pos);
	void OperateGlobalPosition(size_t i, const std::function<void(glm::vec2& position)>&);
	inline glm::vec2 GetGlobalPosition(size_t i) const { WEAK_LOCK_CHECK(m_Children[i], c) return c->GetTransform().position; }
	void SetGlobalRotation(size_t i, const glm::float32& rot);
	void OperateGlobalRotation(size_t i, const std::function<void(glm::float32& rotation)>&);
	inline glm::float32 GetGlobalRotation(size_t i) const { WEAK_LOCK_CHECK(m_Children[i], c) return c->GetTransform().rotation; }
	void SetGlobalScale(size_t i, const glm::vec2& sc);
	void OperateGlobalScale(size_t i, const std::function<void(glm::vec2& scale)>&);
	inline glm::vec2 GetGlobalScale(size_t i) const { WEAK_LOCK_CHECK(m_Children[i], c) return c->GetTransform().scale; }

	void PushBackGlobal(const std::weak_ptr<Transformable2D>& child, bool discard_old_transform = true);
	void PushBackGlobal(std::weak_ptr<Transformable2D>&& child, bool discard_old_transform = true);
	void PushBackGlobals(const std::vector<std::weak_ptr<Transformable2D>>& children, bool discard_old_transform = true);
	void PushBackGlobals(std::vector<std::weak_ptr<Transformable2D>>&& children, bool discard_old_transform = true);
	std::shared_ptr<TransformableProxy2D> PushBackLocal(const Transform2D& local, bool discard_old_transform = true);
	std::vector<std::shared_ptr<TransformableProxy2D>> PushBackLocals(const std::vector<Transform2D>& locals, bool discard_old_transform = true);
	void Remove(size_t i);
	void Remove(const std::vector<std::weak_ptr<Transformable2D>>::iterator& where);
	std::vector<std::weak_ptr<Transformable2D>>::iterator Find(const std::weak_ptr<Transformable2D>& child);
	inline size_t Size() const { return m_Locals.size(); }
};
