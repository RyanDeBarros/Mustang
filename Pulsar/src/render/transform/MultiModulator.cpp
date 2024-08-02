#include "MultiModulator.h"

MultiModulator::MultiModulator(const std::weak_ptr<Modulatable>& parent)
	: m_Parent(parent)
{
}

MultiModulator::MultiModulator(std::weak_ptr<Modulatable>&& parent)
	: m_Parent(std::move(parent))
{
}

MultiModulator::MultiModulator(const std::weak_ptr<Modulatable>& parent, const std::vector<std::weak_ptr<Modulatable>>& children, bool discard_old_colors)
	: m_Parent(parent), m_Children(children)
{
	for (size_t i = 0; i < m_Children.size(); i++)
		m_Locals.push_back({});
	if (discard_old_colors)
		SyncGlobalWithLocals();
	else
		SyncLocalWithGlobals();
}

MultiModulator::MultiModulator(const std::weak_ptr<Modulatable>& parent, std::vector<std::weak_ptr<Modulatable>>&& children, bool discard_old_colors)
	: m_Parent(parent), m_Children(std::move(children))
{
	for (size_t i = 0; i < m_Children.size(); i++)
		m_Locals.push_back({});
	if (discard_old_colors)
		SyncGlobalWithLocals();
	else
		SyncLocalWithGlobals();
}

MultiModulator::MultiModulator(std::weak_ptr<Modulatable>&& parent, const std::vector<std::weak_ptr<Modulatable>>& children, bool discard_old_colors)
	: m_Parent(std::move(parent)), m_Children(children)
{
	for (size_t i = 0; i < m_Children.size(); i++)
		m_Locals.push_back({});
	if (discard_old_colors)
		SyncGlobalWithLocals();
	else
		SyncLocalWithGlobals();
}

MultiModulator::MultiModulator(std::weak_ptr<Modulatable>&& parent, std::vector<std::weak_ptr<Modulatable>>&& children, bool discard_old_colors)
	: m_Parent(std::move(parent)), m_Children(std::move(children))
{
	for (size_t i = 0; i < m_Children.size(); i++)
		m_Locals.push_back({});
	if (discard_old_colors)
		SyncGlobalWithLocals();
	else
		SyncLocalWithGlobals();
}

MultiModulator::MultiModulator(const std::weak_ptr<Modulatable>& parent, const std::vector<glm::vec4>& locals, bool discard_old_colors)
	: m_Parent(parent), m_Locals(locals)
{
	for (size_t i = 0; i < m_Children.size(); i++)
		m_Children.push_back(std::make_shared<ModulatableProxy>());
	if (discard_old_colors)
		SyncLocalWithGlobals();
	else
		SyncGlobalWithLocals();
}

MultiModulator::MultiModulator(const std::weak_ptr<Modulatable>& parent, std::vector<glm::vec4>&& locals, bool discard_old_colors)
	: m_Parent(parent), m_Locals(std::move(locals))
{
	for (size_t i = 0; i < m_Children.size(); i++)
		m_Children.push_back(std::make_shared<ModulatableProxy>());
	if (discard_old_colors)
		SyncLocalWithGlobals();
	else
		SyncGlobalWithLocals();
}

MultiModulator::MultiModulator(std::weak_ptr<Modulatable>&& parent, const std::vector<glm::vec4>& locals, bool discard_old_colors)
	: m_Parent(std::move(parent)), m_Locals(locals)
{
	for (size_t i = 0; i < m_Children.size(); i++)
		m_Children.push_back(std::make_shared<ModulatableProxy>());
	if (discard_old_colors)
		SyncLocalWithGlobals();
	else
		SyncGlobalWithLocals();
}

MultiModulator::MultiModulator(std::weak_ptr<Modulatable>&& parent, std::vector<glm::vec4>&& locals, bool discard_old_colors)
	: m_Parent(std::move(parent)), m_Locals(std::move(locals))
{
	for (size_t i = 0; i < m_Children.size(); i++)
		m_Children.push_back(std::make_shared<ModulatableProxy>());
	if (discard_old_colors)
		SyncLocalWithGlobals();
	else
		SyncGlobalWithLocals();
}

MultiModulator::MultiModulator(const MultiModulator& other)
	: m_Parent(other.m_Parent), m_Children(other.m_Children), m_Locals(other.m_Locals)
{
}

MultiModulator::MultiModulator(MultiModulator&& other) noexcept
	: m_Parent(std::move(other.m_Parent)), m_Children(std::move(other.m_Children)), m_Locals(std::move(other.m_Locals))
{
}

MultiModulator& MultiModulator::operator=(const MultiModulator& other)
{
	m_Parent = other.m_Parent;
	m_Children = other.m_Children;
	m_Locals = other.m_Locals;
	return *this;
}

