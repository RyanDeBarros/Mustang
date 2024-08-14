#include "ParticleEffect.h"

#include "Pulsar.h"
#include "RendererSettings.h"

template class ParticleEffect<unsigned short>;

// TODO eventually, create factory for particle effects?

template<std::unsigned_integral ParticleCount>
ParticleEffect<ParticleCount>::ParticleEffect(const std::vector<ParticleSubsystemData<ParticleCount>>& subsystem_data, const Transform2D& transform, const glm::vec4& modulate, bool enabled)
	: m_Modulate(std::make_shared<ModulatableProxy>(modulate)), enabled(enabled), m_Transformer(transform), m_Modulator(m_Modulate)
{
	ParticleSubsystemIndex i = 0;
	for (const auto& subsys : subsystem_data)
	{
		std::shared_ptr<ParticleSubsystem<ParticleCount>> subsystem = std::make_shared<ParticleSubsystem<ParticleCount>>(subsys, i++);
		m_Subsystems.push_back(subsystem);
		m_Transformer.Attach(&subsystem->m_Transformer);
		m_Modulator.PushBackGlobal(subsystem->m_Modulator);
	}
	Reset();
}

// TODO some way of skipping frames for intensive actors like particle systems. i.e., instead of updating every draw frame, an intensive actor can update for say, 120FPS, to limit draw/update overhead. Drawing will still happen obviously, but the update will happen every N frames. Maybe separate the two into separate functions.
template<std::unsigned_integral ParticleCount>
void ParticleEffect<ParticleCount>::OnUpdate()
{
	// TODO perhaps spawning and enabled should be moved to particle subsystem?
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
	{
		if (m_PlayTime > 0.0f && m_TotalPlayed > m_PlayTime)
		{
			spawning = false;
			Reset();
			return;
		}
		for (auto& subsystem : m_Subsystems)
			subsystem->OnSpawnFrame(*this);
	}
	if (enabled)
	{
		for (auto& subsystem : m_Subsystems)
			subsystem->OnParticlesUpdate(*this);
	}
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
