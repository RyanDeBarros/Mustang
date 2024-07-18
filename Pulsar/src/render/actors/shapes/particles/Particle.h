#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <glm/glm.hpp>

#include "render/actors/shapes/DebugPolygon.h"
#include "render/Transform.h"

// TODO throughout project, pass by value for built-in types instead of const&.
typedef std::function<void(class Particle&)> ParticleProfileFunc;

class Particle
{
	template<std::unsigned_integral ParticleCount>
	friend class ParticleSystem;
	LocalTransformer2D m_Transformer;
	std::shared_ptr<DebugPolygon> m_Shape;
	std::vector<ParticleProfileFunc> m_ProfileFuncs;

	float m_LifespanInv;
	float m_T = 0.0f;
	bool m_Invalid = false;

public:
	Particle(const std::shared_ptr<DebugPolygon>& shape, const LocalTransformer2D& transformer, const float& lifespan, const std::vector<ParticleProfileFunc>& profile_funcs, const std::vector<ParticleProfileFunc>& initial_funcs = {});
	Particle& operator=(const Particle&);

private:
	void OnDraw();

};
