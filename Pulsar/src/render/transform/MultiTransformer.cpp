#include "MultiTransformer.h"

MultiTransformer2D::MultiTransformer2D(const std::shared_ptr<Transform2D>& parent)
	: m_Parent(parent)
{
}

MultiTransformer2D::MultiTransformer2D(std::shared_ptr<Transform2D>&& parent)
	: m_Parent(std::move(parent))
{
}

MultiTransformer2D::MultiTransformer2D(const std::shared_ptr<Transform2D>& parent, const std::vector<std::shared_ptr<Transformable2D>>& children, bool discard_old_transforms)
	: m_Parent(parent), m_Children(children)
{
	for (size_t i = 0; i < m_Children.size(); i++)
		m_Locals.push_back({});
	if (discard_old_transforms)
		SyncGlobalWithLocals();
	else
		SyncLocalWithGlobals();
}

MultiTransformer2D::MultiTransformer2D(std::shared_ptr<Transform2D>&& parent, std::vector<std::shared_ptr<Transformable2D>>&& children, bool discard_old_transforms)
	: m_Parent(std::move(parent)), m_Children(std::move(children))
{
	for (size_t i = 0; i < m_Children.size(); i++)
		m_Locals.push_back({});
	if (discard_old_transforms)
		SyncGlobalWithLocals();
	else
		SyncLocalWithGlobals();
}

MultiTransformer2D::MultiTransformer2D(const std::shared_ptr<Transform2D>& parent, const std::vector<Transform2D>& locals, bool discard_old_transforms)
	: m_Parent(parent), m_Locals(locals)
{
	for (size_t i = 0; i < m_Children.size(); i++)
		m_Children.push_back(std::make_shared<Transformable2D>());
	if (discard_old_transforms)
		SyncLocalWithGlobals();
	else
		SyncGlobalWithLocals();
}

MultiTransformer2D::MultiTransformer2D(const MultiTransformer2D& other)
	: m_Parent(other.m_Parent), m_Children(other.m_Children), m_Locals(other.m_Locals)
{
}

MultiTransformer2D::MultiTransformer2D(MultiTransformer2D&& other) noexcept
	: m_Parent(std::move(other.m_Parent)), m_Children(std::move(other.m_Children)), m_Locals(std::move(other.m_Locals))
{
}

MultiTransformer2D& MultiTransformer2D::operator=(const MultiTransformer2D& other)
{
	m_Parent = other.m_Parent;
	m_Children = other.m_Children;
	m_Locals = other.m_Locals;
	return *this;
}

MultiTransformer2D& MultiTransformer2D::operator=(MultiTransformer2D&& other) noexcept
{
	m_Parent = std::move(other.m_Parent);
	m_Children = std::move(other.m_Children);
	m_Locals = std::move(other.m_Locals);
	return *this;
}

std::vector<glm::vec2> MultiTransformer2D::GetLocalPositions() const
{
	std::vector<glm::vec2> positions;
	for (size_t i = 0; i < m_Locals.size(); i++)
		positions.push_back(GetLocalPosition(i));
	return positions;
}

std::vector<glm::float32> MultiTransformer2D::GetLocalRotations() const
{
	std::vector<glm::float32> rotations;
	for (size_t i = 0; i < m_Locals.size(); i++)
		rotations.push_back(GetLocalRotation(i));
	return rotations;
}

std::vector<glm::vec2> MultiTransformer2D::GetLocalScales() const
{
	std::vector<glm::vec2> scales;
	for (size_t i = 0; i < m_Locals.size(); i++)
		scales.push_back(GetLocalScale(i));
	return scales;
}

void MultiTransformer2D::SetLocalTransform(size_t i, const Transform2D& tr)
{
	m_Locals[i] = tr;
	SyncGlobalWithLocal(i);
}

void MultiTransformer2D::SetLocalPosition(size_t i, const glm::vec2& pos)
{
	m_Locals[i].position = pos;
	m_Children[i]->SetPosition(m_Parent->position + Transform::Rotation(m_Parent->rotation) * (m_Parent->scale * m_Locals[i].position));
}

void MultiTransformer2D::OperateLocalPosition(size_t i, const std::function<void(glm::vec2& position)>& op)
{
	op(m_Locals[i].position);
	m_Children[i]->SetPosition(m_Parent->position + Transform::Rotation(m_Parent->rotation) * (m_Parent->scale * m_Locals[i].position));
}

void MultiTransformer2D::SetLocalRotation(size_t i, const glm::float32& rot)
{
	m_Locals[i].rotation = rot;
	m_Children[i]->SetRotation(m_Parent->rotation + m_Locals[i].rotation);
}

