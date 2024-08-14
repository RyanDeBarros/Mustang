#include "MultiTransformer.h"

MultiTransformer2D::MultiTransformer2D(const std::weak_ptr<Transformable2D>& parent)
	: m_Parent(parent)
{
}

MultiTransformer2D::MultiTransformer2D(std::weak_ptr<Transformable2D>&& parent)
	: m_Parent(std::move(parent))
{
}

MultiTransformer2D::MultiTransformer2D(const std::weak_ptr<Transformable2D>& parent, const std::vector<std::weak_ptr<Transformable2D>>& children, bool discard_old_transforms)
	: m_Parent(parent), m_Children(children)
{
	for (size_t i = 0; i < m_Children.size(); i++)
		m_Locals.push_back({});
	if (discard_old_transforms)
		SyncGlobalWithLocals();
	else
		SyncLocalWithGlobals();
}

MultiTransformer2D::MultiTransformer2D(const std::weak_ptr<Transformable2D>& parent, std::vector<std::weak_ptr<Transformable2D>>&& children, bool discard_old_transforms)
	: m_Parent(parent), m_Children(std::move(children))
{
	for (size_t i = 0; i < m_Children.size(); i++)
		m_Locals.push_back({});
	if (discard_old_transforms)
		SyncGlobalWithLocals();
	else
		SyncLocalWithGlobals();
}

MultiTransformer2D::MultiTransformer2D(std::weak_ptr<Transformable2D>&& parent, const std::vector<std::weak_ptr<Transformable2D>>& children, bool discard_old_transforms)
	: m_Parent(std::move(parent)), m_Children(children)
{
	for (size_t i = 0; i < m_Children.size(); i++)
		m_Locals.push_back({});
	if (discard_old_transforms)
		SyncGlobalWithLocals();
	else
		SyncLocalWithGlobals();
}

MultiTransformer2D::MultiTransformer2D(std::weak_ptr<Transformable2D>&& parent, std::vector<std::weak_ptr<Transformable2D>>&& children, bool discard_old_transforms)
	: m_Parent(std::move(parent)), m_Children(std::move(children))
{
	for (size_t i = 0; i < m_Children.size(); i++)
		m_Locals.push_back({});
	if (discard_old_transforms)
		SyncGlobalWithLocals();
	else
		SyncLocalWithGlobals();
}

MultiTransformer2D::MultiTransformer2D(const std::weak_ptr<Transformable2D>& parent, const std::vector<Transform2D>& locals, bool discard_old_transforms)
	: m_Parent(parent), m_Locals(locals)
{
	for (size_t i = 0; i < m_Children.size(); i++)
		m_Children.push_back(std::make_shared<TransformableProxy2D>());
	if (discard_old_transforms)
		SyncLocalWithGlobals();
	else
		SyncGlobalWithLocals();
}

MultiTransformer2D::MultiTransformer2D(const std::weak_ptr<Transformable2D>& parent, std::vector<Transform2D>&& locals, bool discard_old_transforms)
	: m_Parent(parent), m_Locals(std::move(locals))
{
	for (size_t i = 0; i < m_Children.size(); i++)
		m_Children.push_back(std::make_shared<TransformableProxy2D>());
	if (discard_old_transforms)
		SyncLocalWithGlobals();
	else
		SyncGlobalWithLocals();
}

MultiTransformer2D::MultiTransformer2D(std::weak_ptr<Transformable2D>&& parent, const std::vector<Transform2D>& locals, bool discard_old_transforms)
	: m_Parent(std::move(parent)), m_Locals(locals)
{
	for (size_t i = 0; i < m_Children.size(); i++)
		m_Children.push_back(std::make_shared<TransformableProxy2D>());
	if (discard_old_transforms)
		SyncLocalWithGlobals();
	else
		SyncGlobalWithLocals();
}

