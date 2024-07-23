#include "Transform.h"

static inline glm::vec2 operator/(const float& f, const glm::vec2& v)
{
	return glm::vec2(1 / v.x, 1 / v.y);
}

namespace Transform {
	
	glm::mat3 ToMatrix(const Transform2D& tr)
	{
		return glm::mat3(tr.scale.x * cosf(tr.rotation), tr.scale.x * sinf(tr.rotation), 0, -tr.scale.y * sinf(tr.rotation), tr.scale.y * cosf(tr.rotation), 0, tr.position.x, tr.position.y, 1);
	}

	glm::mat3 ToInverseMatrix(const Transform2D& tr)
	{
		return glm::mat3(cosf(tr.rotation) / tr.scale.x, -sinf(tr.rotation) / tr.scale.x, 0, sinf(tr.rotation) / tr.scale.y, cosf(tr.rotation) / tr.scale.y, 0, -tr.position.x, -tr.position.y, 1);
	}

	glm::mat3x2 ToCondensedMatrix(const Transform2D& tr)
	{
		return glm::mat3x2(tr.position.x, tr.position.y, tr.scale.x * cosf(tr.rotation), tr.scale.x * sinf(tr.rotation), -tr.scale.y * sinf(tr.rotation), tr.scale.y * cosf(tr.rotation));
	}

	glm::mat2 CondensedRS(const Transform2D& tr)
	{
		return glm::mat2(tr.scale.x * cosf(tr.rotation), tr.scale.x * sinf(tr.rotation), -tr.scale.y * sinf(tr.rotation), tr.scale.y * cosf(tr.rotation));
	}
	
	Transform2D Inverse(const Transform2D& tr)
	{
		return { -tr.position, -tr.rotation, 1 / tr.scale };
	}

	glm::mat2 Rotation(const glm::float32& r)
	{
		return { glm::cos(r), glm::sin(r), -glm::sin(r), glm::cos(r) };
	}

	Transform2D RelTo(const Transform2D& global, const Transform2D& parent)
	{
		return { (Transform::Rotation(-parent.rotation) * (global.position - parent.position)) / parent.scale, global.rotation - parent.rotation, global.scale / parent.scale };
	}

	Transform2D AbsTo(const Transform2D& local, const Transform2D& parent)
	{
		return { parent.position + Transform::Rotation(parent.rotation) * (parent.scale * local.position), parent.rotation + local.rotation, parent.scale * local.scale };
	}

}

Transform2D Transform2D::operator^(const Transform2D& transform) const
{
	return { position + transform.position, rotation + transform.rotation, scale * transform.scale };
}

LocalTransformer2D::LocalTransformer2D(const std::shared_ptr<Transform2D>& parent, const std::shared_ptr<Transformable2D>& child, bool discard_old_transform)
	: m_Parent(parent), m_Child(child)
{
	if (discard_old_transform)
		SyncGlobalWithLocal();
	else
		SyncLocalWithGlobal();
}

LocalTransformer2D::LocalTransformer2D(const std::shared_ptr<Transform2D>& parent, const Transform2D& local, bool discard_old_transform)
	: m_Parent(parent), m_Local(local)
{
	m_Child = std::make_shared<Transformable2D>(Transformable2D(Transform2D{}));
	if (discard_old_transform)
		SyncLocalWithGlobal();
	else
		SyncGlobalWithLocal();
}

Transformable2D::Transformable2D(const Transform2D& transform)
	: m_Transform(std::make_shared<Transform2D>(transform))
{
}

Transformable2D::Transformable2D(const std::shared_ptr<Transform2D>& transform)
	: m_Transform(transform)
{
}

Transformable2D::Transformable2D(const Transformable2D& other)
	: m_Transform(other.m_Transform)
{
}

Transformable2D::Transformable2D(Transformable2D&& other) noexcept
	: m_Transform(other.m_Transform)
{
}

LocalTransformer2D::LocalTransformer2D(const LocalTransformer2D& other)
	: m_Parent(other.m_Parent), m_Child(other.m_Child), m_Local(other.m_Local)
{
}

LocalTransformer2D::LocalTransformer2D(LocalTransformer2D&& other) noexcept
	: m_Parent(other.m_Parent), m_Child(other.m_Child), m_Local(other.m_Local)
{
}

LocalTransformer2D& LocalTransformer2D::operator=(const LocalTransformer2D& other)
{
	m_Parent = other.m_Parent;
	m_Child = other.m_Child;
	m_Local = other.m_Local;
	return *this;
}

void LocalTransformer2D::SetLocalTransform(const Transform2D& tr)
{
	m_Local = tr;
	SyncGlobalWithLocal();
}

