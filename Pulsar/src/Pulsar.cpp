#include "Pulsar.h"

#include <cmath>

#include <glm/ext/scalar_constants.hpp>

#include "Typedefs.h"
#include "RendererSettings.h"
#include "Logger.h"
#include "Macros.h"
#include "AssetLoader.h"
#include "render/Renderer.h"
#include "render/actors/ActorPrimitive.h"
#include "render/actors/RectRender.h"
#include "factory/UniformLexiconFactory.h"
#include "render/actors/ActorTesselation.h"
#include "factory/ShaderFactory.h"
#include "factory/Atlas.h"
#include "render/actors/TileMap.h"
#include "render/actors/shapes/DebugPolygon.h"
#include "render/actors/shapes/DebugPoint.h"
#include "render/actors/shapes/DebugCircle.h"
#include "render/actors/shapes/DebugBatcher.h"
#include "render/actors/shapes/particles/ParticleSystem.h"
#include "render/actors/shapes/particles/ParticleWave.h"
#include "render/actors/shapes/particles/Particle.h"

using namespace Pulsar;

real Pulsar::drawTime;
real Pulsar::deltaDrawTime;
real Pulsar::prevDrawTime;
real Pulsar::totalDrawTime;

static void window_refresh_callback(GLFWwindow* window)
{
	Renderer::OnDraw();
	TRY(glFinish()); // important, this waits until rendering result is actually visible, thus making resizing less ugly
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// TODO
	// If _RendererSettings::resize_mode is set to SCALE_IGNORE_ASPECT_RATIO, don't add anything.
	// If it is set to SCALE_KEEP_ASPECT_RATIO, call new Renderer function that will scale objects as usual without stretching their aspect ratios.
	// If it is set to NO_SCALE_KEEP_SIZE, call new Renderer function that will not scale objects - only display more of the scene.
	// TODO also update frame in callback. Currently, animation is paused when resizing.
	TRY(glViewport(0, 0, width, height));
	Renderer::OnDraw();
}

int main()
{
	int startup = Pulsar::StartUp();
	Pulsar::Terminate();
	return startup;
}

