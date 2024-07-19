#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <glm/glm.hpp>

#include "Utils.h"
#include "render/actors/shapes/DebugPolygon.h"
#include "render/Transform.h"

// TODO throughout project, pass by value for built-in types instead of const&.
struct Particle;
struct ParticleProfileFunc
{
	std::function<void(Particle&)> f;
	void operator()(Particle& p) const { f(p); }
};

struct ParticleProfileFunc2ndSeed : ParticleProfileFunc
{
	ParticleProfileFunc2ndSeed(std::function<std::function<void(Particle&)>(float)> fgen) : ParticleProfileFunc{ fgen(rng()) } {}
};

struct ParticleProfileFunc3rdSeed : ParticleProfileFunc
{
	ParticleProfileFunc3rdSeed(std::function<std::function<void(Particle&)>(float, float)> fgen) : ParticleProfileFunc{ fgen(rng(), rng()) } {}
};

struct Particle
{
	LocalTransformer2D m_Transformer;
	std::shared_ptr<DebugPolygon> m_Shape;
	std::vector<ParticleProfileFunc> m_ProfileFuncs;

private:
	template<std::unsigned_integral ParticleCount>
	friend class ParticleSystem;
	real m_LifespanInv;
	real m_T = 0.0f;
	real m_DT = 0.0f;
	bool m_Invalid = false;

public:
	Particle(const std::shared_ptr<DebugPolygon>& shape, const LocalTransformer2D& transformer, const float& lifespan, const std::vector<ParticleProfileFunc>& profile_funcs, const std::vector<ParticleProfileFunc>& initial_funcs = {});
	Particle& operator=(const Particle&);
	
	inline real t() const { return m_T; }
	inline real dt() const { return m_DT; }

private:
	void OnDraw();

};
