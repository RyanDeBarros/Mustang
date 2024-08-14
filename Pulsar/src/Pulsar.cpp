#include "Pulsar.h"

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
#include "render/actors/shapes/DebugRect.h"
#include "render/actors/shapes/DebugBatcher.h"
#include "render/actors/particles/ParticleSystem.h"
#include "render/actors/particles/ParticleSubsystemArray.h"
#include "render/actors/particles/Characteristics.h"
#include "utils/CommonMath.h"
#include "utils/Functors.h"
#include "utils/Strings.h"

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
	GLFWwindow* window = nullptr;
	int startup = Pulsar::StartUp(window);
	Run(window);
	Pulsar::Terminate();
	return startup;
}

int Pulsar::StartUp(GLFWwindow*& window)
{
	if (!Loader::_LoadRendererSettings())
		return -1;
	if (!glfwInit())
		return -1;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	window = glfwCreateWindow((int)_RendererSettings::initial_window_width, (int)_RendererSettings::initial_window_height, "Pulsar Renderer", nullptr, nullptr);
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
	TRY(Logger::LogInfo(reinterpret_cast<const char*>(glGetString(GL_VERSION))));
	std::srand(static_cast<unsigned int>(time(0)));
	Renderer::Init();
	Renderer::FocusWindow(window);
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
	if (Loader::loadTexture("res/assets/snowman.toml", textureSnowman) != LOAD_STATUS::OK)
		ASSERT(false);
	if (Loader::loadTexture("res/assets/tux.toml", textureTux) != LOAD_STATUS::OK)
		ASSERT(false);
	if (Loader::loadTexture("res/assets/flag.toml", textureFlag, true) != LOAD_STATUS::OK)
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
	RectRender actor1(textureFlag, { {-500.0f, 300.0f}, -1.0f, {0.8f, 1.2f} });
	RectRender actor2(textureSnowman, { {400.0f, -200.0f}, 0.25f, {0.7f, 0.7f} });
	RectRender actor3(textureTux, { {0.0f, 0.0f}, 0.0f, {1.0f, 1.0f} });

	Renderer::GetCanvasLayer(0)->OnAttach(&actor1);
	Renderer::GetCanvasLayer(0)->OnAttach(&actor2);
	Renderer::GetCanvasLayer(0)->OnSetZIndex(&actor1, 1);
	Renderer::GetCanvasLayer(0)->OnAttach(&actor3);
	Renderer::AddCanvasLayer(-1);

	Renderable renderable;
	if (Loader::loadRenderable("res/assets/renderable.toml", renderable, true) != LOAD_STATUS::OK)
		ASSERT(false);
	std::shared_ptr<ActorPrimitive2D> actor4(new ActorPrimitive2D(renderable, { {-200.0f, 0.0f}, 0.0f, {800.0f, 800.0f} }));
	Renderer::GetCanvasLayer(-1)->OnAttach(actor4.get());

	actor1.RefTransform()->scale = { 16.0f, 16.0f };
	actor1.FlagTransformRS();
	Renderer::GetCanvasLayer(0)->OnSetZIndex(&actor3, -1);

	actor3.SetPivot(0.0f, 0.0f);
	actor3.RefTransform()->position = _RendererSettings::initial_window_rel_pos(-0.5f, 0.5f);
	actor3.RefTransform()->scale = { 0.3f, 0.3f };
	actor3.FlagTransform();
	actor2.SetModulation(glm::vec4(0.7f, 0.7f, 1.0f, 1.0f));
	actor3.SetModulationPerPoint({
		glm::vec4(1.0f, 0.5f, 0.5f, 1.0f),
		glm::vec4(0.5f, 1.0f, 0.5f, 1.0f),
		glm::vec4(0.5f, 0.5f, 1.0f, 1.0f),
		glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)
		});

	TextureFactory::SetSettings(actor1.GetTextureHandle(), Texture::linear_settings);

	actor3.SetPivot(0.5f, 0.5f);
	actor3.RefTransform()->position = { 0.0f, 0.0f };
	actor3.RefTransform()->scale = { 0.1f, 0.1f };
	actor3.FlagTransform();
	Renderer::GetCanvasLayer(0)->OnDetach(&actor3);

	actor2.CropToRelativeRect({ 0.3f, 0.4f, 0.4f, 0.55f });

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
								p.m_Shape->Transformer()->self->position = glm::vec2{ glm::cos(p[0] * 2 * glm::pi<float>()), glm::sin(p[0] * 2 * glm::pi<float>()) } *20.0f * p[1];
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
			),
			Particles::CHR::SyncAll
		})
	};
	float p2width = 400.0f;
	float p2height = 400.0f;
	ParticleSubsystemData<> wave2{
		1.5f,
		std::shared_ptr<DebugPolygon>(new DebugPolygon({ {0, 0}, {0, 2}, {1, 2}, {1, 0} }, {}, Colors::WHITE, GL_TRIANGLE_FAN)),
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
					Particles::CHR::SetLocalScaleUsingData(LinearCombo2x1({ 0.0f, 1.0f }, { 2.0f * p2width, 0.0f }), 3),
					Particles::CombineConditionalDataLessThan(
						2, 0.0f,
						Particles::CHR::SetLocalPositionUsingData(Composition(LinearCombo2x1({ -0.5f * p2width, -0.5f * p2height}, { 0.0f, 2.0f }), CastFloatToUInt), 1),
						Particles::CHR::SetLocalPositionUsingData(Composition(LinearCombo2x2({ 0.5f * p2width, -0.5f * p2height }, { -2.0f * p2width, 0.0f }, { 0.0f, 2.0f }), Vec2Wrap(Identity, CastFloatToUInt)), 3, 1)
					)
				}),
				Particles::CombineSequential({
					Particles::CHR::SetColorUsingData(LinearCombo4x1({ 2.0f, 2.0f, 1.0f, 1.0f }, { -2.0f, -2.0f, 0.0f, 0.0f }), 3),
					Particles::CHR::SetLocalScaleUsingData(LinearCombo2x1({ 2.0f * p2width, 1.0f }, { -2.0f * p2width, 0.0f }), 3),
					Particles::CombineConditionalDataLessThan(
						2, 0.0f,
						Particles::CHR::SetLocalPositionUsingData(Composition(LinearCombo2x2({ -1.5f * p2width, -0.5f * p2height }, { 2.0f * p2width, 0.0f }, { 0.0f, 2.0f }), Vec2Wrap(Identity, CastFloatToUInt)), 3, 1),
						Particles::CHR::SetLocalPositionUsingData(Composition(LinearCombo2x1({ -0.5f * p2width, -0.5f * p2height}, { 0.0f, 2.0f }), CastFloatToUInt), 1)
					)
				})
			),
			Particles::CHR::SyncAll
		})
	};

	//ParticleSystem<> psys({ wave2, wave2, wave1 });
	//ParticleSystem<> psys({ wave1 });
	ParticleSystem<> psys({ wave2, wave2 });
	psys.SubsystemRef(1).Transformer()->self->rotation = 0.5f * glm::pi<float>();
	psys.SubsystemRef(1).Transformer()->SyncRS();
	//psys.Transformer()->SetLocalScale(1, { 1, _RendererSettings::initial_window_width / static_cast<float>(_RendererSettings::initial_window_height) });
	psys.TransformerRef()->self->scale = glm::vec2{ _RendererSettings::initial_window_width / p2width, _RendererSettings::initial_window_height / p2height } *0.75f;
	psys.TransformerRef()->SyncRS();
	//psys.Transformer()->SyncGlobalWithLocalScales();
	//psys.Transformer()->SetPosition(300, 0);
	//psys.Transformer()->SetLocalTransform(2, { {-500, 0}, 0, {2, 2} });
	//ParticleSubsystemArray<> parr({ wave1, wave2 });

	//psys.SetPosition(-400, 0);
	//psys.Transformer().SyncGlobalWithLocalPositions();
	//parr.SetPosition(400, 0);
	//parr.Transformer().SyncGlobalWithLocalPositions();

	//parr.SubsystemRef(0).SetRotation(1.57f);
	*psys.ModulateRef() = Colors::PSYCHEDELIC_PURPLE;
	psys.ModulatorRef()->Sync();

	Renderer::AddCanvasLayer(11);
	Renderer::GetCanvasLayer(11)->OnAttach(&psys);
	//Renderer::GetCanvasLayer(11)->OnAttach(&parr);

	std::shared_ptr<TileMap> tilemap;
	if (Loader::loadTileMap("res/assets/tilemap.toml", tilemap) != LOAD_STATUS::OK)
		ASSERT(false);

	tilemap->Transformer()->self.transform = { {100.0f, 200.0f}, 0.3f, {5.0f, 8.0f} };
	tilemap->Transformer()->Sync();
	tilemap->Insert(4, 0, 1);
	Renderer::GetCanvasLayer(11)->OnAttach(tilemap.get());

	actor3.RefTransform()->scale *= 2.0f;
	actor3.RefTransformer()->SyncRS();

	TextureFactory::SetSettings(textureFlag, Texture::nearest_settings);

	RectRender root(textureFlag);
	RectRender child(textureSnowman);
	RectRender grandchild(textureTux);
	RectRender child2(textureSnowman);
	RectRender grandchild2(textureTux);

	child.RefTransformer()->Attach(grandchild.RefTransformer());
	grandchild.RefTransform()->scale = { 0.25f, 0.25f };
	grandchild.RefTransform()->position = { 300.0f, -100.0f };
	child.RefTransform()->scale = { 0.25f, 0.25f };
	child.RefTransformer()->Sync();
	child2.RefTransformer()->Attach(grandchild2.RefTransformer());
	grandchild2.RefTransform()->scale = { 0.25f, 0.25f };
	grandchild2.RefTransform()->position = { 300.0f, -100.0f };
	child2.RefTransform()->scale = { 0.25f, 0.25f };
	child2.RefTransformer()->Sync();

	root.RefTransformer()->Attach(child.RefTransformer());
	root.RefTransformer()->Attach(child2.RefTransformer());
	root.RefTransform()->scale = { 10.0f, 10.0f };
	root.RefTransformer()->Sync();
	for (const auto& child : root.RefTransformer()->children)
	{
		child->self->position = { 0.0f, -30.0f };
		child->self->scale *= 0.2f;
		child->Sync();
	}

	//std::shared_ptr<Modulator> first_mod(std::make_shared<Modulator>(child.ModulateWeak(), grandchild.ModulateWeak()));
	//first_mod->SetColor({ 1.0f, 1.0f, 0.0f, 1.0f });
	//first_mod->SetGlobalColor({ 1.0f, 0.0f, 1.0f, 1.0f });
	//std::shared_ptr<Modulator> root_mod(std::make_shared<Modulator>(root.ModulateWeak(), first_mod));
	//root_mod->SetColor(glm::vec4{ 1.0f } * 0.5f);

	Renderer::GetCanvasLayer(11)->OnAttach(&root);
	Renderer::GetCanvasLayer(11)->OnAttach(&child2);
	Renderer::GetCanvasLayer(11)->OnAttach(&grandchild2);
	Renderer::GetCanvasLayer(11)->OnAttach(&child);
	Renderer::GetCanvasLayer(11)->OnAttach(&grandchild);

	DebugRect rect(_RendererSettings::initial_window_width * 0.5f, _RendererSettings::initial_window_height, true, { 1.0f, 0.5f }, {}, { 0.5f, 0.5f, 1.0f, 0.3f }, 1);
	Renderer::GetCanvasLayer(11)->OnAttach(&rect);

	RectRender tux(textureTux);
	ActorTesselation2D tuxTessel(&tux);
	ActorTesselation2D tuxGrid(&tuxTessel);
	Renderer::GetCanvasLayer(11)->OnAttach(&tuxGrid);

	tux.RefTransform()->scale *= 0.1f;
	tux.RefTransformer()->SyncRS();

	tuxTessel.PushBackStatic({
		{ {0, 100} },
		{ {200, 0} },
		{ {-200, 0} }
	});

	tuxGrid.PushBackStatic({
		{ {50, 0} },
		{ {50, 200} },
		{ {50, -200} }
	});

	tux.SetModulationPerPoint({ Colors::WHITE, Colors::BLUE, Colors::TRANSPARENT, Colors::LIGHT_GREEN });

	for (;;)
	{
		drawTime = static_cast<real>(glfwGetTime());
		deltaDrawTime = drawTime - prevDrawTime;
		prevDrawTime = drawTime;
		totalDrawTime += deltaDrawTime;
		
		child.RefTransform()->rotation = -Pulsar::totalDrawTime;
		child2.RefTransform()->rotation = -Pulsar::totalDrawTime;
		grandchild.RefTransform()->rotation = Pulsar::totalDrawTime;
		grandchild2.RefTransform()->rotation = -Pulsar::totalDrawTime;
		root.RefTransform()->rotation = Pulsar::totalDrawTime;
		child.RefTransform()->position += 5 * Pulsar::deltaDrawTime;
		child2.RefTransform()->scale *= 1.0f / (1.0f + 0.1f * Pulsar::deltaDrawTime);
		root.RefTransformer()->Sync();

		Renderer::OnDraw();
		glfwPollEvents();
		if (glfwWindowShouldClose(window))
			break;
	}
}