void MultiTransformer2D::OperateLocalRotation(size_t i, const std::function<void(glm::float32& rotation)>& op)
{
	op(m_Locals[i].rotation);
	m_Children[i]->SetRotation(m_Parent->rotation + m_Locals[i].rotation);
}

void MultiTransformer2D::SetLocalScale(size_t i, const glm::vec2& sc)
{
	m_Locals[i].scale = sc;
	m_Children[i]->SetScale(m_Parent->scale * m_Locals[i].scale);
}

void MultiTransformer2D::OperateLocalScale(size_t i, const std::function<void(glm::vec2& scale)>& op)
{
	op(m_Locals[i].scale);
	m_Children[i]->SetScale(m_Parent->scale * m_Locals[i].scale);
}

void MultiTransformer2D::SyncGlobalWithLocal(size_t i)
{
	m_Children[i]->SetTransform(Transform::AbsTo(m_Locals[i], *m_Parent));
}

void MultiTransformer2D::SyncLocalWithGlobal(size_t i)
{
	m_Locals[i] = Transform::RelTo(m_Children[i]->GetTransform(), *m_Parent);
}

void MultiTransformer2D::SyncGlobalWithParent(size_t i)
{
	m_Children[i]->SetPosition(m_Parent->position + Transform::Rotation(m_Parent->rotation) * (m_Parent->scale * m_Locals[i].position));
	m_Children[i]->SetRotation(m_Parent->rotation + m_Locals[i].rotation);
	m_Children[i]->SetScale(m_Parent->scale * m_Locals[i].scale);
}

void MultiTransformer2D::SyncGlobalWithParentPosition(size_t i)
{
	m_Children[i]->SetPosition(m_Parent->position + Transform::Rotation(m_Parent->rotation) * (m_Parent->scale * m_Locals[i].position));
}

void MultiTransformer2D::SyncGlobalWithParentRotation(size_t i)
{
	m_Children[i]->SetPosition(m_Parent->position + Transform::Rotation(m_Parent->rotation) * (m_Parent->scale * m_Locals[i].position));
	m_Children[i]->SetRotation(m_Parent->rotation + m_Locals[i].rotation);
}

void MultiTransformer2D::SyncGlobalWithParentScale(size_t i)
{
	m_Children[i]->SetPosition(m_Parent->position + Transform::Rotation(m_Parent->rotation) * (m_Parent->scale * m_Locals[i].position));
	m_Children[i]->SetScale(m_Parent->scale * m_Locals[i].scale);
}

void MultiTransformer2D::SyncLocalWithParent(size_t i)
{
	m_Locals[i].position = (Transform::Rotation(-m_Parent->rotation) * (m_Children[i]->GetPosition() - m_Parent->position)) / m_Parent->scale;
	m_Locals[i].rotation = m_Children[i]->GetRotation() - m_Parent->rotation;
	m_Locals[i].scale = m_Children[i]->GetScale() / m_Parent->scale;
}

void MultiTransformer2D::SyncLocalWithParentPosition(size_t i)
{
	m_Locals[i].position = (Transform::Rotation(-m_Parent->rotation) * (m_Children[i]->GetPosition() - m_Parent->position)) / m_Parent->scale;
}

void MultiTransformer2D::SyncLocalWithParentRotation(size_t i)
{
	m_Locals[i].position = (Transform::Rotation(-m_Parent->rotation) * (m_Children[i]->GetPosition() - m_Parent->position)) / m_Parent->scale;
	m_Locals[i].rotation = m_Children[i]->GetRotation() - m_Parent->rotation;
}

void MultiTransformer2D::SyncLocalWithParentScale(size_t i)
{
	m_Locals[i].position = (Transform::Rotation(-m_Parent->rotation) * (m_Children[i]->GetPosition() - m_Parent->position)) / m_Parent->scale;
	m_Locals[i].scale = m_Children[i]->GetScale() / m_Parent->scale;
}

std::vector<Transform2D> MultiTransformer2D::GetGlobalTransforms() const
{
	std::vector<Transform2D> transforms;
	for (size_t i = 0; i < m_Children.size(); i++)
		transforms.push_back(m_Children[i]->GetTransform());
	return transforms;
}

std::vector<glm::vec2> MultiTransformer2D::GetGlobalPositions() const
{
	std::vector<glm::vec2> positions;
	for (size_t i = 0; i < m_Children.size(); i++)
		positions.push_back(m_Children[i]->GetPosition());
	return positions;
}

