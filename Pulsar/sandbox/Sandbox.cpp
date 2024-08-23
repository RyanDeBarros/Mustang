#include "Pulsar.h"
#include "CHRS.h"
#include "render/actors/particles/ParticleSubsystemRegistry.h"

static void register_particle_subsystems()
{
	ParticleSubsystemRegistry::Instance().Register("wave1", &Sandbox::wave1);
	ParticleSubsystemRegistry::Instance().Register("wave2", &Sandbox::wave2);
}

static void post_init()
{
	register_particle_subsystems();
}

static void frame_start()
{
}

int main()
{
	Pulsar::PostInit(&post_init);
	Pulsar::FrameStart(&frame_start);

	GLFWwindow* window = nullptr;
	int startup = Pulsar::StartUp(window);
	Pulsar::Run(window);
	Pulsar::Terminate();
	return startup;
}