void LocalTransformer2D::SetLocalPosition(const glm::vec2& pos)
{
	m_Local.position = pos;
	m_Child->SetPosition(m_Parent->position + Transform::Rotation(m_Parent->rotation) * (m_Parent->scale * m_Local.position));
}

void LocalTransformer2D::OperateLocalPosition(const std::function<void(glm::vec2& position)>& op)
{
	op(m_Local.position);
	m_Child->SetPosition(m_Parent->position + Transform::Rotation(m_Parent->rotation) * (m_Parent->scale * m_Local.position));
}

void LocalTransformer2D::SetLocalRotation(const glm::float32& rot)
{
	m_Local.rotation = rot;
	m_Child->SetRotation(m_Parent->rotation + m_Local.rotation);
}

void LocalTransformer2D::OperateLocalRotation(const std::function<void(glm::float32& rotation)>& op)
{
	op(m_Local.rotation);
	m_Child->SetRotation(m_Parent->rotation + m_Local.rotation);
}

void LocalTransformer2D::SetLocalScale(const glm::vec2& sc)
{
	m_Local.scale = sc;
	m_Child->SetScale(m_Parent->scale * m_Local.scale);
}

void LocalTransformer2D::OperateLocalScale(const std::function<void(glm::vec2& scale)>& op)
{
	op(m_Local.scale);
	m_Child->SetScale(m_Parent->scale * m_Local.scale);
}

void LocalTransformer2D::SyncGlobalWithLocal()
{
	m_Child->SetTransform(Transform::AbsTo(m_Local, *m_Parent));
}

void LocalTransformer2D::SyncLocalWithGlobal()
{
	m_Local = Transform::RelTo(m_Child->GetTransform(), *m_Parent);
}

void LocalTransformer2D::SyncGlobalWithParent()
{
	m_Child->SetPosition(m_Parent->position + Transform::Rotation(m_Parent->rotation) * (m_Parent->scale * m_Local.position));
	m_Child->SetRotation(m_Parent->rotation + m_Local.rotation);
	m_Child->SetScale(m_Parent->scale * m_Local.scale);
}

void LocalTransformer2D::SyncGlobalWithParentPosition()
{
	m_Child->SetPosition(m_Parent->position + Transform::Rotation(m_Parent->rotation) * (m_Parent->scale * m_Local.position));
}

void LocalTransformer2D::SyncGlobalWithParentRotation()
{
	m_Child->SetPosition(m_Parent->position + Transform::Rotation(m_Parent->rotation) * (m_Parent->scale * m_Local.position));
	m_Child->SetRotation(m_Parent->rotation + m_Local.rotation);
}

void LocalTransformer2D::SyncGlobalWithParentScale()
{
	m_Child->SetPosition(m_Parent->position + Transform::Rotation(m_Parent->rotation) * (m_Parent->scale * m_Local.position));
	m_Child->SetScale(m_Parent->scale * m_Local.scale);
}

void LocalTransformer2D::SyncLocalWithParent()
{
	m_Local.position = (Transform::Rotation(-m_Parent->rotation) * (m_Child->GetPosition() - m_Parent->position)) / m_Parent->scale;
	m_Local.rotation = m_Child->GetRotation() - m_Parent->rotation;
	m_Local.scale = m_Child->GetScale() / m_Parent->scale;
}

void LocalTransformer2D::SyncLocalWithParentPosition()
{
	m_Local.position = (Transform::Rotation(-m_Parent->rotation) * (m_Child->GetPosition() - m_Parent->position)) / m_Parent->scale;
}

void LocalTransformer2D::SyncLocalWithParentRotation()
{
	m_Local.position = (Transform::Rotation(-m_Parent->rotation) * (m_Child->GetPosition() - m_Parent->position)) / m_Parent->scale;
	m_Local.rotation = m_Child->GetRotation() - m_Parent->rotation;
}

void LocalTransformer2D::SyncLocalWithParentScale()
{
	m_Local.position = (Transform::Rotation(-m_Parent->rotation) * (m_Child->GetPosition() - m_Parent->position)) / m_Parent->scale;
	m_Local.scale = m_Child->GetScale() / m_Parent->scale;
}

void LocalTransformer2D::SetGlobalTransform(const Transform2D& tr)
{
	m_Child->SetTransform(tr);
	SyncLocalWithGlobal();
}

void LocalTransformer2D::SetGlobalPosition(const glm::vec2& pos)
{
	m_Child->SetPosition(pos);
	m_Local.position = (Transform::Rotation(-m_Parent->rotation) * (m_Child->GetPosition() - m_Parent->position)) / m_Parent->scale;
}

