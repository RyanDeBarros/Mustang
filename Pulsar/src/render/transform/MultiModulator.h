#pragma once

#include "Modulatable.h"
#include "Transformable.h"
#include "Logger.h"
#include "Modulator.h"

class MultiModulator : public Modulatable
{
	std::weak_ptr<Modulatable> m_Parent;
	std::vector<std::weak_ptr<Modulatable>> m_Children;
	std::vector<glm::vec4> m_Locals;

public:
	MultiModulator() = delete;
	MultiModulator(const std::weak_ptr<Modulatable>& parent);
	MultiModulator(std::weak_ptr<Modulatable>&& parent);

	MultiModulator(const std::weak_ptr<Modulatable>& parent, const std::vector<std::weak_ptr<Modulatable>>& children, bool discard_old_colors = true);
	MultiModulator(const std::weak_ptr<Modulatable>& parent, std::vector<std::weak_ptr<Modulatable>>&& children, bool discard_old_colors = true);
	MultiModulator(std::weak_ptr<Modulatable>&& parent, const std::vector<std::weak_ptr<Modulatable>>& children, bool discard_old_colors = true);
	MultiModulator(std::weak_ptr<Modulatable>&& parent, std::vector<std::weak_ptr<Modulatable>>&& children, bool discard_old_colors = true);

	MultiModulator(const std::weak_ptr<Modulatable>& parent, const std::vector<glm::vec4>& locals, bool discard_old_colors = true);
	MultiModulator(const std::weak_ptr<Modulatable>& parent, std::vector<glm::vec4>&& locals, bool discard_old_colors = true);
	MultiModulator(std::weak_ptr<Modulatable>&& parent, const std::vector<glm::vec4>& locals, bool discard_old_colors = true);
	MultiModulator(std::weak_ptr<Modulatable>&& parent, std::vector<glm::vec4>&& locals, bool discard_old_colors = true);

	MultiModulator(const MultiModulator&);
	MultiModulator(MultiModulator&&) noexcept;
	MultiModulator& operator=(const MultiModulator&);
	MultiModulator& operator=(MultiModulator&&) noexcept;
	
	inline void SetParent(const std::weak_ptr<Modulatable>& parent) { m_Parent = parent; }
	inline void SetParent(std::weak_ptr<Modulatable>&& parent) { m_Parent = std::move(parent); }

	inline glm::vec4 GetColor() const override { WEAK_LOCK_CHECK(m_Parent, p) return p->GetColor(); }
	inline void OperateColor(const std::function<void(glm::vec4& color)>& op) override { WEAK_LOCK_CHECK(m_Parent, p) p->OperateColor(op); SyncGlobalWithLocals(); }
	inline void SetColorParentOnly(const glm::vec4& color) { OperateColorParentOnly([&color](glm::vec4& c) { c = color; }); }
	inline void OperateColorParentOnly(const std::function<void(glm::vec4& color)>& op) { WEAK_LOCK_CHECK(m_Parent, p) p->OperateColor(op); SyncLocalWithGlobals(); }

	void SetLocalColors(const glm::vec4& color) { for (size_t i = 0; i < m_Locals.size(); i++) SetLocalColor(i, color); }
	inline void OperateLocalColors(const std::function<void(glm::vec4& color)>& op) { for (size_t i = 0; i < m_Locals.size(); i++) OperateLocalColor(i, op); }
	inline std::vector<glm::vec4> GetLocalColors() const { return m_Locals; }

	void SetLocalColor(size_t i, const glm::vec4& color);
	void OperateLocalColor(size_t i, const std::function<void(glm::vec4& color)>&);
	inline glm::vec4 GetLocalColor(size_t i) const { return m_Locals[i]; }

	inline void SyncGlobalWithLocals() { for (size_t i = 0; i < m_Locals.size(); i++) SyncGlobalWithLocal(i); }
	inline void SyncLocalWithGlobals() { for (size_t i = 0; i < m_Locals.size(); i++) SyncLocalWithGlobal(i); }

	void SyncGlobalWithLocal(size_t i);
	void SyncLocalWithGlobal(size_t i);

	void SetGlobalColors(const glm::vec4& color) { for (size_t i = 0; i < m_Locals.size(); i++) SetGlobalColor(i, color); }
	void OperateGlobalColors(const std::function<void(glm::vec4& color)>& op) { for (size_t i = 0; i < m_Locals.size(); i++) OperateGlobalColor(i, op); }
	std::vector<glm::vec4> GetGlobalColors() const;
	
	void SetGlobalColor(size_t i, const glm::vec4& color);
	void OperateGlobalColor(size_t i, const std::function<void(glm::vec4& color)>&);
	inline glm::vec4 GetGlobalColor(size_t i) const { WEAK_LOCK_CHECK(m_Children[i], c) return c->GetColor(); }

	void PushBackGlobal(const std::weak_ptr<Modulatable>& child, bool discard_old_color = true);
	void PushBackGlobal(std::weak_ptr<Modulatable>&& child, bool discard_old_color = true);
	void PushBackGlobals(const std::vector<std::weak_ptr<Modulatable>>& children, bool discard_old_color = true);
	void PushBackGlobals(std::vector<std::weak_ptr<Modulatable>>&& children, bool discard_old_color = true);
	std::shared_ptr<ModulatableProxy> PushBackLocal(const glm::vec4& local, bool discard_old_color = true);
	std::vector<std::shared_ptr<ModulatableProxy>> PushBackLocals(const std::vector<glm::vec4>& locals, bool discard_old_color = true);
	void Remove(size_t i);
	void Remove(const std::vector<std::weak_ptr<Modulatable>>::iterator& where);
	std::vector<std::weak_ptr<Modulatable>>::iterator Find(const std::weak_ptr<Modulatable>& child);
	inline size_t Size() const { return m_Locals.size(); }

	std::function<glm::vec4(const glm::vec4& parent, const glm::vec4& local)> m_ModulationMethod = StandardModulationMethod;
	std::function<glm::vec4(const glm::vec4& parent, const glm::vec4& child)> m_ModulationMethodInverse = StandardModulationMethodInverse;
};