int Pulsar::StartUp()
{
	if (!_LoadRendererSettings())
		return -1;
	if (!glfwInit())
		return -1;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	GLFWwindow* window = glfwCreateWindow((int)_RendererSettings::initial_window_width, (int)_RendererSettings::initial_window_height, "Pulsar Renderer", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	glfwSetWindowRefreshCallback(window, window_refresh_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	if (glewInit() != GLEW_OK)
	{
		glfwTerminate();
		return -1;
	}
	Logger::LogInfo("Welcome to Pulsar Renderer! GL_VERSION:");
	TRY(Logger::LogInfo(glGetString(GL_VERSION)));
	std::srand(time(0));
	Renderer::Init();
	Renderer::FocusWindow(window);
	Run(window);
	return 0;
}

void Pulsar::Terminate()
{
	Renderer::Terminate();
	glfwTerminate();
}

void Pulsar::Run(GLFWwindow* window)
{
	prevDrawTime = drawTime = static_cast<real>(glfwGetTime());
	deltaDrawTime = totalDrawTime = 0;

	// Load textures
	TextureHandle textureSnowman, textureTux, textureFlag;
	if (loadTexture("res/assets/snowman.toml", textureSnowman) != LOAD_STATUS::OK)
		ASSERT(false);
	if (loadTexture("res/assets/tux.toml", textureTux) != LOAD_STATUS::OK)
		ASSERT(false);
	if (loadTexture("res/assets/flag.toml", textureFlag, true) != LOAD_STATUS::OK)
		ASSERT(false);
	//if (loadTexture("res/assets/atlas.toml", textureAtlas) != LOAD_STATUS::OK)
	//	ASSERT(false);
	TextureHandle tex_dirtTL = TextureFactory::GetHandle("res/textures/dirtTL.png");
	TextureHandle tex_dirtTR = TextureFactory::GetHandle("res/textures/dirtTR.png");
	TextureHandle tex_grassSingle = TextureFactory::GetHandle("res/textures/grassSingle.png");
	TextureHandle tex_grassTL = TextureFactory::GetHandle("res/textures/grassTL.png");
	TextureHandle tex_grassTE = TextureFactory::GetHandle("res/textures/grassTE.png");
	TextureHandle tex_grassTR = TextureFactory::GetHandle("res/textures/grassTR.png");

	Renderer::AddCanvasLayer(0);

	// Create actors
	std::shared_ptr<RectRender> actor1(new RectRender(textureFlag, { {-500.0f, 300.0f}, -1.0f, {0.8f, 1.2f} }));
	std::shared_ptr<RectRender> actor2(new RectRender(textureSnowman, { {400.0f, -200.0f}, 0.25f, {0.7f, 0.7f} }));
	std::shared_ptr<RectRender> actor3(new RectRender(textureTux, { {0.0f, 0.0f}, 0.0f, {1.0f, 1.0f} }));

	Renderer::GetCanvasLayer(0)->OnAttach(actor1.get());
	Renderer::GetCanvasLayer(0)->OnAttach(actor2.get());
	Renderer::GetCanvasLayer(0)->OnSetZIndex(actor1.get(), 1);
	Renderer::GetCanvasLayer(0)->OnAttach(actor3.get());
	Renderer::AddCanvasLayer(-1);

	Renderable renderable;
	if (loadRenderable("res/assets/renderable.toml", renderable, true) != LOAD_STATUS::OK)
		ASSERT(false);
	std::shared_ptr<ActorPrimitive2D> actor4(new ActorPrimitive2D(renderable, { {-200.0f, 0.0f}, 0.0f, {800.0f, 800.0f} }));
	Renderer::GetCanvasLayer(-1)->OnAttach(actor4.get());

	actor1->SetScale(16.0f, 16.0f);
	Renderer::GetCanvasLayer(0)->OnSetZIndex(actor3.get(), -1);

	actor3->SetPivot(0.0f, 0.0f);
	actor3->SetPosition(_RendererSettings::initial_window_rel_pos(-0.5f, 0.5f));
	actor3->SetScale(0.3f, 0.3f);
	actor2->SetModulation(glm::vec4(0.7f, 0.7f, 1.0f, 1.0f));
	actor3->SetModulationPerPoint({
		glm::vec4(1.0f, 0.5f, 0.5f, 1.0f),
		glm::vec4(0.5f, 1.0f, 0.5f, 1.0f),
		glm::vec4(0.5f, 0.5f, 1.0f, 1.0f),
		glm::vec4(0.5f, 0.5f, 0.5f, 1.0f),
		});

	TextureFactory::SetSettings(actor1->GetTextureHandle(), Texture::linear_settings);

	actor3->SetPivot(0.5f, 0.5f);
	actor3->SetPosition(0.0f, 0.0f);
	actor3->SetScale(0.1f, 0.1f);
	Renderer::GetCanvasLayer(0)->OnDetach(actor3.get());

	std::shared_ptr<ActorTesselation2D> tessel(std::make_shared<ActorTesselation2D>(std::static_pointer_cast<ActorRenderBase2D>(actor3)));
	Renderer::GetCanvasLayer(0)->OnAttach(tessel.get());

	float w = actor3->GetWidth() * actor3->GetScale().x;
	float h = actor3->GetHeight() * actor3->GetScale().y;
	tessel->PushBackGlobals({
		std::make_shared<Transformable2D>(Transformable2D({ {0.0f, 0.0f}, 0.0f, {1.0f, 1.0f} })),
		std::make_shared<Transformable2D>(Transformable2D({ {w, 0.0f}, 0.0f, {1.0f, 1.0f} })),
		std::make_shared<Transformable2D>(Transformable2D({ {2.0f * w, 0.0f}, 0.5f, {1.0f, 1.0f} }))
	});
	Renderer::GetCanvasLayer(0)->OnSetZIndex(tessel.get(), 10);

	ActorPrimitive2D* const actor_ref = dynamic_cast<ActorPrimitive2D* const>(tessel->ActorRef().get());
	if (actor_ref)
	{
		actor_ref->SetModulationPerPoint({
			{1.0f, 1.0f, 1.0f, 1.0f},
			{1.0f, 1.0f, 1.0f, 1.0f},
			{0.0f, 0.0f, 0.0f, 0.0f},
			{0.0f, 0.0f, 0.0f, 0.0f},
			});
	}

	std::shared_ptr<ActorTesselation2D> tesselVertical(std::make_shared<ActorTesselation2D>(ActorTesselation2D(tessel)));
	Renderer::GetCanvasLayer(0)->OnDetach(tessel.get());
	Renderer::GetCanvasLayer(0)->OnAttach(tesselVertical.get());
	tesselVertical->PushBackGlobals({
		std::shared_ptr<Transformable2D>(new Transformable2D({ {0.0f, 0}, 0.0f, {1.0f, 1.0f} })),
		std::shared_ptr<Transformable2D>(new Transformable2D({ {0.0f, -h}, 0.0f, {1.0f, 1.0f} })),
		std::shared_ptr<Transformable2D>(new Transformable2D({ {0.0f, -2 * h}, 0.5f, {1.0f, 1.0f}}))
	});
	
	std::shared_ptr<ActorTesselation2D> tesselDiagonal(std::make_shared<ActorTesselation2D>(ActorTesselation2D(tesselVertical)));
	Renderer::GetCanvasLayer(0)->OnDetach(tesselVertical.get());
	Renderer::GetCanvasLayer(0)->OnAttach(tesselDiagonal.get());
	tesselDiagonal->PushBackGlobals({
		std::shared_ptr<Transformable2D>(new Transformable2D({ {0.0f, 0.0f}, 0.0f, {1.0f, 1.0f} })),
		std::shared_ptr<Transformable2D>(new Transformable2D({ {-0.5f * w, 0.5f * h}, -0.8f, {0.75f, -0.75f} })),
	});

	actor2->CropToRelativeRect({ 0.3f, 0.4f, 0.4f, 0.55f });

	Renderer::RemoveCanvasLayer(0);
	Renderer::RemoveCanvasLayer(-1);

	const float mt = 0.5f;
	const float imt = 1.0f / (1.0f - mt);
	ParticleWaveData<> wave1{
		1.5f,
		std::shared_ptr<DebugPolygon>(new DebugCircle(4.0f)),
		CumulativeFunc<>([](float t) { return t < 0.6f ? PowerFunc(2000.0f, 0.5f)(t) : PowerFunc(2000.0f, 0.5f)(0.6f); }),
		[](const Particles::CHRSeed& seed) { return 0.4f - seed.waveT * 0.05f; },
		Particles::CombineSequential({
			[mt, imt](const Particles::CHRSeed& seed)
			{
				return Particles::CHRBind{
					[mt, imt](Particle& p) {
						if (p.t() < mt)
							p.m_Shape->SetColor({ 1.0f, p.t(), 0.0f, 1.0f });
						else
							p.m_Shape->SetColor({ 1.0f, p.t(), 0.0f, 1.0f - (p.t() - mt) * imt });
					}, 0
				};
			},
			Particles::CombineInitialOverTime(
				[](const Particles::CHRSeed& seed)
				{
					return Particles::CHRBind{
						[&seed](Particle& p)
						{
							p.data().s[0] = rng();
							p.m_Transformer.SetLocalPosition(glm::vec2{ glm::cos(p.data().s[0] * 2 * glm::pi<float>()), glm::sin(p.data().s[0] * 2 * glm::pi<float>()) } * 20.0f * rng());
							glm::vec2 vel = glm::normalize(glm::vec2{ rng() * 2.0f - 1.0f, rng() * 2.0f - 1.0f }) * glm::vec2{ 200.0f, 100.0f };
							p.data().s[0] = rng();
							vel *= (1.0f + p.data().s[0] * 0.25f) * (1.0f - 0.4f * glm::pow(seed.waveT, 0.2f + p.data().s[0] * 0.25f));
							p.data().s[0] = vel.x;
							p.data().s[1] = vel.y;
						}, 2
					};
				},
				[](const Particles::CHRSeed& seed)
				{
					return Particles::CHRBind{
						[](Particle& p)
						{
							p.m_Transformer.OperateLocalPosition([&](glm::vec2& pos) { pos += glm::vec2{ p.data().s[0], p.data().s[1] } * p.dt(); });
						}, 2
					};
				}
			)
		})
	};
	float p2width = 400.0f;
	float p2height = 400.0f;
	ParticleWaveData<> wave2{
		1.5f,
		std::shared_ptr<DebugPolygon>(new DebugPolygon({ {0, 0}, {0, 3}, {1, 3}, {1, 0} }, {}, {}, GL_TRIANGLE_FAN)),
		CumulativeFunc<>(LinearFunc(p2height * 0.5f)),
		[](const Particles::CHRSeed& seed) { return 1.5f; },
		[p2width, p2height](const Particles::CHRSeed& seed)
		{
			return Particles::CHRBind{
				[p2width, p2height, &seed](Particle& p) {
					if (p.t() == 0.0f)
					{
						p.data().s[0] = rng();
						p.data().s[1] = static_cast<float>(std::rand() % seed.totalSpawn);
						p.data().s[2] = std::rand() % 2 == 0 ? 1 : -1;
					}
					real mt = std::fmod(p.t() + p.data().s[0], 1.0f);
					if (mt < 0.5f)
						p.m_Shape->SetColor({ 2.0f * mt, 2.0f * mt, 1.0f, 0.4f });
					else
						p.m_Shape->SetColor({ 2.0f * (1.0f - mt), 2.0f * (1.0f - mt), 1.0f, 0.4f });
					if (mt < 0.5f)
						p.m_Transformer.SetLocalScale({ 1.0f + 2.0f * mt * p2width, 1.0f });
					else
						p.m_Transformer.SetLocalScale({ 1.0f + 2.0f * (1.0f - mt) * p2width, 1.0f});
					if (p.data().s[2] > 0)
					{
						if (mt < 0.5f)
							p.m_Transformer.SetLocalPosition({ -0.5f * p2width, 2.0f * static_cast<unsigned int>(p.data().s[1]) - 0.5f * p2height });
						else
							p.m_Transformer.SetLocalPosition({ (p2width + 1.0f) * (mt - 0.5f) * 2.0f - 0.5f * p2width, 2.0f * static_cast<unsigned int>(p.data().s[1]) - 0.5f * p2height });
					}
					else
					{
						if (mt < 0.5f)
							p.m_Transformer.SetLocalPosition({ -(p2width - 1.0f) * (mt - 0.5f) * 2.0f - 0.5f * p2width, 2.0f * static_cast<unsigned int>(p.data().s[1]) - 0.5f * p2height });
						else
							p.m_Transformer.SetLocalPosition({ -0.5f * p2width, 2.0f * static_cast<unsigned int>(p.data().s[1]) - 0.5f * p2height});
					}
					p.m_Transformer.SyncGlobalWithLocal();
				}, 3
			};
		}
	};

	//ParticleSystem<> psys({ wave1, wave2 });
	ParticleSystem<> psys({ wave1 });

	Renderer::AddCanvasLayer(11);
	Renderer::GetCanvasLayer(11)->OnAttach(&psys);
	Logger::NewLine();

	psys.Pause();

	std::shared_ptr<TileMap> tilemap;
	if (loadTileMap("res/assets/tilemap.toml", tilemap) != LOAD_STATUS::OK)
		ASSERT(false);

	tilemap->SetTransform({ {100.0f, 200.0f}, 0.3f, { 5.0f, 8.0f } });
	tilemap->Insert(4, 0, 1);

	//Renderer::GetCanvasLayer(11)->OnAttach(tilemap.get());
	//Renderer::GetCanvasLayer(11)->OnAttach(tesselDiagonal.get());
	tesselDiagonal->SetPosition(-400, 300);
	tessel->SetScale(0.6, 0.6);
	actor3->OperateScale([](glm::vec2& scale) { scale *= 2.0f; });

	for (;;)
	{
		drawTime = static_cast<real>(glfwGetTime());
		deltaDrawTime = drawTime - prevDrawTime;
		prevDrawTime = drawTime;
		totalDrawTime += deltaDrawTime;
		// small delay for smoother window init
		if (totalDrawTime > 0.3f)
		{
			// OnUpdate here
			psys.Resume();
			psys.SetRotation(totalDrawTime * 0.25f);
			psys.SetScale(1.0f - 0.2f * glm::sin(totalDrawTime), 1.0f + 0.2f * glm::sin(totalDrawTime));
		}

		Renderer::OnDraw();
		glfwPollEvents();
		if (glfwWindowShouldClose(window))
			break;
	}
}