std::vector<glm::float32> MultiTransformer2D::GetGlobalRotations() const
{
	std::vector<glm::float32> rotations;
	for (size_t i = 0; i < m_Children.size(); i++)
		rotations.push_back(m_Children[i]->GetRotation());
	return rotations;
}

std::vector<glm::vec2> MultiTransformer2D::GetGlobalScales() const
{
	std::vector<glm::vec2> scales;
	for (size_t i = 0; i < m_Children.size(); i++)
		scales.push_back(m_Children[i]->GetScale());
	return scales;
}

void MultiTransformer2D::SetGlobalTransform(size_t i, const Transform2D& tr)
{
	m_Children[i]->SetTransform(tr);
	SyncLocalWithGlobal(i);
}

void MultiTransformer2D::SetGlobalPosition(size_t i, const glm::vec2& pos)
{
	m_Children[i]->SetPosition(pos);
	m_Locals[i].position = (Transform::Rotation(-m_Parent->rotation) * (m_Children[i]->GetPosition() - m_Parent->position)) / m_Parent->scale;
}

void MultiTransformer2D::OperateGlobalPosition(size_t i, const std::function<void(glm::vec2& position)>& op)
{
	m_Children[i]->OperatePosition(op);
	m_Locals[i].position = (Transform::Rotation(-m_Parent->rotation) * (m_Children[i]->GetPosition() - m_Parent->position)) / m_Parent->scale;
}

void MultiTransformer2D::SetGlobalRotation(size_t i, const glm::float32& rot)
{
	m_Children[i]->SetRotation(rot);
	m_Locals[i].rotation = m_Children[i]->GetRotation() - m_Parent->rotation;
}

void MultiTransformer2D::OperateGlobalRotation(size_t i, const std::function<void(glm::float32& rotation)>& op)
{
	m_Children[i]->OperateRotation(op);
	m_Locals[i].rotation = m_Children[i]->GetRotation() - m_Parent->rotation;
}

void MultiTransformer2D::SetGlobalScale(size_t i, const glm::vec2& sc)
{
	m_Children[i]->SetScale(sc);
	m_Locals[i].scale = m_Children[i]->GetScale() / m_Parent->scale;
}

void MultiTransformer2D::OperateGlobalScale(size_t i, const std::function<void(glm::vec2& scale)>& op)
{
	m_Children[i]->OperateScale(op);
	m_Locals[i].scale = m_Children[i]->GetScale() / m_Parent->scale;
}

void MultiTransformer2D::PushBackGlobal(const std::shared_ptr<Transformable2D>& child, bool discard_old_transform)
{
	m_Children.push_back(child);
	m_Locals.push_back({});
	if (discard_old_transform)
		SyncGlobalWithLocals();
	else
		SyncLocalWithGlobals();
}

void MultiTransformer2D::PushBackGlobal(std::shared_ptr<Transformable2D>&& child, bool discard_old_transform)
{
	m_Children.push_back(std::move(child));
	m_Locals.push_back({});
	if (discard_old_transform)
		SyncGlobalWithLocals();
	else
		SyncLocalWithGlobals();
}

void MultiTransformer2D::PushBackLocal(const Transform2D& local, bool discard_old_transform)
{
	m_Children.push_back(std::make_shared<Transformable2D>());
	m_Locals.push_back(local);
	if (discard_old_transform)
		SyncLocalWithGlobals();
	else
		SyncGlobalWithLocals();
}

void MultiTransformer2D::PushBackGlobals(const std::vector<std::shared_ptr<Transformable2D>>& children, bool discard_old_transform)
{
	for (const auto& child : children)
		PushBackGlobal(child, discard_old_transform);
}

void MultiTransformer2D::PushBackGlobals(std::vector<std::shared_ptr<Transformable2D>>&& children, bool discard_old_transform)
{
	for (auto&& child : children)
		PushBackGlobal(std::move(child), discard_old_transform);
}

void MultiTransformer2D::PushBackLocals(const std::vector<Transform2D>& locals, bool discard_old_transform)
{
	for (const auto& local : locals)
		PushBackLocal(local, discard_old_transform);
}

void MultiTransformer2D::Remove(size_t i)
{
	m_Children.erase(m_Children.begin() + i);
	m_Locals.erase(m_Locals.begin() + i);
}

void MultiTransformer2D::Remove(const std::vector<std::shared_ptr<Transformable2D>>::iterator& where)
{
	m_Children.erase(where);
	m_Locals.erase(m_Locals.begin() + (where - m_Children.begin()));
}

std::vector<std::shared_ptr<Transformable2D>>::iterator MultiTransformer2D::Find(const std::shared_ptr<Transformable2D>& child)
{
	return std::find(m_Children.begin(), m_Children.end(), child);
}