MultiTransformer2D::MultiTransformer2D(std::weak_ptr<Transformable2D>&& parent, std::vector<Transform2D>&& locals, bool discard_old_transforms)
	: m_Parent(std::move(parent)), m_Locals(std::move(locals))
{
	for (size_t i = 0; i < m_Children.size(); i++)
		m_Children.push_back(std::make_shared<TransformableProxy2D>());
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

void MultiTransformer2D::OperateLocalTransform(size_t i, const std::function<void(Transform2D& transform)>& op)
{
	op(m_Locals[i]);
	SyncGlobalWithLocal(i);
}

void MultiTransformer2D::SetLocalPosition(size_t i, const glm::vec2& pos)
{
	m_Locals[i].position = pos;
	WEAK_LOCK_CHECK(m_Parent, p);
	WEAK_LOCK_CHECK(m_Children[i], c);
	c->SetPosition(p->GetPosition() + Transform::Rotation(p->GetRotation()) * (p->GetScale() * m_Locals[i].position));
}

void MultiTransformer2D::OperateLocalPosition(size_t i, const std::function<void(glm::vec2& position)>& op)
{
	op(m_Locals[i].position);
	WEAK_LOCK_CHECK(m_Parent, p);
	WEAK_LOCK_CHECK(m_Children[i], c);
	c->SetPosition(p->GetPosition() + Transform::Rotation(p->GetRotation()) * (p->GetScale() * m_Locals[i].position));
}

void MultiTransformer2D::SetLocalRotation(size_t i, const glm::float32& rot)
{
	m_Locals[i].rotation = rot;
	WEAK_LOCK_CHECK(m_Parent, p);
	WEAK_LOCK_CHECK(m_Children[i], c);
	c->SetRotation(p->GetRotation() + m_Locals[i].rotation);
}

void MultiTransformer2D::OperateLocalRotation(size_t i, const std::function<void(glm::float32& rotation)>& op)
{
	op(m_Locals[i].rotation);
	WEAK_LOCK_CHECK(m_Parent, p);
	WEAK_LOCK_CHECK(m_Children[i], c);
	c->SetRotation(p->GetRotation() + m_Locals[i].rotation);
}

void MultiTransformer2D::SetLocalScale(size_t i, const glm::vec2& sc)
{
	m_Locals[i].scale = sc;
	WEAK_LOCK_CHECK(m_Parent, p);
	WEAK_LOCK_CHECK(m_Children[i], c);
	c->SetScale(p->GetScale() * m_Locals[i].scale);
}

void MultiTransformer2D::OperateLocalScale(size_t i, const std::function<void(glm::vec2& scale)>& op)
{
	op(m_Locals[i].scale);
	WEAK_LOCK_CHECK(m_Parent, p);
	WEAK_LOCK_CHECK(m_Children[i], c);
	c->SetScale(p->GetScale() * m_Locals[i].scale);
}

void MultiTransformer2D::SyncGlobalWithLocal(size_t i)
{
	WEAK_LOCK_CHECK(m_Parent, p);
	WEAK_LOCK_CHECK(m_Children[i], c);
	c->SetTransform(Transform::AbsTo(m_Locals[i], p->GetTransform()));
}

void MultiTransformer2D::SyncGlobalWithLocalPosition(size_t i)
{
	WEAK_LOCK_CHECK(m_Parent, p);
	WEAK_LOCK_CHECK(m_Children[i], c);
	c->SetPosition(p->GetPosition() + Transform::Rotation(p->GetRotation()) * (p->GetScale() * m_Locals[i].position));
}

void MultiTransformer2D::SyncGlobalWithLocalRotation(size_t i)
{
	WEAK_LOCK_CHECK(m_Parent, p);
	WEAK_LOCK_CHECK(m_Children[i], c);
	c->SetPosition(p->GetPosition() + Transform::Rotation(p->GetRotation()) * (p->GetScale() * m_Locals[i].position));
	c->SetRotation(p->GetRotation() + m_Locals[i].rotation);
}

void MultiTransformer2D::SyncGlobalWithLocalScale(size_t i)
{
	WEAK_LOCK_CHECK(m_Parent, p);
	WEAK_LOCK_CHECK(m_Children[i], c);
	c->SetPosition(p->GetPosition() + Transform::Rotation(p->GetRotation()) * (p->GetScale() * m_Locals[i].position));
	c->SetScale(p->GetScale() * m_Locals[i].scale);
}

void MultiTransformer2D::SyncLocalWithGlobal(size_t i)
{
	WEAK_LOCK_CHECK(m_Parent, p);
	WEAK_LOCK_CHECK(m_Children[i], c);
	m_Locals[i] = Transform::RelTo(c->GetTransform(), p->GetTransform());
}

void MultiTransformer2D::SyncLocalWithGlobalPosition(size_t i)
{
	WEAK_LOCK_CHECK(m_Parent, p);
	WEAK_LOCK_CHECK(m_Children[i], c);
	m_Locals[i].position = (Transform::Rotation(-p->GetRotation()) * (c->GetPosition() - p->GetPosition())) / p->GetScale();
}

void MultiTransformer2D::SyncLocalWithGlobalRotation(size_t i)
{
	WEAK_LOCK_CHECK(m_Parent, p);
	WEAK_LOCK_CHECK(m_Children[i], c);
	m_Locals[i].position = (Transform::Rotation(-p->GetRotation()) * (c->GetPosition() - p->GetPosition())) / p->GetScale();
	m_Locals[i].rotation = c->GetRotation() - p->GetRotation();
}

void MultiTransformer2D::SyncLocalWithGlobalScale(size_t i)
{
	WEAK_LOCK_CHECK(m_Parent, p);
	WEAK_LOCK_CHECK(m_Children[i], c);
	m_Locals[i].position = (Transform::Rotation(-p->GetRotation()) * (c->GetPosition() - p->GetPosition())) / p->GetScale();
	m_Locals[i].scale = c->GetScale() / p->GetScale();
}

std::vector<Transform2D> MultiTransformer2D::GetGlobalTransforms() const
{
	std::vector<Transform2D> transforms;
	for (size_t i = 0; i < m_Children.size(); i++)
	{
		WEAK_LOCK_CHECK(m_Children[i], c);
		transforms.push_back(c->GetTransform());
	}
	return transforms;
}

std::vector<glm::vec2> MultiTransformer2D::GetGlobalPositions() const
{
	std::vector<glm::vec2> positions;
	for (size_t i = 0; i < m_Children.size(); i++)
	{
		WEAK_LOCK_CHECK(m_Children[i], c);
		positions.push_back(c->GetPosition());
	}
	return positions;
}

std::vector<glm::float32> MultiTransformer2D::GetGlobalRotations() const
{
	std::vector<glm::float32> rotations;
	for (size_t i = 0; i < m_Children.size(); i++)
	{
		WEAK_LOCK_CHECK(m_Children[i], c);
		rotations.push_back(c->GetRotation());
	}
	return rotations;
}

std::vector<glm::vec2> MultiTransformer2D::GetGlobalScales() const
{
	std::vector<glm::vec2> scales;
	for (size_t i = 0; i < m_Children.size(); i++)
	{
		WEAK_LOCK_CHECK(m_Children[i], c);
		scales.push_back(c->GetScale());
	}
	return scales;
}

void MultiTransformer2D::SetGlobalTransform(size_t i, const Transform2D& tr)
{
	WEAK_LOCK_CHECK(m_Children[i], c);
	c->SetTransform(tr);
	SyncLocalWithGlobal(i);
}

void MultiTransformer2D::OperateGlobalTransform(size_t i, const std::function<void(Transform2D& transform)>& op)
{
	WEAK_LOCK_CHECK(m_Children[i], c);
	c->OperateTransform(op);
	SyncLocalWithGlobal(i);
}

void MultiTransformer2D::SetGlobalPosition(size_t i, const glm::vec2& pos)
{
	WEAK_LOCK_CHECK(m_Parent, p);
	WEAK_LOCK_CHECK(m_Children[i], c);
	c->SetPosition(pos);
	m_Locals[i].position = (Transform::Rotation(-p->GetRotation()) * (c->GetPosition() - p->GetPosition())) / p->GetScale();
}

void MultiTransformer2D::OperateGlobalPosition(size_t i, const std::function<void(glm::vec2& position)>& op)
{
	WEAK_LOCK_CHECK(m_Parent, p);
	WEAK_LOCK_CHECK(m_Children[i], c);
	c->OperatePosition(op);
	m_Locals[i].position = (Transform::Rotation(-p->GetRotation()) * (c->GetPosition() - p->GetPosition())) / p->GetScale();
}

void MultiTransformer2D::SetGlobalRotation(size_t i, const glm::float32& rot)
{
	WEAK_LOCK_CHECK(m_Parent, p);
	WEAK_LOCK_CHECK(m_Children[i], c);
	c->SetRotation(rot);
	m_Locals[i].rotation = c->GetRotation() - p->GetRotation();
}

void MultiTransformer2D::OperateGlobalRotation(size_t i, const std::function<void(glm::float32& rotation)>& op)
{
	WEAK_LOCK_CHECK(m_Parent, p);
	WEAK_LOCK_CHECK(m_Children[i], c);
	c->OperateRotation(op);
	m_Locals[i].rotation = c->GetRotation() - p->GetRotation();
}

void MultiTransformer2D::SetGlobalScale(size_t i, const glm::vec2& sc)
{
	WEAK_LOCK_CHECK(m_Parent, p);
	WEAK_LOCK_CHECK(m_Children[i], c);
	c->SetScale(sc);
	m_Locals[i].scale = c->GetScale() / p->GetScale();
}

void MultiTransformer2D::OperateGlobalScale(size_t i, const std::function<void(glm::vec2& scale)>& op)
{
	WEAK_LOCK_CHECK(m_Parent, p);
	WEAK_LOCK_CHECK(m_Children[i], c);
	c->OperateScale(op);
	m_Locals[i].scale = c->GetScale() / p->GetScale();
}

void MultiTransformer2D::PushBackGlobal(const std::weak_ptr<Transformable2D>& child, bool discard_old_transform)
{
	m_Children.push_back(child);
	m_Locals.push_back({});
	if (discard_old_transform)
		SyncGlobalWithLocal(m_Locals.size() - 1);
	else
		SyncLocalWithGlobal(m_Locals.size() - 1);
}

void MultiTransformer2D::PushBackGlobal(std::weak_ptr<Transformable2D>&& child, bool discard_old_transform)
{
	m_Children.push_back(std::move(child));
	m_Locals.push_back({});
	if (discard_old_transform)
		SyncGlobalWithLocal(m_Locals.size() - 1);
	else
		SyncLocalWithGlobal(m_Locals.size() - 1);
}

void MultiTransformer2D::PushBackGlobals(const std::vector<std::weak_ptr<Transformable2D>>& children, bool discard_old_transform)
{
	for (const auto& child : children)
		PushBackGlobal(child, discard_old_transform);
}

void MultiTransformer2D::PushBackGlobals(std::vector<std::weak_ptr<Transformable2D>>&& children, bool discard_old_transform)
{
	for (auto& child : children)
		PushBackGlobal(std::move(child), discard_old_transform);
}

std::shared_ptr<TransformableProxy2D> MultiTransformer2D::PushBackLocal(const Transform2D& local, bool discard_old_transform)
{
	std::shared_ptr<TransformableProxy2D> child(std::make_shared<TransformableProxy2D>());
	m_Children.push_back(child);
	m_Locals.push_back(local);
	if (discard_old_transform)
		SyncLocalWithGlobal(m_Locals.size() - 1);
	else
		SyncGlobalWithLocal(m_Locals.size() - 1);
	return child;
}

std::vector<std::shared_ptr<TransformableProxy2D>> MultiTransformer2D::PushBackLocals(const std::vector<Transform2D>& locals, bool discard_old_transform)
{
	std::vector<std::shared_ptr<TransformableProxy2D>> children;
	for (const auto& local : locals)
		children.push_back(PushBackLocal(local, discard_old_transform));
	return children;
}

void MultiTransformer2D::Remove(size_t i)
{
	// TODO use swap/pop instead? maybe define a swap/pop utility function to use throughout project?
	m_Children.erase(m_Children.begin() + i);
	m_Locals.erase(m_Locals.begin() + i);
}

void MultiTransformer2D::Remove(const std::vector<std::weak_ptr<Transformable2D>>::iterator& where)
{
	m_Children.erase(where);
	m_Locals.erase(m_Locals.begin() + (where - m_Children.begin()));
}

void MultiTransformer2D::SwapPop(size_t i)
{
	if (m_Children.size() > 1)
	{
		std::swap(m_Children[i], m_Children.back());
		std::swap(m_Locals[i], m_Locals.back());
	}
	m_Children.pop_back();
	m_Locals.pop_back();
}

std::vector<std::weak_ptr<Transformable2D>>::iterator MultiTransformer2D::Find(const std::weak_ptr<Transformable2D>& child)
{
	return std::find(m_Children.begin(), m_Children.end(), child);
}
