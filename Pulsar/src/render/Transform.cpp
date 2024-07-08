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

}

Transform2D Transform2D::operator^(const Transform2D& transform) const
{
	return { position + transform.position, rotation + transform.rotation, scale * transform.scale };
}

Transform2D Transform2D::RelTo(const Transform2D& global, const Transform2D& parent)
{
	return { (Transform::Rotation(-parent.rotation) * (global.position - parent.position)) / parent.scale, global.rotation - parent.rotation, global.scale / parent.scale };
}

Transform2D Transform2D::AbsTo(const Transform2D& local, const Transform2D& parent)
{
	return { parent.position + Transform::Rotation(parent.rotation) * (parent.scale * local.position), parent.rotation + local.rotation, parent.scale * local.scale };
}

LocalTransformer2D::LocalTransformer2D(Transform2D* const parent, Transformable2D* const child)
	: m_Parent(parent), m_Child(child)
{
	SyncLocalWithGlobal();
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
	m_Child->SetTransform(Transform2D::AbsTo(m_Local, *m_Parent));
}

void LocalTransformer2D::SyncLocalWithGlobal()
{
	m_Local = Transform2D::RelTo(m_Child->GetTransform(), *m_Parent);
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