void LocalTransformer2D::OperateGlobalPosition(const std::function<void(glm::vec2& position)>& op)
{
	m_Child->OperatePosition(op);
	m_Local.position = (Transform::Rotation(-m_Parent->rotation) * (m_Child->GetPosition() - m_Parent->position)) / m_Parent->scale;
}

void LocalTransformer2D::SetGlobalRotation(const glm::float32& rot)
{
	m_Child->SetRotation(rot);
	m_Local.rotation = m_Child->GetRotation() - m_Parent->rotation;
}

void LocalTransformer2D::OperateGlobalRotation(const std::function<void(glm::float32& rotation)>& op)
{
	m_Child->OperateRotation(op);
	m_Local.rotation = m_Child->GetRotation() - m_Parent->rotation;
}

void LocalTransformer2D::SetGlobalScale(const glm::vec2& sc)
{
	m_Child->SetScale(sc);
	m_Local.scale = m_Child->GetScale() / m_Parent->scale;
}

void LocalTransformer2D::OperateGlobalScale(const std::function<void(glm::vec2& scale)>& op)
{
	m_Child->OperateScale(op);
	m_Local.scale = m_Child->GetScale() / m_Parent->scale;
}

MultiLocalTransformer2D::MultiLocalTransformer2D(const std::shared_ptr<Transform2D>& parent, const std::vector<std::shared_ptr<Transformable2D>>& children, bool discard_old_transforms)
	: m_Parent(parent), m_Children(children)
{
	for (size_t i = 0; i < m_Children.size(); i++)
		m_Locals.push_back({});
	if (discard_old_transforms)
		SyncGlobalWithLocals();
	else
		SyncLocalWithGlobals();
}

MultiLocalTransformer2D::MultiLocalTransformer2D(const std::shared_ptr<Transform2D>& parent, const std::vector<Transform2D>& locals, bool discard_old_transforms)
	: m_Parent(parent), m_Locals(locals)
{
	for (size_t i = 0; i < m_Children.size(); i++)
		m_Children.push_back(std::make_shared<Transformable2D>(Transformable2D(Transform2D{})));
	if (discard_old_transforms)
		SyncLocalWithGlobals();
	else
		SyncGlobalWithLocals();
}

MultiLocalTransformer2D::MultiLocalTransformer2D(const MultiLocalTransformer2D& other)
	: m_Parent(other.m_Parent), m_Children(other.m_Children), m_Locals(other.m_Locals)
{
}

MultiLocalTransformer2D::MultiLocalTransformer2D(MultiLocalTransformer2D&& other) noexcept
	: m_Parent(other.m_Parent), m_Children(other.m_Children), m_Locals(other.m_Locals)
{
}

MultiLocalTransformer2D& MultiLocalTransformer2D::operator=(const MultiLocalTransformer2D& other)
{
	m_Parent = other.m_Parent;
	m_Children = other.m_Children;
	m_Locals = other.m_Locals;
	return *this;
}

std::vector<glm::vec2> MultiLocalTransformer2D::GetLocalPositions() const
{
	std::vector<glm::vec2> positions;
	for (size_t i = 0; i < m_Locals.size(); i++)
		positions.push_back(GetLocalPosition(i));
	return positions;
}

std::vector<glm::float32> MultiLocalTransformer2D::GetLocalRotations() const
{
	std::vector<glm::float32> rotations;
	for (size_t i = 0; i < m_Locals.size(); i++)
		rotations.push_back(GetLocalRotation(i));
	return rotations;
}

std::vector<glm::vec2> MultiLocalTransformer2D::GetLocalScales() const
{
	std::vector<glm::vec2> scales;
	for (size_t i = 0; i < m_Locals.size(); i++)
		scales.push_back(GetLocalScale(i));
	return scales;
}

void MultiLocalTransformer2D::SetLocalTransform(size_t i, const Transform2D& tr)
{
	m_Locals[i] = tr;
	SyncGlobalWithLocal(i);
}

void MultiLocalTransformer2D::SetLocalPosition(size_t i, const glm::vec2& pos)
{
	m_Locals[i].position = pos;
	m_Children[i]->SetPosition(m_Parent->position + Transform::Rotation(m_Parent->rotation) * (m_Parent->scale * m_Locals[i].position));
}

void MultiLocalTransformer2D::OperateLocalPosition(size_t i, const std::function<void(glm::vec2& position)>& op)
{
	op(m_Locals[i].position);
	m_Children[i]->SetPosition(m_Parent->position + Transform::Rotation(m_Parent->rotation) * (m_Parent->scale * m_Locals[i].position));
}

