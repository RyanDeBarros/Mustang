#include "Particle.h"

#include "Pulsar.h"
#include "Utils.h"

Particle::Particle(const std::shared_ptr<DebugPolygon>& shape, const LocalTransformer2D& transformer, const float& lifespan, const std::vector<ParticleProfileFunc>& profile_funcs, const std::vector<ParticleProfileFunc>& initial_funcs)
	: m_Shape(shape), m_Transformer(transformer), m_ProfileFuncs(profile_funcs), m_LifespanInv(1.0f / lifespan)
{
	for (const auto& func : initial_funcs)
		func(*this);
}

Particle& Particle::operator=(const Particle& other)
{
	m_Shape = other.m_Shape;
	m_Transformer = other.m_Transformer;
	m_ProfileFuncs = other.m_ProfileFuncs;
	m_LifespanInv = other.m_LifespanInv;
	m_Invalid = other.m_Invalid;
	m_T = other.m_T;
	return *this;
}

void Particle::OnDraw()
{
	if ((m_T += Pulsar::deltaDrawTime * m_LifespanInv) > 1.0f)
		m_Invalid = true;
	else
	{
		for (const auto& func : m_ProfileFuncs)
			func(*this);
	}
}