MultiModulator& MultiModulator::operator=(MultiModulator&& other) noexcept
{
	m_Parent = std::move(other.m_Parent);
	m_Children = std::move(other.m_Children);
	m_Locals = std::move(other.m_Locals);
	return *this;
}

void MultiModulator::SetLocalColor(size_t i, const glm::vec4& color)
{
	m_Locals[i] = color;
	SyncGlobalWithLocal(i);
}

void MultiModulator::OperateLocalColor(size_t i, const std::function<void(glm::vec4& color)>& op)
{
	op(m_Locals[i]);
	SyncGlobalWithLocal(i);
}

void MultiModulator::SyncGlobalWithLocal(size_t i)
{
	WEAK_LOCK_CHECK(m_Parent, p);
	WEAK_LOCK_CHECK(m_Children[i], c);
	c->SetColor(m_ModulationMethod(p->GetColor(), m_Locals[i]));
}

void MultiModulator::SyncLocalWithGlobal(size_t i)
{
	WEAK_LOCK_CHECK(m_Parent, p);
	WEAK_LOCK_CHECK(m_Children[i], c);
	m_Locals[i] = m_ModulationMethodInverse(p->GetColor(), c->GetColor());
}

std::vector<glm::vec4> MultiModulator::GetGlobalColors() const
{
	std::vector<glm::vec4> colors;
	for (size_t i = 0; i < m_Children.size(); i++)
	{
		WEAK_LOCK_CHECK(m_Children[i], c);
		colors.push_back(c->GetColor());
	}
	return colors;
}

void MultiModulator::SetGlobalColor(size_t i, const glm::vec4& color)
{
	OperateGlobalColor(i, [&color](glm::vec4& c) { c = color; });
}

void MultiModulator::OperateGlobalColor(size_t i, const std::function<void(glm::vec4& color)>& op)
{
	WEAK_LOCK_CHECK(m_Children[i], c);
	c->OperateColor(op);
	SyncLocalWithGlobal(i);
}

void MultiModulator::PushBackGlobal(const std::weak_ptr<Modulatable>& child, bool discard_old_color)
{
	m_Children.push_back(child);
	m_Locals.push_back({});
	if (discard_old_color)
		SyncGlobalWithLocal(m_Locals.size() - 1);
	else
		SyncLocalWithGlobal(m_Locals.size() - 1);
}

void MultiModulator::PushBackGlobal(std::weak_ptr<Modulatable>&& child, bool discard_old_color)
{
	m_Children.push_back(std::move(child));
	m_Locals.push_back({});
	if (discard_old_color)
		SyncGlobalWithLocal(m_Locals.size() - 1);
	else
		SyncLocalWithGlobal(m_Locals.size() - 1);
}

void MultiModulator::PushBackGlobals(const std::vector<std::weak_ptr<Modulatable>>& children, bool discard_old_color)
{
	for (const auto& child : children)
		PushBackGlobal(child, discard_old_color);
}

void MultiModulator::PushBackGlobals(std::vector<std::weak_ptr<Modulatable>>&& children, bool discard_old_color)
{
	for (auto& child : children)
		PushBackGlobal(std::move(child), discard_old_color);
}

std::shared_ptr<ModulatableProxy> MultiModulator::PushBackLocal(const glm::vec4& local, bool discard_old_color)
{
	std::shared_ptr<ModulatableProxy> child(std::make_shared<ModulatableProxy>());
	m_Children.push_back(child);
	m_Locals.push_back(local);
	if (discard_old_color)
		SyncLocalWithGlobal(m_Locals.size() - 1);
	else
		SyncGlobalWithLocal(m_Locals.size() - 1);
	return child;
}

std::vector<std::shared_ptr<ModulatableProxy>> MultiModulator::PushBackLocals(const std::vector<glm::vec4>& locals, bool discard_old_color)
{
	std::vector<std::shared_ptr<ModulatableProxy>> children;
	for (const auto& local : locals)
		children.push_back(PushBackLocal(local, discard_old_color));
	return children;
}

void MultiModulator::Remove(size_t i)
{
	m_Children.erase(m_Children.begin() + i);
	m_Locals.erase(m_Locals.begin() + i);
}

void MultiModulator::Remove(const std::vector<std::weak_ptr<Modulatable>>::iterator& where)
{
	m_Children.erase(where);
	m_Locals.erase(m_Locals.begin() + (where - m_Children.begin()));
}

void MultiModulator::SwapPop(size_t i)
{
	if (m_Children.size() > 1)
	{
		std::swap(m_Children[i], m_Children.back());
		std::swap(m_Locals[i], m_Locals.back());
	}
	m_Children.pop_back();
	m_Locals.pop_back();
}

std::vector<std::weak_ptr<Modulatable>>::iterator MultiModulator::Find(const std::weak_ptr<Modulatable>& child)
{
	return std::find(m_Children.begin(), m_Children.end(), child);
}
