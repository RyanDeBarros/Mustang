#include "Protean.h"

ProteanProxy2D::ProteanProxy2D(const Transform2D& transform, const glm::vec4& color)
	: TransformableProxy2D(transform), ModulatableProxy(color)
{
}

ProteanProxy2D::ProteanProxy2D(const ProteanProxy2D& other)
	: TransformableProxy2D(other), ModulatableProxy(other)
{
}

ProteanProxy2D::ProteanProxy2D(ProteanProxy2D&& other) noexcept
	: TransformableProxy2D(std::move(other)), ModulatableProxy(std::move(other))
{
}

ProteanProxy2D& ProteanProxy2D::operator=(const ProteanProxy2D& other)
{
	TransformableProxy2D::operator=(other);
	ModulatableProxy::operator=(other);
	return *this;
}

ProteanProxy2D& ProteanProxy2D::operator=(ProteanProxy2D&& other) noexcept
{
	TransformableProxy2D::operator=(std::move(other));
	ModulatableProxy::operator=(std::move(other));
	return *this;
}
