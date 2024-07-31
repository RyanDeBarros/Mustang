#include "Transformer.h"

Transformer2D::Transformer2D(const std::weak_ptr<Transformable2D>& parent, const std::weak_ptr<Transformable2D>& child, bool discard_old_transform)
	: m_Parent(parent), m_Child(child)
{
	if (discard_old_transform)
		SyncGlobalWithLocal();
	else
		SyncLocalWithGlobal();
}

Transformer2D::Transformer2D(const std::weak_ptr<Transformable2D>& parent, std::weak_ptr<Transformable2D>&& child, bool discard_old_transform)
	: m_Parent(parent), m_Child(std::move(child))
{
	if (discard_old_transform)
		SyncGlobalWithLocal();
	else
		SyncLocalWithGlobal();
}

Transformer2D::Transformer2D(std::weak_ptr<Transformable2D>&& parent, const std::weak_ptr<Transformable2D>& child, bool discard_old_transform)
	: m_Parent(std::move(parent)), m_Child(child)
{
	if (discard_old_transform)
		SyncGlobalWithLocal();
	else
		SyncLocalWithGlobal();
}

Transformer2D::Transformer2D(std::weak_ptr<Transformable2D>&& parent, std::weak_ptr<Transformable2D>&& child, bool discard_old_transform)
	: m_Parent(std::move(parent)), m_Child(std::move(child))
{
	if (discard_old_transform)
		SyncGlobalWithLocal();
	else
		SyncLocalWithGlobal();
}

Transformer2D::Transformer2D(const std::weak_ptr<Transformable2D>& parent, const Transform2D& local, bool discard_old_transform)
	: m_Parent(parent), m_Child(std::make_shared<TransformableProxy2D>()), m_Local(local)
{
	if (discard_old_transform)
		SyncLocalWithGlobal();
	else
		SyncGlobalWithLocal();
}

Transformer2D::Transformer2D(std::weak_ptr<Transformable2D>&& parent, const Transform2D& local, bool discard_old_transform)
	: m_Parent(std::move(parent)), m_Child(std::make_shared<TransformableProxy2D>()), m_Local(local)
{
	if (discard_old_transform)
		SyncLocalWithGlobal();
	else
		SyncGlobalWithLocal();
}

Transformer2D::Transformer2D(const Transformer2D& other)
	: m_Parent(other.m_Parent), m_Child(other.m_Child), m_Local(other.m_Local)
{
}

Transformer2D::Transformer2D(Transformer2D&& other) noexcept
	: m_Parent(std::move(other.m_Parent)), m_Child(std::move(other.m_Child)), m_Local(other.m_Local)
{
}

Transformer2D& Transformer2D::operator=(const Transformer2D& other)
{
	m_Parent = other.m_Parent;
	m_Child = other.m_Child;
	m_Local = other.m_Local;
	return *this;
}

Transformer2D& Transformer2D::operator=(Transformer2D&& other) noexcept
{
	m_Parent = std::move(other.m_Parent);
	m_Child = std::move(other.m_Child);
	m_Local = other.m_Local;
	return *this;
}

void Transformer2D::SetLocalTransform(const Transform2D& tr)
{
	m_Local = tr;
	SyncGlobalWithLocal();
}

void Transformer2D::OperateLocalTransform(const std::function<void(Transform2D& transform)>& op)
{
	op(m_Local);
	SyncGlobalWithLocal();
}

void Transformer2D::SetLocalPosition(const glm::vec2& pos)
{
	m_Local.position = pos;
	WEAK_LOCK_CHECK(m_Parent, p);
	WEAK_LOCK_CHECK(m_Child, c);
	c->SetPosition(p->GetPosition() + Transform::Rotation(p->GetRotation()) * (p->GetScale() * m_Local.position));
}

