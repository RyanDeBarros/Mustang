#include "Transformable.h"

TransformableProxy2D::TransformableProxy2D(const Transform2D& transform)
	: m_Transform(transform)
{
}

TransformableProxy2D::TransformableProxy2D(const TransformableProxy2D& other)
	: m_Transform(other.m_Transform)
{
}

TransformableProxy2D::TransformableProxy2D(TransformableProxy2D&& other) noexcept
	: m_Transform(std::move(other.m_Transform))
{
}

TransformableProxy2D& TransformableProxy2D::operator=(const TransformableProxy2D& other)
{
	SetTransform(other.m_Transform);
	return *this;
}

TransformableProxy2D& TransformableProxy2D::operator=(TransformableProxy2D&& other) noexcept
{
	SetTransform(std::move(other.m_Transform));
	return *this;
}