void MultiLocalTransformer2D::SetLocalRotation(size_t i, const glm::float32& rot)
{
	m_Locals[i].rotation = rot;
	m_Children[i]->SetRotation(m_Parent->rotation + m_Locals[i].rotation);
}

void MultiLocalTransformer2D::OperateLocalRotation(size_t i, const std::function<void(glm::float32& rotation)>& op)
{
	op(m_Locals[i].rotation);
	m_Children[i]->SetRotation(m_Parent->rotation + m_Locals[i].rotation);
}

void MultiLocalTransformer2D::SetLocalScale(size_t i, const glm::vec2& sc)
{
	m_Locals[i].scale = sc;
	m_Children[i]->SetScale(m_Parent->scale * m_Locals[i].scale);
}

void MultiLocalTransformer2D::OperateLocalScale(size_t i, const std::function<void(glm::vec2& scale)>& op)
{
	op(m_Locals[i].scale);
	m_Children[i]->SetScale(m_Parent->scale * m_Locals[i].scale);
}

void MultiLocalTransformer2D::SyncGlobalWithLocal(size_t i)
{
	m_Children[i]->SetTransform(Transform::AbsTo(m_Locals[i], *m_Parent));
}

void MultiLocalTransformer2D::SyncLocalWithGlobal(size_t i)
{
	m_Locals[i] = Transform::RelTo(m_Children[i]->GetTransform(), *m_Parent);
}

void MultiLocalTransformer2D::SyncGlobalWithParent(size_t i)
{
	m_Children[i]->SetPosition(m_Parent->position + Transform::Rotation(m_Parent->rotation) * (m_Parent->scale * m_Locals[i].position));
	m_Children[i]->SetRotation(m_Parent->rotation + m_Locals[i].rotation);
	m_Children[i]->SetScale(m_Parent->scale * m_Locals[i].scale);
}

void MultiLocalTransformer2D::SyncGlobalWithParentPosition(size_t i)
{
	m_Children[i]->SetPosition(m_Parent->position + Transform::Rotation(m_Parent->rotation) * (m_Parent->scale * m_Locals[i].position));
}

void MultiLocalTransformer2D::SyncGlobalWithParentRotation(size_t i)
{
	m_Children[i]->SetPosition(m_Parent->position + Transform::Rotation(m_Parent->rotation) * (m_Parent->scale * m_Locals[i].position));
	m_Children[i]->SetRotation(m_Parent->rotation + m_Locals[i].rotation);
}

void MultiLocalTransformer2D::SyncGlobalWithParentScale(size_t i)
{
	m_Children[i]->SetPosition(m_Parent->position + Transform::Rotation(m_Parent->rotation) * (m_Parent->scale * m_Locals[i].position));
	m_Children[i]->SetScale(m_Parent->scale * m_Locals[i].scale);
}

void MultiLocalTransformer2D::SyncLocalWithParent(size_t i)
{
	m_Locals[i].position = (Transform::Rotation(-m_Parent->rotation) * (m_Children[i]->GetPosition() - m_Parent->position)) / m_Parent->scale;
	m_Locals[i].rotation = m_Children[i]->GetRotation() - m_Parent->rotation;
	m_Locals[i].scale = m_Children[i]->GetScale() / m_Parent->scale;
}

void MultiLocalTransformer2D::SyncLocalWithParentPosition(size_t i)
{
	m_Locals[i].position = (Transform::Rotation(-m_Parent->rotation) * (m_Children[i]->GetPosition() - m_Parent->position)) / m_Parent->scale;
}

void MultiLocalTransformer2D::SyncLocalWithParentRotation(size_t i)
{
	m_Locals[i].position = (Transform::Rotation(-m_Parent->rotation) * (m_Children[i]->GetPosition() - m_Parent->position)) / m_Parent->scale;
	m_Locals[i].rotation = m_Children[i]->GetRotation() - m_Parent->rotation;
}

void MultiLocalTransformer2D::SyncLocalWithParentScale(size_t i)
{
	m_Locals[i].position = (Transform::Rotation(-m_Parent->rotation) * (m_Children[i]->GetPosition() - m_Parent->position)) / m_Parent->scale;
	m_Locals[i].scale = m_Children[i]->GetScale() / m_Parent->scale;
}

std::vector<Transform2D> MultiLocalTransformer2D::GetGlobalTransforms() const
{
	std::vector<Transform2D> transforms;
	for (size_t i = 0; i < m_Children.size(); i++)
		transforms.push_back(m_Children[i]->GetTransform());
	return transforms;
}

