#pragma once

#include <memory>

#include "Modulatable.h"
#include "Transformable.h"
#include "Logger.h"
#include "utils/CommonMath.h"

static auto StandardModulationMethod = [](const glm::vec4& parent, const glm::vec4& local) -> glm::vec4 { return parent * local; };
static auto StandardModulationMethodInverse = [](const glm::vec4& parent, const glm::vec4& child) -> glm::vec4 { return DivideOrZero(child, parent); };

class Modulator : public Modulatable
{
	std::weak_ptr<Modulatable> m_Parent;
	std::weak_ptr<Modulatable> m_Child;
	glm::vec4 m_Local;

public:
	Modulator() = delete;

	Modulator(const std::weak_ptr<Modulatable>& parent, const std::weak_ptr<Modulatable>& child, bool discard_old_color = true);
	Modulator(const std::weak_ptr<Modulatable>& parent, std::weak_ptr<Modulatable>&& child, bool discard_old_color = true);
	Modulator(std::weak_ptr<Modulatable>&& parent, const std::weak_ptr<Modulatable>& child, bool discard_old_color = true);
	Modulator(std::weak_ptr<Modulatable>&& parent, std::weak_ptr<Modulatable>&& child, bool discard_old_color = true);

	Modulator(const std::weak_ptr<Modulatable>& parent, const glm::vec4& local, bool discard_old_color = true);
	Modulator(std::weak_ptr<Modulatable>&& parent, const glm::vec4& local, bool discard_old_color = true);

	Modulator(const Modulator&);
	Modulator(Modulator&&) noexcept;
	Modulator& operator=(const Modulator&);
	Modulator& operator=(Modulator&&) noexcept;

	inline glm::vec4 GetColor() const override { WEAK_LOCK_CHECK(m_Parent, p) return p->GetColor(); }
	inline void OperateColor(const std::function<void(glm::vec4&)>& op) override { WEAK_LOCK_CHECK(m_Parent, p) p->OperateColor(op); SyncGlobalWithLocal(); }
	inline void SetColorParentOnly(const glm::vec4& color) { OperateColorParentOnly([&color](glm::vec4& c) { c = color; }); }
	inline void OperateColorParentOnly(const std::function<void(glm::vec4&)>& op) { WEAK_LOCK_CHECK(m_Parent, p) p->OperateColor(op); SyncLocalWithGlobal(); }

	inline void SetLocalColor(const glm::vec4& color) { OperateLocalColor([&color](glm::vec4& c) { c = color; }); }
	void OperateLocalColor(const std::function<void(glm::vec4&)>&);
	glm::vec4 GetLocalColor() const { return m_Local; }

	void SyncGlobalWithLocal();
	void SyncLocalWithGlobal();

	inline void SetGlobalColor(const glm::vec4& color) { OperateGlobalColor([&color](glm::vec4& c) { c = color; }); }
	void OperateGlobalColor(const std::function<void(glm::vec4&)>&);
	glm::vec4 GetGlobalColor() const { WEAK_LOCK_CHECK(m_Child, c) return c->GetColor(); }

	std::function<glm::vec4(const glm::vec4& parent, const glm::vec4& local)> m_ModulationMethod = StandardModulationMethod;
	std::function<glm::vec4(const glm::vec4& parent, const glm::vec4& child)> m_ModulationMethodInverse = StandardModulationMethodInverse;
};
