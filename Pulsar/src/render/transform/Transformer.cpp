#include "Transformer.h"

Transformer2D::Transformer2D(const std::shared_ptr<Transform2D>& parent, const std::shared_ptr<Transformable2D>& child, bool discard_old_transform)
	: m_Parent(parent), m_Child(child)
{
	if (discard_old_transform)
		SyncGlobalWithLocal();
	else
		SyncLocalWithGlobal();
}

Transformer2D::Transformer2D(const std::shared_ptr<Transform2D>& parent, const Transform2D& local, bool discard_old_transform)
	: m_Parent(parent), m_Local(local)
{
	m_Child = std::make_shared<Transformable2D>(Transformable2D(Transform2D{}));
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
	: m_Parent(other.m_Parent), m_Child(other.m_Child), m_Local(other.m_Local)
{
}

Transformer2D& Transformer2D::operator=(const Transformer2D& other)
{
	m_Parent = other.m_Parent;
	m_Child = other.m_Child;
	m_Local = other.m_Local;
	return *this;
}

void Transformer2D::SetLocalTransform(const Transform2D& tr)
{
	m_Local = tr;
	SyncGlobalWithLocal();
}

void Transformer2D::SetLocalPosition(const glm::vec2& pos)
{
	m_Local.position = pos;
	m_Child->SetPosition(m_Parent->position + Transform::Rotation(m_Parent->rotation) * (m_Parent->scale * m_Local.position));
}

void Transformer2D::OperateLocalPosition(const std::function<void(glm::vec2& position)>& op)
{
	op(m_Local.position);
	m_Child->SetPosition(m_Parent->position + Transform::Rotation(m_Parent->rotation) * (m_Parent->scale * m_Local.position));
}

void Transformer2D::SetLocalRotation(const glm::float32& rot)
{
	m_Local.rotation = rot;
	m_Child->SetRotation(m_Parent->rotation + m_Local.rotation);
}

void Transformer2D::OperateLocalRotation(const std::function<void(glm::float32& rotation)>& op)
{
	op(m_Local.rotation);
	m_Child->SetRotation(m_Parent->rotation + m_Local.rotation);
}

void Transformer2D::SetLocalScale(const glm::vec2& sc)
{
	m_Local.scale = sc;
	m_Child->SetScale(m_Parent->scale * m_Local.scale);
}

void Transformer2D::OperateLocalScale(const std::function<void(glm::vec2& scale)>& op)
{
	op(m_Local.scale);
	m_Child->SetScale(m_Parent->scale * m_Local.scale);
}

void Transformer2D::SyncGlobalWithLocal()
{
	m_Child->SetTransform(Transform::AbsTo(m_Local, *m_Parent));
}

void Transformer2D::SyncLocalWithGlobal()
{
	m_Local = Transform::RelTo(m_Child->GetTransform(), *m_Parent);
}

void Transformer2D::SyncGlobalWithParent()
{
	m_Child->SetPosition(m_Parent->position + Transform::Rotation(m_Parent->rotation) * (m_Parent->scale * m_Local.position));
	m_Child->SetRotation(m_Parent->rotation + m_Local.rotation);
	m_Child->SetScale(m_Parent->scale * m_Local.scale);
}

void Transformer2D::SyncGlobalWithParentPosition()
{
	m_Child->SetPosition(m_Parent->position + Transform::Rotation(m_Parent->rotation) * (m_Parent->scale * m_Local.position));
}

void Transformer2D::SyncGlobalWithParentRotation()
{
	m_Child->SetPosition(m_Parent->position + Transform::Rotation(m_Parent->rotation) * (m_Parent->scale * m_Local.position));
	m_Child->SetRotation(m_Parent->rotation + m_Local.rotation);
}

void Transformer2D::SyncGlobalWithParentScale()
{
	m_Child->SetPosition(m_Parent->position + Transform::Rotation(m_Parent->rotation) * (m_Parent->scale * m_Local.position));
	m_Child->SetScale(m_Parent->scale * m_Local.scale);
}

void Transformer2D::SyncLocalWithParent()
{
	m_Local.position = (Transform::Rotation(-m_Parent->rotation) * (m_Child->GetPosition() - m_Parent->position)) / m_Parent->scale;
	m_Local.rotation = m_Child->GetRotation() - m_Parent->rotation;
	m_Local.scale = m_Child->GetScale() / m_Parent->scale;
}

void Transformer2D::SyncLocalWithParentPosition()
{
	m_Local.position = (Transform::Rotation(-m_Parent->rotation) * (m_Child->GetPosition() - m_Parent->position)) / m_Parent->scale;
}

void Transformer2D::SyncLocalWithParentRotation()
{
	m_Local.position = (Transform::Rotation(-m_Parent->rotation) * (m_Child->GetPosition() - m_Parent->position)) / m_Parent->scale;
	m_Local.rotation = m_Child->GetRotation() - m_Parent->rotation;
}

void Transformer2D::SyncLocalWithParentScale()
{
	m_Local.position = (Transform::Rotation(-m_Parent->rotation) * (m_Child->GetPosition() - m_Parent->position)) / m_Parent->scale;
	m_Local.scale = m_Child->GetScale() / m_Parent->scale;
}

void Transformer2D::SetGlobalTransform(const Transform2D& tr)
{
	m_Child->SetTransform(tr);
	SyncLocalWithGlobal();
}

void Transformer2D::SetGlobalPosition(const glm::vec2& pos)
{
	m_Child->SetPosition(pos);
	m_Local.position = (Transform::Rotation(-m_Parent->rotation) * (m_Child->GetPosition() - m_Parent->position)) / m_Parent->scale;
}

void Transformer2D::OperateGlobalPosition(const std::function<void(glm::vec2& position)>& op)
{
	m_Child->OperatePosition(op);
	m_Local.position = (Transform::Rotation(-m_Parent->rotation) * (m_Child->GetPosition() - m_Parent->position)) / m_Parent->scale;
}

void Transformer2D::SetGlobalRotation(const glm::float32& rot)
{
	m_Child->SetRotation(rot);
	m_Local.rotation = m_Child->GetRotation() - m_Parent->rotation;
}

void Transformer2D::OperateGlobalRotation(const std::function<void(glm::float32& rotation)>& op)
{
	m_Child->OperateRotation(op);
	m_Local.rotation = m_Child->GetRotation() - m_Parent->rotation;
}

void Transformer2D::SetGlobalScale(const glm::vec2& sc)
{
	m_Child->SetScale(sc);
	m_Local.scale = m_Child->GetScale() / m_Parent->scale;
}

void Transformer2D::OperateGlobalScale(const std::function<void(glm::vec2& scale)>& op)
{
	m_Child->OperateScale(op);
	m_Local.scale = m_Child->GetScale() / m_Parent->scale;
}