std::vector<glm::vec2> MultiLocalTransformer2D::GetGlobalPositions() const
{
	std::vector<glm::vec2> positions;
	for (size_t i = 0; i < m_Children.size(); i++)
		positions.push_back(m_Children[i]->GetPosition());
	return positions;
}

std::vector<glm::float32> MultiLocalTransformer2D::GetGlobalRotations() const
{
	std::vector<glm::float32> rotations;
	for (size_t i = 0; i < m_Children.size(); i++)
		rotations.push_back(m_Children[i]->GetRotation());
	return rotations;
}

std::vector<glm::vec2> MultiLocalTransformer2D::GetGlobalScales() const
{
	std::vector<glm::vec2> scales;
	for (size_t i = 0; i < m_Children.size(); i++)
		scales.push_back(m_Children[i]->GetScale());
	return scales;
}

void MultiLocalTransformer2D::SetGlobalTransform(size_t i, const Transform2D& tr)
{
	m_Children[i]->SetTransform(tr);
	SyncLocalWithGlobal(i);
}

void MultiLocalTransformer2D::SetGlobalPosition(size_t i, const glm::vec2& pos)
{
	m_Children[i]->SetPosition(pos);
	m_Locals[i].position = (Transform::Rotation(-m_Parent->rotation) * (m_Children[i]->GetPosition() - m_Parent->position)) / m_Parent->scale;
}

void MultiLocalTransformer2D::OperateGlobalPosition(size_t i, const std::function<void(glm::vec2& position)>& op)
{
	m_Children[i]->OperatePosition(op);
	m_Locals[i].position = (Transform::Rotation(-m_Parent->rotation) * (m_Children[i]->GetPosition() - m_Parent->position)) / m_Parent->scale;
}

void MultiLocalTransformer2D::SetGlobalRotation(size_t i, const glm::float32& rot)
{
	m_Children[i]->SetRotation(rot);
	m_Locals[i].rotation = m_Children[i]->GetRotation() - m_Parent->rotation;
}

void MultiLocalTransformer2D::OperateGlobalRotation(size_t i, const std::function<void(glm::float32& rotation)>& op)
{
	m_Children[i]->OperateRotation(op);
	m_Locals[i].rotation = m_Children[i]->GetRotation() - m_Parent->rotation;
}

void MultiLocalTransformer2D::SetGlobalScale(size_t i, const glm::vec2& sc)
{
	m_Children[i]->SetScale(sc);
	m_Locals[i].scale = m_Children[i]->GetScale() / m_Parent->scale;
}

void MultiLocalTransformer2D::OperateGlobalScale(size_t i, const std::function<void(glm::vec2& scale)>& op)
{
	m_Children[i]->OperateScale(op);
	m_Locals[i].scale = m_Children[i]->GetScale() / m_Parent->scale;
}

void MultiLocalTransformer2D::PushBackGlobal(const std::shared_ptr<Transformable2D>& child, bool discard_old_transform)
{
	m_Children.push_back(child);
	m_Locals.push_back({});
	if (discard_old_transform)
		SyncGlobalWithLocals();
	else
		SyncLocalWithGlobals();
}

void MultiLocalTransformer2D::PushBackLocal(const Transform2D& local, bool discard_old_transform)
{
	m_Children.push_back(std::make_shared<Transformable2D>(Transformable2D(Transform2D{})));
	m_Locals.push_back(local);
	if (discard_old_transform)
		SyncLocalWithGlobals();
	else
		SyncGlobalWithLocals();
}

void MultiLocalTransformer2D::PushBackGlobals(const std::vector<std::shared_ptr<Transformable2D>> &children, bool discard_old_transform)
{
	for (const auto& child : children)
		PushBackGlobal(child, discard_old_transform);
}

void MultiLocalTransformer2D::PushBackLocals(const std::vector<Transform2D>& locals, bool discard_old_transform)
{
	for (const auto& local : locals)
		PushBackLocal(local, discard_old_transform);
}

void MultiLocalTransformer2D::Remove(size_t i)
{
	m_Children.erase(m_Children.begin() + i);
	m_Locals.erase(m_Locals.begin() + i);
}

void MultiLocalTransformer2D::Remove(const std::vector<std::shared_ptr<Transformable2D>>::iterator& where)
{
	m_Children.erase(where);
	m_Locals.erase(m_Locals.begin() + (where - m_Children.begin()));
}

std::vector<std::shared_ptr<Transformable2D>>::iterator MultiLocalTransformer2D::Find(const std::shared_ptr<Transformable2D>& child)
{
	return std::find(m_Children.begin(), m_Children.end(), child);
}
