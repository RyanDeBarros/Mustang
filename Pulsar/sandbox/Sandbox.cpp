#include "Pulsar.h"
#include "CHRS.h"
#include "Logger.inl"
#include "Macros.h"
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
	int startup = Pulsar::StartUp();
	auto window = Pulsar::WelcomeWindow("Pulsar Renderer");
	//window->SetPostInit(&post_init);
	//window->SetFrameStart(&frame_start-);
	// TODO PostInit and FrameStart should be methods on Window.
	Pulsar::PostInit(&post_init);
	Pulsar::FrameStart(&frame_start);

	Pulsar::Run(*window);

	Pulsar::Terminate();
	return startup;
}
