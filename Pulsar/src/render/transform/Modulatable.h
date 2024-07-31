#pragma once

#include <glm/glm.hpp>
#include <functional>

class Modulatable
{
public:
	inline virtual glm::vec4 GetColor() const { return {}; }
	inline virtual void OperateColor(const std::function<void(glm::vec4&)>& op) {}
	inline void SetColor(const glm::vec4& color) { OperateColor([&color](glm::vec4& c) { c = color; }); }
};

class ModulatableProxy : public Modulatable
{
protected:
	glm::vec4 m_Color;

public:
	ModulatableProxy(const glm::vec4& color = {});
	ModulatableProxy(const ModulatableProxy&);
	ModulatableProxy(ModulatableProxy&&) noexcept;
	ModulatableProxy& operator=(const ModulatableProxy&);
	ModulatableProxy& operator=(ModulatableProxy&&) noexcept;

	inline glm::vec4 GetColor() const override { return m_Color; }
	inline virtual void OperateColor(const std::function<void(glm::vec4&)>& op) override { op(m_Color); }
};
