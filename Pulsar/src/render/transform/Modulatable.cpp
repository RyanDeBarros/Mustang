#include "Modulatable.h"

ModulatableProxy::ModulatableProxy(const glm::vec4& color)
	: m_Color(color)
{
}

ModulatableProxy::ModulatableProxy(const ModulatableProxy& other)
	: m_Color(other.m_Color)
{
}

ModulatableProxy::ModulatableProxy(ModulatableProxy&& other) noexcept
	: m_Color(other.m_Color)
{
}

ModulatableProxy& ModulatableProxy::operator=(const ModulatableProxy& other)
{
	SetColor(other.GetColor());
	return *this;
}

ModulatableProxy& ModulatableProxy::operator=(ModulatableProxy&& other) noexcept
{
	SetColor(std::move(other.GetColor()));
	return *this;
}
