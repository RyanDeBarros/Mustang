#include "ParticleEffect.h"

#include "Pulsar.h"
#include "RendererSettings.h"

template class ParticleEffect<unsigned short>;

template<std::unsigned_integral ParticleCount>
ParticleEffect<ParticleCount>::ParticleEffect(const std::vector<ParticleSubsystemData<ParticleCount>>& subsystem_data, const Transform2D& transform, bool enabled)
	: Transformable2D(transform), enabled(enabled), m_Transformer(m_Transform)
{
	ParticleSubsystemIndex i = 0;
	for (const auto& subsys : subsystem_data)
	{
		std::shared_ptr<ParticleSubsystem<ParticleCount>> subsystem = std::make_shared<ParticleSubsystem<ParticleCount>>(ParticleSubsystem<ParticleCount>(subsys, i));
		m_Subsystems.push_back(subsystem);
		m_Transformer.PushBackGlobal(subsystem);
		m_Particles.push_back({});
	}
	Reset();
}

// TODO some way of skipping frames for intensive actors like particle systems. i.e., instead of updating every draw frame, an intensive actor can update for say, 120FPS, to limit draw/update overhead. Drawing will still happen obviously, but the update will happen every N frames. Maybe separate the two into separate functions.
template<std::unsigned_integral ParticleCount>
void ParticleEffect<ParticleCount>::OnUpdate()
{
	// TODO perhaps spawning and enabled should be moved to particle wave?
	if (!paused)
	{
		m_LeftoverDT += Pulsar::deltaDrawTime;
		if (m_LeftoverDT >= _RendererSettings::particle_frame_length)
		{
			m_DeltaTime = _RendererSettings::particle_frame_length;
			m_LeftoverDT -= _RendererSettings::particle_frame_length;
		}
		else
		{
			m_DeltaTime = m_LeftoverDT;
			m_LeftoverDT = 0.0f;
		}
		m_TotalPlayed += m_DeltaTime;
	}
	if (spawning)
		OnSubsystemsUpdate();
	if (enabled)
		OnParticlesUpdate();
}

template<std::unsigned_integral ParticleCount>
void ParticleEffect<ParticleCount>::OnSubsystemsUpdate()
{
	if (m_PlayTime > 0.0f && m_TotalPlayed > m_PlayTime)
	{
		spawning = false;
		Reset();
		return;
	}
	for (auto& subsystem : m_Subsystems)
		subsystem->OnUpdate(*this);
}

template<std::unsigned_integral ParticleCount>
void ParticleEffect<ParticleCount>::OnParticlesUpdate()
{
	bool erase = false;
	for (ParticleSubsystemIndex i = 0; i < m_Particles.size(); i++)
	{
		for (auto& part : m_Particles[i])
		{
			part.OnDraw(m_DeltaTime);
			if (part.m_Invalid)
			{
				erase = true;
				InvalidateParticle(part, i);
			}
		}
	}
	if (erase)
		DespawnInvalidParticles();
}

template<std::unsigned_integral ParticleCount>
void ParticleEffect<ParticleCount>::DespawnInvalidParticles()
{
	for (auto& p_vec : m_Particles)
		std::erase_if(p_vec, [](const Particle& part) { return part.m_Invalid; });
}

template<std::unsigned_integral ParticleCount>
void ParticleEffect<ParticleCount>::AddParticle(ParticleSubsystemIndex i, const Particle& part)
{
	m_Particles[i].push_back(std::move(part));
}

template<std::unsigned_integral ParticleCount>
void ParticleEffect<ParticleCount>::Pause()
{
	if (enabled || spawning)
	{
		enabled = false;
		spawning = false;
		paused = true;
	}
}

template<std::unsigned_integral ParticleCount>
void ParticleEffect<ParticleCount>::Resume()
{
	if (!enabled || !spawning)
	{
		enabled = true;
		spawning = true;
		paused = false;
	}
}

template<std::unsigned_integral ParticleCount>
void ParticleEffect<ParticleCount>::PlayFor(real n)
{
	m_PlayTime = n;
	spawning = true;
}
