#pragma once

#include <glm/glm.hpp>
#include <functional>
#include <memory>

struct Transform2D
{
	glm::vec2 position = glm::vec2(0.0f, 0.0f);
	glm::float32 rotation = 0.0f;
	glm::vec2 scale = glm::vec2(1.0f, 1.0f);

	Transform2D operator^(const Transform2D& transform) const;
	static Transform2D RelTo(const Transform2D& global, const Transform2D& parent);
	static Transform2D AbsTo(const Transform2D& local, const Transform2D& parent);

	inline bool operator==(const Transform2D& other) const { return position == other.position && rotation == other.rotation && scale == other.scale; }
};

namespace Transform
{
	extern glm::mat3 ToMatrix(const Transform2D& tr);
	extern glm::mat3 ToInverseMatrix(const Transform2D& tr);
	extern glm::mat3x2 ToCondensedMatrix(const Transform2D& tr);
	extern glm::mat2 CondensedRS(const Transform2D& tr);
	extern Transform2D Inverse(const Transform2D& tr);
	extern glm::mat2 Rotation(const glm::float32& r);
}

class Transformable2D
{
protected:
	std::shared_ptr<Transform2D> m_Transform;

public:
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

class LocalTransformer2D
{
	std::shared_ptr<Transform2D> m_Parent;
	std::shared_ptr<Transformable2D> m_Child;
	Transform2D m_Local;

public:
	LocalTransformer2D(const std::shared_ptr<Transform2D>& parent, const std::shared_ptr<Transformable2D>& child, bool discard_old_transform = true);
	LocalTransformer2D(const LocalTransformer2D&);
	LocalTransformer2D(LocalTransformer2D&&) noexcept;
	LocalTransformer2D& operator=(const LocalTransformer2D&);

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

class MultiLocalTransformer2D
{
	std::shared_ptr<Transform2D> m_Parent;
	std::vector<std::shared_ptr<Transformable2D>> m_Children;
	std::vector<Transform2D> m_Locals;

public:
	MultiLocalTransformer2D(const std::shared_ptr<Transform2D>& parent, const std::vector<std::shared_ptr<Transformable2D>>& children, bool discard_old_transforms = true);
	MultiLocalTransformer2D(const MultiLocalTransformer2D&);
	MultiLocalTransformer2D(MultiLocalTransformer2D&&) noexcept;
	MultiLocalTransformer2D& operator=(const MultiLocalTransformer2D&);

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
	inline void SyncGlobalWithParentPositions() { for (size_t i = 0; i < m_Locals.size(); i++) SyncGlobalWithParentPosition(i); }
	inline void SyncGlobalWithParentRotations() { for (size_t i = 0; i < m_Locals.size(); i++) SyncGlobalWithParentRotation(i); }
	inline void SyncGlobalWithParentScales() { for (size_t i = 0; i < m_Locals.size(); i++) SyncGlobalWithParentScale(i); }
	inline void SyncLocalWithParentPositions() { for (size_t i = 0; i < m_Locals.size(); i++) SyncLocalWithParentPosition(i); }
	inline void SyncLocalWithParentRotations() { for (size_t i = 0; i < m_Locals.size(); i++) SyncLocalWithParentRotation(i); }
	inline void SyncLocalWithParentScales() { for (size_t i = 0; i < m_Locals.size(); i++) SyncLocalWithParentScale(i); }

	void SyncGlobalWithLocal(size_t i);
	void SyncLocalWithGlobal(size_t i);
	void SyncGlobalWithParentPosition(size_t i);
	void SyncGlobalWithParentRotation(size_t i);
	void SyncGlobalWithParentScale(size_t i);
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

	void PushBack(const std::shared_ptr<Transformable2D>& child, bool discard_old_transform = true);
	void PushBack(const std::vector<std::shared_ptr<Transformable2D>>& children, bool discard_old_transform = true);
	void Remove(size_t i);
	void Remove(const std::vector<std::shared_ptr<Transformable2D>>::iterator& where);
	std::vector<std::shared_ptr<Transformable2D>>::iterator Find(const std::shared_ptr<Transformable2D>& child);
	inline size_t Size() const { return m_Locals.size(); }
};
