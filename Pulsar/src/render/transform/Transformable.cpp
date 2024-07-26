#include "Transformable.h"

Transformable2D::Transformable2D()
	: m_Transform(std::make_shared<Transform2D>(Transform2D{}))
{
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
