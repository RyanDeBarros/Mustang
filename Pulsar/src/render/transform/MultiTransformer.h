#pragma once

#include "Transform.h"
#include "Transformable.h"

class MultiTransformer2D
{
	std::shared_ptr<Transform2D> m_Parent;
	std::vector<std::shared_ptr<Transformable2D>> m_Children;
	std::vector<Transform2D> m_Locals;

public:
	MultiTransformer2D(const std::shared_ptr<Transform2D>& parent);
	MultiTransformer2D(std::shared_ptr<Transform2D>&& parent);
	MultiTransformer2D(const std::shared_ptr<Transform2D>& parent, const std::vector<std::shared_ptr<Transformable2D>>& children, bool discard_old_transforms = true);
	MultiTransformer2D(std::shared_ptr<Transform2D>&& parent, std::vector<std::shared_ptr<Transformable2D>>&& children, bool discard_old_transforms = true);
	MultiTransformer2D(const std::shared_ptr<Transform2D>& parent, const std::vector<Transform2D>& locals, bool discard_old_transforms = true);
	MultiTransformer2D(std::shared_ptr<Transform2D>&& parent, std::vector<Transform2D>&& locals, bool discard_old_transforms = true);
	MultiTransformer2D(const MultiTransformer2D&);
	MultiTransformer2D(MultiTransformer2D&&) noexcept;
	MultiTransformer2D& operator=(const MultiTransformer2D&);
	MultiTransformer2D& operator=(MultiTransformer2D&&) noexcept;

	void SetLocalTransforms(const Transform2D& tr) { for (size_t i = 0; i < m_Locals.size(); i++) SetLocalTransform(i, tr); }
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
	inline void SyncLocalWithGlobals() { for (size_t i = 0; i < m_Locals.size(); i++) SyncLocalWithGlobal(i); }
	inline void SyncGlobalWithParent() { for (size_t i = 0; i < m_Locals.size(); i++) SyncGlobalWithParent(i); }
	inline void SyncGlobalWithParentPositions() { for (size_t i = 0; i < m_Locals.size(); i++) SyncGlobalWithParentPosition(i); }
	inline void SyncGlobalWithParentRotations() { for (size_t i = 0; i < m_Locals.size(); i++) SyncGlobalWithParentRotation(i); }
	inline void SyncGlobalWithParentScales() { for (size_t i = 0; i < m_Locals.size(); i++) SyncGlobalWithParentScale(i); }
	inline void SyncLocalWithParent() { for (size_t i = 0; i < m_Locals.size(); i++) SyncLocalWithParent(i); }
	inline void SyncLocalWithParentPositions() { for (size_t i = 0; i < m_Locals.size(); i++) SyncLocalWithParentPosition(i); }
	inline void SyncLocalWithParentRotations() { for (size_t i = 0; i < m_Locals.size(); i++) SyncLocalWithParentRotation(i); }
	inline void SyncLocalWithParentScales() { for (size_t i = 0; i < m_Locals.size(); i++) SyncLocalWithParentScale(i); }

	void SyncGlobalWithLocal(size_t i);
	void SyncLocalWithGlobal(size_t i);
	void SyncGlobalWithParent(size_t i);
	void SyncGlobalWithParentPosition(size_t i);
	void SyncGlobalWithParentRotation(size_t i);
	void SyncGlobalWithParentScale(size_t i);
	void SyncLocalWithParent(size_t i);
	void SyncLocalWithParentPosition(size_t i);
	void SyncLocalWithParentRotation(size_t i);
	void SyncLocalWithParentScale(size_t i);

	void SetGlobalTransforms(const Transform2D& tr) { for (size_t i = 0; i < m_Locals.size(); i++) SetGlobalTransform(i, tr); }
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
	inline Transform2D GetGlobalTransform(size_t i) const { return m_Children[i]->GetTransform(); }
	void SetGlobalPosition(size_t i, const glm::vec2& pos);
	void OperateGlobalPosition(size_t i, const std::function<void(glm::vec2& position)>&);
	inline glm::vec2 GetGlobalPosition(size_t i) const { return m_Children[i]->GetTransform().position; }
	void SetGlobalRotation(size_t i, const glm::float32& rot);
	void OperateGlobalRotation(size_t i, const std::function<void(glm::float32& rotation)>&);
	inline glm::float32 GetGlobalRotation(size_t i) const { return m_Children[i]->GetTransform().rotation; }
	void SetGlobalScale(size_t i, const glm::vec2& sc);
	void OperateGlobalScale(size_t i, const std::function<void(glm::vec2& scale)>&);
	inline glm::vec2 GetGlobalScale(size_t i) const { return m_Children[i]->GetTransform().scale; }

	void PushBackGlobal(const std::shared_ptr<Transformable2D>& child, bool discard_old_transform = true);
	void PushBackGlobal(std::shared_ptr<Transformable2D>&& child, bool discard_old_transform = true);
	void PushBackLocal(const Transform2D& local, bool discard_old_transform = true);
	void PushBackGlobals(const std::vector<std::shared_ptr<Transformable2D>>& children, bool discard_old_transform = true);
	void PushBackGlobals(std::vector<std::shared_ptr<Transformable2D>>&& children, bool discard_old_transform = true);
	void PushBackLocals(const std::vector<Transform2D>& locals, bool discard_old_transform = true);
	void Remove(size_t i);
	void Remove(const std::vector<std::shared_ptr<Transformable2D>>::iterator& where);
	std::vector<std::shared_ptr<Transformable2D>>::iterator Find(const std::shared_ptr<Transformable2D>& child);
	inline size_t Size() const { return m_Locals.size(); }
};
