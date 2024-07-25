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
#include "render/actors/shapes/particles/ParticleSubsystemArray.h"
#include "render/actors/shapes/particles/Characteristics.h"

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
	ParticleSubsystemData<> wave1{
		1.5f,
		std::shared_ptr<DebugPolygon>(new DebugCircle(4.0f)),
		CumulativeFunc<>([](float t) { return t < 0.6f ? PowerFunc(2000.0f, 0.5f)(t) : PowerFunc(2000.0f, 0.5f)(0.6f); }),
		[](const Particles::CHRSeed& seed) { return 0.4f - seed.waveT * 0.05f; },
		Particles::CombineSequential({
			Particles::CombineConditionalTimeLessThan(
				mt,
				Particles::CHR::SetColorUsingTime(LinearCombo4x1({ 1.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 0.0f })),
				Particles::CHR::SetColorUsingTime(LinearCombo4x1({ 1.0f, 0.0f, 0.0f, 1.0f + mt / (1.0f - mt) }, { 0.0f, 1.0f, 0.0f, -1.0f / (1.0f - mt)}))
			),
			Particles::CombineInitialOverTime(
				Particles::CombineSequential({
					Particles::CHR::FeedDataRNG(0),
					Particles::CHR::FeedDataRNG(1),
					[](const Particles::CHRSeed& seed)
					{
						return Particles::CHRBind{
							[](Particle& p)
							{
								p.m_Transformer.SetLocalPosition(glm::vec2{ glm::cos(p[0] * 2 * glm::pi<float>()), glm::sin(p[0] * 2 * glm::pi<float>()) } * 20.0f * p[1]);
							}, 1
						};
					},
					Particles::CHR::FeedDataRNG(0),
					Particles::CHR::FeedDataRNG(1),
					[](const Particles::CHRSeed& seed)
					{
						return Particles::CHRBind{
							[](Particle& p)
							{
								glm::vec2 vel = LinearCombo2x2({ -1.0f, -1.0f }, { 2.0f, 0.0f }, { 0.0f, 2.0f })({p[0], p[1]});
								vel = glm::normalize(vel);
								vel *= glm::vec2{ 200.0f, 100.0f };
								p[0] = vel.x;
								p[1] = vel.y;
							}, 2
						};
					},
					Particles::CHR::FeedDataRNG(2),
					[](const Particles::CHRSeed& seed)
					{
						return Particles::CHRBind{
							[&seed](Particle& p)
							{
								p[2] = (1.0f + p[2] * 0.25f) * (1.0f - 0.4f * glm::pow(seed.waveT, 0.2f + p[2] * 0.25f));
							}, 3
						};
					},
					Particles::CHR::OperateData(0, OperateFloatMultWrap, 2, 1, OperateFloatMultWrap, 2)
				}),
				Particles::CHR::OperateLocalPositionFromVelocityData(0, 1)
			)
		})
	};
	float p2width = 400.0f;
	float p2height = 400.0f;
	ParticleSubsystemData<> wave2{
		1.5f,
		std::shared_ptr<DebugPolygon>(new DebugPolygon({ {0, 0}, {0, 3}, {1, 3}, {1, 0} }, {}, {}, GL_TRIANGLE_FAN)),
		CumulativeFunc<>(LinearFunc(p2height * 0.5f)),
		[](const Particles::CHRSeed& seed) { return 1.5f; },
		Particles::CombineSequential({
			Particles::GenSetup(
				Particles::CombineSequential({
					Particles::CHR::FeedDataRNG(0),
					Particles::CHR::FeedDataRandSpawnIndex(1),
					Particles::CHR::FeedDataRandBinChoice(2, 1, -1)
				})
			),
			Particles::CHR::FeedDataShiftMod(3, 0, 1.0f),
			Particles::CombineConditionalDataLessThan(
				3, 0.5f,
				Particles::CombineSequential({
					Particles::CHR::SetColorUsingData(LinearCombo4x1({ 0.0f, 0.0f, 1.0f, 1.0f }, { 2.0f, 2.0f, 0.0f, 0.0f }), 3),
					Particles::CHR::SetLocalScaleUsingData(LinearCombo2x1({ 1.0f, 1.0f }, { 2.0f * p2width, 0.0f }), 3),
					Particles::CombineConditionalDataLessThan(
						2, 0.0f,
						Particles::CHR::SetLocalPositionUsingData(Composition(LinearCombo2x1({ -0.5f * p2width, -0.5f * p2height}, { 0.0f, 2.0f }), CastFloatToUInt), 1),
						Particles::CHR::SetLocalPositionUsingData(Composition(LinearCombo2x2({ 0.5f * p2width - 1.0f, -0.5f * p2height }, { -2.0f * (p2width - 1.0f), 0.0f }, { 0.0f, 2.0f }), Vec2Wrap(Identity, CastFloatToUInt)), 3, 1)
					)
				}),
				Particles::CombineSequential({
					Particles::CHR::SetColorUsingData(LinearCombo4x1({ 2.0f, 2.0f, 1.0f, 1.0f }, { -2.0f, -2.0f, 0.0f, 0.0f }), 3),
					Particles::CHR::SetLocalScaleUsingData(LinearCombo2x1({ 1.0f + 2.0f * p2width, 1.0f }, { -2.0f * p2width, 0.0f }), 3),
					Particles::CombineConditionalDataLessThan(
						2, 0.0f,
						Particles::CHR::SetLocalPositionUsingData(Composition(LinearCombo2x2({ -1.5f * p2width - 1.0f, -0.5f * p2height }, { 2.0f * (p2width + 1.0f), 0.0f }, { 0.0f, 2.0f }), Vec2Wrap(Identity, CastFloatToUInt)), 3, 1),
						Particles::CHR::SetLocalPositionUsingData(Composition(LinearCombo2x1({ -0.5f * p2width, -0.5f * p2height}, { 0.0f, 2.0f }), CastFloatToUInt), 1)
					)
				})
			),
			Particles::CHR::SyncGlobalWithLocal
		})
	};

	ParticleSystem<> psys({ wave1, wave2 });
	ParticleSubsystemArray<> parr({ wave1, wave2 });

	psys.SetPosition(-400, 0);
	parr.SetPosition(400, 0);
	
	psys.Pause();
	parr.Pause();

	Renderer::AddCanvasLayer(11);
	Renderer::GetCanvasLayer(11)->OnAttach(&psys);
	Renderer::GetCanvasLayer(11)->OnAttach(&parr);

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
			//psys.SetRotation(totalDrawTime * 0.25f);
			//psys.SetScale(1.0f - 0.2f * glm::sin(totalDrawTime), 1.0f + 0.2f * glm::sin(totalDrawTime));
			parr.Resume();
		}

		Renderer::OnDraw();
		glfwPollEvents();
		if (glfwWindowShouldClose(window))
			break;
	}
}
