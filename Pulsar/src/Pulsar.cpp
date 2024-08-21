#include "Pulsar.h"

#include <glm/ext/scalar_constants.hpp>

#include "Typedefs.h"
#include "RendererSettings.h"
#include "Logger.inl"
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
#include "utils/Functor.inl"
#include "utils/Functions.inl"
#include "utils/Strings.h"
#include "utils/Data.h"
#include "utils/Constants.h"

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
	// Also update frame in callback. Currently, animation is paused when resizing.
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
	TextureHandle tex_dirtTL = TextureFactory::GetHandle({ "res/textures/dirtTL.png" });
	TextureHandle tex_dirtTR = TextureFactory::GetHandle({ "res/textures/dirtTR.png" });
	TextureHandle tex_grassSingle = TextureFactory::GetHandle({ "res/textures/grassSingle.png" });
	TextureHandle tex_grassTL = TextureFactory::GetHandle({ "res/textures/grassTL.png" });
	TextureHandle tex_grassTE = TextureFactory::GetHandle({ "res/textures/grassTE.png" });
	TextureHandle tex_grassTR = TextureFactory::GetHandle({ "res/textures/grassTR.png" });

	Renderer::AddCanvasLayer(0);

	// Create actors
	RectRender actor1(textureFlag);
	set_ptr(actor1.Fickler().Transform(), { {-500.0f, 300.0f}, -1.0f, {0.8f, 1.2f} });
	actor1.Fickler().SyncT();
	RectRender actor2(textureSnowman);
	set_ptr(actor2.Fickler().Transform(), { {400.0f, -200.0f}, 0.25f, {0.7f, 0.7f} });
	actor2.Fickler().SyncT();
	RectRender actor3(textureTux);
	set_ptr(actor3.Fickler().Transform(), { {0.0f, 0.0f}, 0.0f, {1.0f, 1.0f} });
	actor3.Fickler().SyncT();

	Renderer::GetCanvasLayer(0)->OnAttach(&actor1);
	Renderer::GetCanvasLayer(0)->OnAttach(&actor2);
	Renderer::GetCanvasLayer(0)->OnSetZIndex(&actor1, 1);
	Renderer::GetCanvasLayer(0)->OnAttach(&actor3);
	Renderer::AddCanvasLayer(-1);

	Renderable renderable;
	if (Loader::loadRenderable("res/assets/renderable.toml", renderable, true) != LOAD_STATUS::OK)
		ASSERT(false);
	ActorPrimitive2D actor4(renderable);
	set_ptr(actor4.Fickler().Transform(), { {-200.0f, 0.0f}, 0.0f, {800.0f, 800.0f} });
	actor4.Fickler().SyncT();
	Renderer::GetCanvasLayer(-1)->OnAttach(&actor4);

	set_ptr(actor1.Fickler().Scale(), {16.0f, 16.0f});
	actor1.Fickler().SyncRS();
	Renderer::GetCanvasLayer(0)->OnSetZIndex(&actor3, -1);

	actor3.SetPivot(0.0f, 0.0f);
	set_ptr(actor3.Fickler().Position(), _RendererSettings::initial_window_rel_pos(-0.5f, 0.5f));
	set_ptr(actor3.Fickler().Scale(), { 0.3f, 0.3f });
	actor3.Fickler().SyncT();
	actor2.SetModulation(glm::vec4(0.7f, 0.7f, 1.0f, 1.0f));
	actor3.SetModulationPerPoint({
		glm::vec4(1.0f, 0.5f, 0.5f, 1.0f),
		glm::vec4(0.5f, 1.0f, 0.5f, 1.0f),
		glm::vec4(0.5f, 0.5f, 1.0f, 1.0f),
		glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)
		});

	TextureFactory::SetSettings(actor1.GetTextureHandle(), Texture::linear_settings);

	actor3.SetPivot(0.5f, 0.5f);
	set_ptr(actor3.Fickler().Position(), { 0.0f, 0.0f });
	set_ptr(actor3.Fickler().Scale(), { 0.1f, 0.1f });
	actor3.Fickler().SyncT();
	Renderer::GetCanvasLayer(0)->OnDetach(&actor3);

	actor2.CropToRelativeRect({ 0.3f, 0.4f, 0.4f, 0.55f });

	Renderer::RemoveCanvasLayer(0);
	Renderer::RemoveCanvasLayer(-1);

	const float mt = 0.5f;
	ParticleSubsystemData<> wave1{
		1.5f,
		std::shared_ptr<DebugPolygon>(new DebugCircle(4.0f)),
		CumulativeFunc<>(func_conditional<float, float>(
			make_functor_ptr<true>(&less_than, 0.6f),
			make_functor_ptr(&power_func, std::tuple<float, float>{ 2000.0f, 0.5f }),
			make_functor_ptr<true>(&constant_func<float>, power_func(0.6f, { 2000.0f, 0.5f })))
		),
		func_compose<float, const Particles::CHRSeed&>(Particles::CHR::Seed_waveT(), make_functor_ptr(&linear_func, std::tuple(-0.05f, 0.4f))),
		Particles::CombineSequential({
			Particles::CombineConditionalTimeLessThan(mt,
				Particles::CHR::SetColorUsingTime(make_functor_ptr(&linear_combo_f<4>, std::array<glm::vec4, 2>{
						glm::vec4{ 1.0f, 0.0f, 0.0f, 1.0f }, glm::vec4{ 0.0f, 1.0f, 0.0f, 0.0f } })),
				Particles::CHR::SetColorUsingTime(make_functor_ptr(&linear_combo_f<4>, std::array<glm::vec4, 2>{
						glm::vec4{ 1.0f, 0.0f, 0.0f, 1.0f + mt / (1.0f - mt) }, glm::vec4{ 0.0f, 1.0f, 0.0f, -1.0f / (1.0f - mt)} }))
			),
			Particles::CombineInitialOverTime(
				Particles::CombineSequential({
					Particles::CHR::FeedDataRNG(0),
					Particles::CHR::FeedDataRNG(1),
					Particles::CHR::SetLocalPositionUsingData(make_functor_ptr([](const glm::vec2& ps) -> Position2D {
						return glm::vec2{ glm::cos(ps.x * 2 * glm::pi<float>()), glm::sin(ps.x * 2 * glm::pi<float>()) } * 20.0f * ps.y;
					}), 0, 1),
					Particles::CHR::FeedDataRNG(0),
					Particles::CHR::FeedDataRNG(1),
					Particles::Characterize(make_functor_ptr([](Particle& p) {
						glm::vec2 vel = linear_combo<2, 2>({ p[0], p[1] }, { glm::vec2{ -1.0f, -1.0f }, glm::vec2{ 2.0f, 0.0f }, glm::vec2{ 0.0f, 2.0f } });
						vel = glm::normalize(vel);
						vel *= glm::vec2{ 200.0f, 100.0f };
						p[0] = vel.x;
						p[1] = vel.y;
					}), 2),
					Particles::CHR::FeedDataRNG(2),
					Particles::CHR::OperateDataUsingSeed(2, make_functor_ptr([](const std::tuple<float&, const Particles::CHRSeed&>& tuple) -> void {
						std::get<0>(tuple) = (1.0f + std::get<0>(tuple) * 0.25f) * (1.0f - 0.4f * glm::pow(std::get<1>(tuple).waveT, 0.2f + std::get<0>(tuple) * 0.25f));
					})),
					Particles::CHR::OperateData(0, make_functor_ptr([](const std::tuple<float&, float>& tuple) { std::get<0>(tuple) *= std::get<1>(tuple); }), 2),
					Particles::CHR::OperateData(1, make_functor_ptr([](const std::tuple<float&, float>& tuple) { std::get<0>(tuple) *= std::get<1>(tuple); }), 2),
				}),
				Particles::CHR::OperateLocalPositionFromVelocityData(0, 1)
			),
			Particles::CHR::SyncAll()
		})
	};
	float p2width = 400.0f;
	float p2height = 400.0f;
	ParticleSubsystemData<> wave2{
		1.5f,
		std::shared_ptr<DebugPolygon>(new DebugPolygon({ {0, 0}, {0, 2}, {1, 2}, {1, 0} }, GL_TRIANGLE_FAN)),
		CumulativeFunc<>(make_functor_ptr(&linear_func, std::tuple<float, float>{ p2height * 0.5f, 0.0f })),
		make_functor_ptr<true>(&constant_func<float, const Particles::CHRSeed&>, 1.5f),
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
					Particles::CHR::SetColorUsingData(make_functor_ptr(&linear_combo_f<4>, std::array<glm::vec4, 2>{
							glm::vec4{0.0f, 0.0f, 1.0f, 1.0f}, glm::vec4{2.0f, 2.0f, 0.0f, 0.0f} }), 3),
					Particles::CHR::SetLocalScaleUsingData(make_functor_ptr(&linear_combo_f<2>, std::array<glm::vec2, 2>{
							glm::vec2{ 0.0f, 1.0f }, glm::vec2{ 2.0f * p2width, 0.0f } }), 3),
					Particles::CombineConditionalDataLessThan(
						2, 0.0f,
						Particles::CHR::SetLocalPositionUsingData(func_compose<Position2D, float>(
							make_functor_ptr(&cast<float, unsigned int>),
							make_functor_ptr(&linear_combo_f<2>, std::array<glm::vec2, 2>{
									glm::vec2{ -0.5f * p2width, -0.5f * p2height}, glm::vec2{ 0.0f, 2.0f } })), 1),
						Particles::CHR::SetLocalPositionUsingData(func_compose<Position2D, const glm::vec2&>(
							vec2_compwise<float, float>(make_functor_ptr(&identity<float>), make_functor_ptr(&cast<float, unsigned int>)),
							make_functor_ptr(&linear_combo<2, 2>, std::array<glm::vec2, 3>{
									glm::vec2{ 0.5f * p2width, -0.5f * p2height }, glm::vec2{-2.0f * p2width, 0.0f}, glm::vec2{0.0f, 2.0f} })), 3, 1)
					)
				}),
				Particles::CombineSequential({
					Particles::CHR::SetColorUsingData(make_functor_ptr(&linear_combo_f<4>, std::array<glm::vec4, 2>{
							glm::vec4{2.0f, 2.0f, 1.0f, 1.0f}, glm::vec4{-2.0f, -2.0f, 0.0f, 0.0f} }), 3),
					Particles::CHR::SetLocalScaleUsingData(make_functor_ptr(&linear_combo_f<2>, std::array<glm::vec2, 2>{
							glm::vec2{ 2.0f * p2width, 1.0f }, glm::vec2{ -2.0f * p2width, 0.0f } }), 3),
					Particles::CombineConditionalDataLessThan(
						2, 0.0f,
						Particles::CHR::SetLocalPositionUsingData(func_compose<Position2D, const glm::vec2&>(
							vec2_compwise<float, float>(make_functor_ptr(&identity<float>), make_functor_ptr(&cast<float, unsigned int>)),
							make_functor_ptr(&linear_combo<2, 2>, std::array<glm::vec2, 3>{
									glm::vec2{ -1.5f * p2width, -0.5f * p2height }, glm::vec2{ 2.0f * p2width, 0.0f }, glm::vec2{ 0.0f, 2.0f }})), 3, 1),
						Particles::CHR::SetLocalPositionUsingData(func_compose<Position2D, float>(
							make_functor_ptr(&cast<float, unsigned int>),
							make_functor_ptr(&linear_combo_f<2>, std::array<glm::vec2, 2>{
									glm::vec2{ -0.5f * p2width, -0.5f * p2height}, glm::vec2{ 0.0f, 2.0f }})), 1)
					)
				})
			),
			Particles::CHR::SyncAll()
		})
	};

	//ParticleSystem<> psys({ wave2, wave2, wave1 });
	//ParticleSystem<> psys({ wave1 });
	ParticleSystem<> psys({ wave2, wave2 });
	set_ptr(psys.SubsystemRef(1).Fickler().Rotation(), 0.5f * glm::pi<float>());
	psys.SubsystemRef(1).Fickler().SyncRS();
	set_ptr(psys.Fickler().Scale(), glm::vec2{_RendererSettings::initial_window_width / p2width, _RendererSettings::initial_window_height / p2height} * 0.75f);
	psys.Fickler().SyncRS();
	//psys.Transformer()->SyncGlobalWithLocalScales();
	//psys.Transformer()->SetPosition(300, 0);
	//psys.Transformer()->SetLocalTransform(2, { {-500, 0}, 0, {2, 2} });
	//ParticleSubsystemArray<> parr({ wave1, wave2 });

	//psys.SetPosition(-400, 0);
	//psys.Transformer().SyncGlobalWithLocalPositions();
	//parr.SetPosition(400, 0);
	//parr.Transformer().SyncGlobalWithLocalPositions();

	//parr.SubsystemRef(0).SetRotation(1.57f);
	//set_ptr(psys.Fickler().Modulate(), Colors::PSYCHEDELIC_PURPLE);
	//psys.Fickler().SyncM();

	Renderer::AddCanvasLayer(11);
	Renderer::GetCanvasLayer(11)->OnAttach(&psys);
	//Renderer::GetCanvasLayer(11)->OnAttach(&parr);

	TileMap* tilemap_ini;
	if (Loader::loadTileMap("res/assets/tilemap.toml", tilemap_ini) != LOAD_STATUS::OK)
		ASSERT(false);
	std::unique_ptr<TileMap> tilemap(tilemap_ini);
	set_ptr(tilemap->Fickler().Transform(), {{100.0f, 200.0f}, 0.3f, {5.0f, 8.0f}});
	tilemap->Fickler().SyncT();
	tilemap->Insert(4, 0, 1);
	Renderer::GetCanvasLayer(11)->OnAttach(tilemap.get());
	// dangerous to set scale without checking if non-null, but the fickle type of selector is held constant, so scale is known to be non-null.
	*actor3.Fickler().Scale() *= 2.0f;
	actor3.Fickler().SyncRS();

	TextureFactory::SetSettings(textureFlag, Texture::nearest_settings);

	RectRender root(textureFlag);
	RectRender child(textureSnowman);
	RectRender grandchild(textureTux);
	RectRender child2(textureSnowman);
	RectRender grandchild2(textureTux);

	child.Fickler().ProteanLinker()->Attach(grandchild.Fickler().ProteanLinker());
	set_ptr(grandchild.Fickler().Scale(), {0.25f, 0.25f});
	set_ptr(grandchild.Fickler().Position(), {300.0f, -100.0f});
	set_ptr(child.Fickler().Scale(), {0.25f, 0.25f});
	child.Fickler().SyncT();
	child2.Fickler().ProteanLinker()->Attach(grandchild2.Fickler().ProteanLinker());
	set_ptr(grandchild2.Fickler().Scale(), { 0.25f, 0.25f });
	set_ptr(grandchild2.Fickler().Position(), { 300.0f, -100.0f });
	set_ptr(child2.Fickler().Scale(), { 0.25f, 0.25f });
	child2.Fickler().SyncT();

	root.Fickler().ProteanLinker()->Attach(child.Fickler().ProteanLinker());
	root.Fickler().ProteanLinker()->Attach(child2.Fickler().ProteanLinker());
	set_ptr(root.Fickler().Scale(), {10.0f, 10.0f});
	root.Fickler().SyncT();
	for (const auto& child : root.Fickler().ProteanLinker()->children)
	{
		child->Position() = { 0.0f, -30.0f };
		child->Scale() *= 0.2f;
		child->SyncT();
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

	DebugRect rect(_RendererSettings::initial_window_width * 0.5f, _RendererSettings::initial_window_height * 1.0f, true, { 1.0f, 0.5f }, 1);
	set_ptr(rect.Fickler().Modulate(), { 0.5f, 0.5f, 1.0f, 0.3f });
	rect.Fickler().SyncM();
	Renderer::GetCanvasLayer(11)->OnAttach(&rect);

	RectRender tux(textureTux);
	ActorTesselation2D tuxTessel(&tux);
	ActorTesselation2D tuxGrid(&tuxTessel);
	Renderer::GetCanvasLayer(11)->OnAttach(&tuxGrid);

	*tux.Fickler().Scale() *= 0.1f;
	tux.Fickler().SyncRS();

	tuxTessel.PushBackStatic({
		{ { 0, 100 } },
		{ { 200, 0 } },
		{ { -200, 0 } }
	});
	tuxGrid.PushBackStatic({
		{ { 50, 0 } },
		{ { 50, 200 } },
		{ { 50, -200 } }
	});

	tux.SetModulationPerPoint({ Colors::WHITE, Colors::BLUE, Colors::TRANSPARENT, Colors::LIGHT_GREEN });

	for (;;)
	{
		drawTime = static_cast<real>(glfwGetTime());
		deltaDrawTime = drawTime - prevDrawTime;
		prevDrawTime = drawTime;
		totalDrawTime += deltaDrawTime;
		
		*child.Fickler().Rotation() = -Pulsar::totalDrawTime;
		*child2.Fickler().Rotation() = -Pulsar::totalDrawTime;
		*grandchild.Fickler().Rotation() = Pulsar::totalDrawTime;
		*grandchild2.Fickler().Rotation() = -Pulsar::totalDrawTime;
		*root.Fickler().Rotation() = Pulsar::totalDrawTime;
		*child.Fickler().Position() += 5 * Pulsar::deltaDrawTime;
		*child2.Fickler().Scale() *= 1.0f / (1.0f + 0.1f * Pulsar::deltaDrawTime);
		root.Fickler().SyncT();

		Renderer::OnDraw();
		glfwPollEvents();
		if (glfwWindowShouldClose(window))
			break;
	}
}