void Transformer2D::OperateLocalPosition(const std::function<void(glm::vec2& position)>& op)
{
	op(m_Local.position);
	WEAK_LOCK_CHECK(m_Parent, p);
	WEAK_LOCK_CHECK(m_Child, c);
	c->SetPosition(p->GetPosition() + Transform::Rotation(p->GetRotation()) * (p->GetScale() * m_Local.position));
}

void Transformer2D::SetLocalRotation(const glm::float32& rot)
{
	m_Local.rotation = rot;
	WEAK_LOCK_CHECK(m_Parent, p);
	WEAK_LOCK_CHECK(m_Child, c);
	c->SetRotation(p->GetRotation() + m_Local.rotation);
}

void Transformer2D::OperateLocalRotation(const std::function<void(glm::float32& rotation)>& op)
{
	op(m_Local.rotation);
	WEAK_LOCK_CHECK(m_Parent, p);
	WEAK_LOCK_CHECK(m_Child, c);
	c->SetRotation(p->GetRotation() + m_Local.rotation);
}

void Transformer2D::SetLocalScale(const glm::vec2& sc)
{
	m_Local.scale = sc;
	WEAK_LOCK_CHECK(m_Parent, p);
	WEAK_LOCK_CHECK(m_Child, c);
	c->SetScale(p->GetScale() * m_Local.scale);
}

void Transformer2D::OperateLocalScale(const std::function<void(glm::vec2& scale)>& op)
{
	op(m_Local.scale);
	WEAK_LOCK_CHECK(m_Parent, p);
	WEAK_LOCK_CHECK(m_Child, c);
	c->SetScale(p->GetScale() * m_Local.scale);
}

void Transformer2D::SyncGlobalWithLocal()
{
	WEAK_LOCK_CHECK(m_Parent, p);
	WEAK_LOCK_CHECK(m_Child, c);
	c->SetTransform(Transform::AbsTo(m_Local, p->GetTransform()));
}

void Transformer2D::SyncLocalWithGlobal()
{
	WEAK_LOCK_CHECK(m_Parent, p);
	WEAK_LOCK_CHECK(m_Child, c);
	m_Local = Transform::RelTo(c->GetTransform(), p->GetTransform());
}

void Transformer2D::SyncGlobalWithParent()
{
	WEAK_LOCK_CHECK(m_Parent, p);
	WEAK_LOCK_CHECK(m_Child, c);
	c->SetPosition(p->GetPosition() + Transform::Rotation(p->GetRotation()) * (p->GetScale() * m_Local.position));
	c->SetRotation(p->GetRotation() + m_Local.rotation);
	c->SetScale(p->GetScale() * m_Local.scale);
}

void Transformer2D::SyncGlobalWithParentPosition()
{
	WEAK_LOCK_CHECK(m_Parent, p);
	WEAK_LOCK_CHECK(m_Child, c);
	c->SetPosition(p->GetPosition() + Transform::Rotation(p->GetRotation()) * (p->GetScale() * m_Local.position));
}

void Transformer2D::SyncGlobalWithParentRotation()
{
	WEAK_LOCK_CHECK(m_Parent, p);
	WEAK_LOCK_CHECK(m_Child, c);
	c->SetPosition(p->GetPosition() + Transform::Rotation(p->GetRotation()) * (p->GetScale() * m_Local.position));
	c->SetRotation(p->GetRotation() + m_Local.rotation);
}

void Transformer2D::SyncGlobalWithParentScale()
{
	WEAK_LOCK_CHECK(m_Parent, p);
	WEAK_LOCK_CHECK(m_Child, c);
	c->SetPosition(p->GetPosition() + Transform::Rotation(p->GetRotation()) * (p->GetScale() * m_Local.position));
	c->SetScale(p->GetScale() * m_Local.scale);
}

void Transformer2D::SyncLocalWithParent()
{
	WEAK_LOCK_CHECK(m_Parent, p);
	WEAK_LOCK_CHECK(m_Child, c);
	m_Local.position = (Transform::Rotation(-p->GetRotation()) * (c->GetPosition() - p->GetPosition())) / p->GetScale();
	m_Local.rotation = c->GetRotation() - p->GetRotation();
	m_Local.scale = c->GetScale() / p->GetScale();
}

