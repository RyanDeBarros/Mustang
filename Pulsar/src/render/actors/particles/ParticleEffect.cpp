#include "ParticleEffect.h"

#include "Pulsar.h"
#include "RendererSettings.h"

// TODO eventually, create factory for particle effects?

ParticleEffect::ParticleEffect(const std::vector<ParticleSubsystemData>& subsystem_data, ZIndex z, FickleType fickle_type, bool enabled)
	: FickleActor2D(fickle_type, z), enabled(enabled)
{
	ParticleSubsystemIndex i = 0;
	for (const auto& subsys : subsystem_data)
	{
		std::shared_ptr<ParticleSubsystem> subsystem = std::make_shared<ParticleSubsystem>(subsys, i++, fickle_type);
		m_Subsystems.push_back(subsystem);
		m_Fickler.Attach(subsystem->Fickler());
	}
	Reset();
}

// TODO some way of skipping frames for intensive actors like particle systems. i.e., instead of updating every draw frame, an intensive actor can update for say, 120FPS, to limit draw/update overhead. Drawing will still happen obviously, but the update will happen every N frames. Maybe separate the two into separate functions.
void ParticleEffect::OnUpdate()
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

void ParticleEffect::Pause()
{
	if (enabled || spawning)
	{
		enabled = false;
		spawning = false;
		paused = true;
	}
}

void ParticleEffect::Resume()
{
	if (!enabled || !spawning)
	{
		enabled = true;
		spawning = true;
		paused = false;
	}
}

void ParticleEffect::PlayFor(real n)
{
	m_PlayTime = n;
	spawning = true;
}
