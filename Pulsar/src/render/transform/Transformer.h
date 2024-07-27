#pragma once

#include "Transform.h"
#include "Transformable.h"

// TODO make Transformer and MultiTransformer a Transformable, so they can be chained. The parent will be the transformer.

class Transformer2D : public Transformable2D
{
	std::shared_ptr<Transformable2D> m_Child;
	Transform2D m_Local;

public:
	Transformer2D(const std::shared_ptr<Transform2D>& parent, const std::shared_ptr<Transformable2D>& child, bool discard_old_transform = true);
	Transformer2D(const std::shared_ptr<Transform2D>& parent, std::shared_ptr<Transformable2D>&& child, bool discard_old_transform = true);
	Transformer2D(std::shared_ptr<Transform2D>&& parent, std::shared_ptr<Transformable2D>&& child, bool discard_old_transform = true);
	Transformer2D(const std::shared_ptr<Transform2D>& parent, const Transform2D& local, bool discard_old_transform = true);
	Transformer2D(std::shared_ptr<Transform2D>&& parent, const Transform2D& local, bool discard_old_transform = true);
	Transformer2D(const Transformer2D&);
	Transformer2D(Transformer2D&&) noexcept;
	Transformer2D& operator=(const Transformer2D&);
	Transformer2D& operator=(Transformer2D&&) noexcept;

	inline virtual void OperateTransform(const std::function<void(Transform2D& position)>& op) { op(*m_Transform); SyncGlobalWithParent(); }
	inline virtual void OperatePosition(const std::function<void(glm::vec2& position)>& op) { op(m_Transform->position); SyncGlobalWithParentPosition(); }
	inline virtual void OperateRotation(const std::function<void(glm::float32& rotation)>& op) { op(m_Transform->rotation); SyncGlobalWithParentRotation(); }
	inline virtual void OperateScale(const std::function<void(glm::vec2& scale)>& op) { op(m_Transform->scale); SyncGlobalWithParentScale(); }

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