void Transformer2D::SyncLocalWithParentPosition()
{
	WEAK_LOCK_CHECK(m_Parent, p);
	WEAK_LOCK_CHECK(m_Child, c);
	m_Local.position = (Transform::Rotation(-p->GetRotation()) * (c->GetPosition() - p->GetPosition())) / p->GetScale();
}

void Transformer2D::SyncLocalWithParentRotation()
{
	WEAK_LOCK_CHECK(m_Parent, p);
	WEAK_LOCK_CHECK(m_Child, c);
	m_Local.position = (Transform::Rotation(-p->GetRotation()) * (c->GetPosition() - p->GetPosition())) / p->GetScale();
	m_Local.rotation = c->GetRotation() - p->GetRotation();
}

void Transformer2D::SyncLocalWithParentScale()
{
	WEAK_LOCK_CHECK(m_Parent, p);
	WEAK_LOCK_CHECK(m_Child, c);
	m_Local.position = (Transform::Rotation(-p->GetRotation()) * (c->GetPosition() - p->GetPosition())) / p->GetScale();
	m_Local.scale = c->GetScale() / p->GetScale();
}

void Transformer2D::SetGlobalTransform(const Transform2D& tr)
{
	WEAK_LOCK_CHECK(m_Child, c);
	c->SetTransform(tr);
	SyncLocalWithGlobal();
}

void Transformer2D::OperateGlobalTransform(const std::function<void(Transform2D& transform)>& op)
{
	WEAK_LOCK_CHECK(m_Child, c);
	c->OperateTransform(op);
	SyncLocalWithGlobal();
}

void Transformer2D::SetGlobalPosition(const glm::vec2& pos)
{
	WEAK_LOCK_CHECK(m_Parent, p);
	WEAK_LOCK_CHECK(m_Child, c);
	c->SetPosition(pos);
	m_Local.position = (Transform::Rotation(-p->GetRotation()) * (c->GetPosition() - p->GetPosition())) / p->GetScale();
}

void Transformer2D::OperateGlobalPosition(const std::function<void(glm::vec2& position)>& op)
{
	WEAK_LOCK_CHECK(m_Parent, p);
	WEAK_LOCK_CHECK(m_Child, c);
	c->OperatePosition(op);
	m_Local.position = (Transform::Rotation(-p->GetRotation()) * (c->GetPosition() - p->GetPosition())) / p->GetScale();
}

void Transformer2D::SetGlobalRotation(const glm::float32& rot)
{
	WEAK_LOCK_CHECK(m_Parent, p);
	WEAK_LOCK_CHECK(m_Child, c);
	c->SetRotation(rot);
	m_Local.rotation = c->GetRotation() - p->GetRotation();
}

void Transformer2D::OperateGlobalRotation(const std::function<void(glm::float32& rotation)>& op)
{
	WEAK_LOCK_CHECK(m_Parent, p);
	WEAK_LOCK_CHECK(m_Child, c);
	c->OperateRotation(op);
	m_Local.rotation = c->GetRotation() - p->GetRotation();
}

void Transformer2D::SetGlobalScale(const glm::vec2& sc)
{
	WEAK_LOCK_CHECK(m_Parent, p);
	WEAK_LOCK_CHECK(m_Child, c);
	c->SetScale(sc);
	m_Local.scale = c->GetScale() / p->GetScale();
}

void Transformer2D::OperateGlobalScale(const std::function<void(glm::vec2& scale)>& op)
{
	WEAK_LOCK_CHECK(m_Parent, p);
	WEAK_LOCK_CHECK(m_Child, c);
	c->OperateScale(op);
	m_Local.scale = c->GetScale() / p->GetScale();
}
