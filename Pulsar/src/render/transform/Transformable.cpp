#include "Transformable.h"

Transformable2D::Transformable2D()
	: m_Transform(std::make_shared<Transform2D>())
{
}

Transformable2D::Transformable2D(const Transform2D& transform)
	: m_Transform(std::make_shared<Transform2D>(transform))
{
}

Transformable2D::Transformable2D(const std::shared_ptr<Transform2D>& transform)
	: m_Transform(std::make_shared<Transform2D>(*transform))
{
}

Transformable2D::Transformable2D(std::shared_ptr<Transform2D>&& transform)
	: m_Transform(std::move(transform))
{
}

Transformable2D::Transformable2D(const Transformable2D& other)
	: m_Transform(std::make_shared<Transform2D>(*other.m_Transform))
{
}

Transformable2D::Transformable2D(Transformable2D&& other) noexcept
	: m_Transform(std::move(other.m_Transform))
{
}

Transformable2D& Transformable2D::operator=(const Transformable2D& other)
{
	SetTransform(*other.m_Transform);
	return *this;
}

Transformable2D& Transformable2D::operator=(Transformable2D&& other) noexcept
{
	m_Transform = std::move(other.m_Transform);
	SetTransform(*m_Transform);
	return *this;
}
