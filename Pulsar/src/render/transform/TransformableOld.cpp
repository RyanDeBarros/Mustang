#include "Transformable.h"

PackedTransform2D Transformable2D::GetGlobalPackedTransform()
{
	if (auto pp = GetPackedParent())
	{
		return PackedTransform2D(Transform::CondensedP(GetPosition(), *pp), Transform::CombineGamma(Transform::Gamma(GetTransform()), pp->gamma));
	}
	else
		return PackedTransform2D(GetTransform());
}

glm::vec2 Transformable2D::GetGlobalPackedTransformP()
{
	if (auto pp = GetPackedParent())
		return Transform::CondensedP(GetPosition(), *pp);
	else
		return GetPosition();
}

glm::mat4 Transformable2D::GetGlobalPackedTransformRS()
{
	if (auto pp = GetPackedParent())
		return Transform::CondensedRS(Transform::Gamma(GetTransform()), pp->gamma);
	else
		return Transform::CondensedRS(GetTransform());
}

TransformableProxy2D::TransformableProxy2D(const Transform2D& transform)
	: m_Transform(transform)
{
}

TransformableProxy2D::TransformableProxy2D(const TransformableProxy2D& other)
	: m_Transform(other.GetTransform()), m_PackedParent(other.m_PackedParent)
{
}

TransformableProxy2D::TransformableProxy2D(TransformableProxy2D&& other) noexcept
	: m_Transform(std::move(other.GetTransform())), m_PackedParent(std::move(other.m_PackedParent))
{
}

TransformableProxy2D& TransformableProxy2D::operator=(const TransformableProxy2D& other)
{
	SetTransform(other.GetTransform());
	LinkPackedParent(other.m_PackedParent);
	return *this;
}

TransformableProxy2D& TransformableProxy2D::operator=(TransformableProxy2D&& other) noexcept
{
	SetTransform(std::move(other.GetTransform()));
	LinkPackedParent(std::move(other.m_PackedParent));
	return *this;
}
