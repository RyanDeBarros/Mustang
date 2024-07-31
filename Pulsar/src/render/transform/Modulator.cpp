#include "Modulator.h"

Modulator::Modulator(const std::weak_ptr<Modulatable>& parent, const std::weak_ptr<Modulatable>& child, bool discard_old_color)
	: m_Parent(parent), m_Child(child)
{
	if (discard_old_color)
		SyncGlobalWithLocal();
	else
		SyncLocalWithGlobal();
}

Modulator::Modulator(const std::weak_ptr<Modulatable>& parent, std::weak_ptr<Modulatable>&& child, bool discard_old_color)
	: m_Parent(parent), m_Child(std::move(child))
{
	if (discard_old_color)
		SyncGlobalWithLocal();
	else
		SyncLocalWithGlobal();
}

Modulator::Modulator(std::weak_ptr<Modulatable>&& parent, const std::weak_ptr<Modulatable>& child, bool discard_old_color)
	: m_Parent(std::move(parent)), m_Child(child)
{
	if (discard_old_color)
		SyncGlobalWithLocal();
	else
		SyncLocalWithGlobal();
}

Modulator::Modulator(std::weak_ptr<Modulatable>&& parent, std::weak_ptr<Modulatable>&& child, bool discard_old_color)
	: m_Parent(std::move(parent)), m_Child(std::move(child))
{
	if (discard_old_color)
		SyncGlobalWithLocal();
	else
		SyncLocalWithGlobal();
}

Modulator::Modulator(const std::weak_ptr<Modulatable>& parent, const glm::vec4& local, bool discard_old_color)
	: m_Parent(parent), m_Child(std::make_shared<ModulatableProxy>()), m_Local(local)
{
	if (discard_old_color)
		SyncLocalWithGlobal();
	else
		SyncGlobalWithLocal();
}

Modulator::Modulator(std::weak_ptr<Modulatable>&& parent, const glm::vec4& local, bool discard_old_color)
	: m_Parent(std::move(parent)), m_Child(std::make_shared<ModulatableProxy>()), m_Local(local)
{
	if (discard_old_color)
		SyncLocalWithGlobal();
	else
		SyncGlobalWithLocal();
}

Modulator::Modulator(const Modulator& other)
	: m_Parent(other.m_Parent), m_Child(other.m_Child), m_Local(other.m_Local)
{
}

Modulator::Modulator(Modulator&& other) noexcept
	: m_Parent(std::move(other.m_Parent)), m_Child(std::move(other.m_Child)), m_Local(other.m_Local)
{
}

Modulator& Modulator::operator=(const Modulator& other)
{
	m_Parent = other.m_Parent;
	m_Child = other.m_Child;
	m_Local = other.m_Local;
	return *this;
}

Modulator& Modulator::operator=(Modulator&& other) noexcept
{
	m_Parent = std::move(other.m_Parent);
	m_Child = std::move(other.m_Child);
	m_Local = other.m_Local;
	return *this;
}

void Modulator::OperateLocalColor(const std::function<void(glm::vec4&)>& op)
{
	op(m_Local);
	SyncGlobalWithLocal();
}

void Modulator::SyncGlobalWithLocal()
{
	WEAK_LOCK_CHECK(m_Child, c);
	WEAK_LOCK_CHECK(m_Parent, p);
	c->SetColor(m_ModulationMethod(p->GetColor(), m_Local));
}

void Modulator::SyncLocalWithGlobal()
{
	WEAK_LOCK_CHECK(m_Child, c);
	WEAK_LOCK_CHECK(m_Parent, p);
	m_Local = m_ModulationMethodInverse(p->GetColor(), c->GetColor());
}

void Modulator::OperateGlobalColor(const std::function<void(glm::vec4&)>& op)
{
	WEAK_LOCK_CHECK(m_Child, c);
	c->OperateColor(op);
	SyncLocalWithGlobal();
}
