#include "Transformer.h"

Transformer2D::Transformer2D(const Transform2D& parent)
	: m_Transform(std::make_shared<TransformableProxy2D>(parent)), m_Packed(std::make_shared<PackedTransform2D>(parent))
{
}

Transformer2D::Transformer2D(const std::shared_ptr<Transformable2D>& external_parent)
	: m_Transform(external_parent), m_Packed(std::make_shared<PackedTransform2D>(m_Transform->GetTransform()))
{
}

Transformer2D::Transformer2D(std::shared_ptr<Transformable2D>&& external_parent)
	: m_Transform(std::move(external_parent)), m_Packed(std::make_shared<PackedTransform2D>(m_Transform->GetTransform()))
{
}

Transformer2D::Transformer2D(const Transformer2D& other)
	: m_Transform(std::make_shared<TransformableProxy2D>(other.m_Transform->GetTransform())), m_Packed(std::make_shared<PackedTransform2D>(m_Transform->GetTransform()))
{
}

Transformer2D::Transformer2D(Transformer2D&& other) noexcept
	: m_Transform(std::make_shared<TransformableProxy2D>(other.m_Transform->GetTransform())), m_Packed(std::make_shared<PackedTransform2D>(m_Transform->GetTransform()))
{
}

Transformer2D& Transformer2D::operator=(const Transformer2D& other)
{
	m_Transform = other.m_Transform;
	m_Packed->Set(m_Transform->GetTransform());
	return *this;
}

Transformer2D& Transformer2D::operator=(Transformer2D&& other) noexcept
{
	m_Transform = std::move(other.m_Transform);
	m_Packed->Set(m_Transform->GetTransform());
	return *this;
}

void Transformer2D::OperateTransform(const std::function<void(Transform2D& position)>& op)
{
	m_Transform->OperateTransform(op);
	m_Packed->Set(m_Transform->GetTransform());
}

void Transformer2D::OperatePosition(const std::function<void(glm::vec2& position)>& op)
{
	m_Transform->OperatePosition(op);
	m_Packed->position = m_Transform->GetPosition();
}

void Transformer2D::OperateRotation(const std::function<void(glm::float32& rotation)>& op)
{
	m_Transform->OperateRotation(op);
	m_Packed->gamma = Transform::Gamma(m_Transform->GetTransform());
}

void Transformer2D::OperateScale(const std::function<void(glm::vec2& scale)>& op)
{
	m_Transform->OperateScale(op);
	m_Packed->gamma = Transform::Gamma(m_Transform->GetTransform());
}