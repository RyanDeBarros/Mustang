#include "VendorInclude.h"
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
#include "render/actors/ActorTesselation.h"
#include "factory/Atlas.h"
#include "render/actors/TileMap.h"
#include "render/actors/shapes/DebugPolygon.h"
#include "render/actors/shapes/DebugPoint.h"
#include "render/actors/shapes/DebugCircle.h"
#include "render/actors/shapes/DebugRect.h"
#include "render/actors/shapes/DebugBatcher.h"
#include "render/actors/particles/ParticleSystem.h"
#include "render/actors/particles/ParticleSubsystemArray.h"
#include "render/actors/particles/ParticleSubsystemRegistry.h"
#include "render/actors/particles/Characteristics.h"
#include "utils/CommonMath.h"
#include "utils/Functor.inl"
#include "utils/Functions.inl"
#include "utils/Strings.h"
#include "utils/Data.inl"
#include "utils/Constants.h"
#include "render/actors/anim/FramesArray.h"
#include "render/actors/anim/AnimActorPrimitive.h"
#include "render/actors/anim/AnimationPlayer.h"
#include "render/actors/anim/KeyFrames.inl"

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

static void(*_post_init)() = []() {};
static void(*_frame_start)() = []() {};

void Pulsar::PostInit(void(*post_init)())
{
	if (post_init)
		_post_init = post_init;
}

void Pulsar::FrameStart(void(*frame_start)())
{
	if (frame_start)
		_frame_start = frame_start;
}

void Pulsar::Run(GLFWwindow* window)
{
	prevDrawTime = drawTime = static_cast<real>(glfwGetTime());
	deltaDrawTime = totalDrawTime = 0;
	_post_init();

	// Load textures
	TextureHandle textureSnowman, textureTux, textureFlag;
	if (Loader::loadTexture("res/assets/snowman.toml", textureSnowman) != LOAD_STATUS::OK)
		ASSERT(false);
	if (Loader::loadTexture("res/assets/tux.toml", textureTux) != LOAD_STATUS::OK)
		ASSERT(false);
	if (Loader::loadTexture("res/assets/flag.toml", textureFlag) != LOAD_STATUS::OK)
		ASSERT(false);
	//if (loadTexture("res/assets/atlas.toml", textureAtlas) != LOAD_STATUS::OK)
	//	ASSERT(false);
	TextureHandle tex_dirtTL = TextureRegistry::GetHandle({ "res/textures/dirtTL.png" });
	TextureHandle tex_dirtTR = TextureRegistry::GetHandle({ "res/textures/dirtTR.png" });
	TextureHandle tex_grassSingle = TextureRegistry::GetHandle({ "res/textures/grassSingle.png" });
	TextureHandle tex_grassTL = TextureRegistry::GetHandle({ "res/textures/grassTL.png" });
	TextureHandle tex_grassTE = TextureRegistry::GetHandle({ "res/textures/grassTE.png" });
	TextureHandle tex_grassTR = TextureRegistry::GetHandle({ "res/textures/grassTR.png" });

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
	if (Loader::loadRenderable("res/assets/renderable.toml", renderable) != LOAD_STATUS::OK)
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

	TextureRegistry::SetSettings(actor1.GetTextureHandle(), Texture::linear_settings);

	actor3.SetPivot(0.5f, 0.5f);
	set_ptr(actor3.Fickler().Position(), { 0.0f, 0.0f });
	set_ptr(actor3.Fickler().Scale(), { 0.1f, 0.1f });
	actor3.Fickler().SyncT();
	Renderer::GetCanvasLayer(0)->OnDetach(&actor3);

	actor2.CropToRelativeRect({ 0.3f, 0.4f, 0.4f, 0.55f });

	Renderer::RemoveCanvasLayer(0);
	Renderer::RemoveCanvasLayer(-1);

	float p2width = 400.0f;
	float p2height = 400.0f;
	ParticleEffect* psys_raw = nullptr;
	if (Loader::loadParticleEffect("res/assets/psys.toml", psys_raw, "system", true) != LOAD_STATUS::OK)
		ASSERT(false);
	std::unique_ptr<ParticleEffect> psys(psys_raw);

	set_ptr(psys->Fickler().Scale(), glm::vec2{_RendererSettings::initial_window_width / p2width, _RendererSettings::initial_window_height / p2height} * 0.75f);
	psys->Fickler().SyncRS();

	Renderer::AddCanvasLayer(11);
	Renderer::GetCanvasLayer(11)->OnAttach(psys.get());

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

	TextureRegistry::SetSettings(textureFlag, Texture::nearest_settings);

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

	AnimActorPrimitive2D serotonin(new RectRender(0, ShaderRegistry::Standard(), 10), { FramesArray("res/textures/serotonin.gif") }, 1.0f / 60.0f, false, 0.5f);
	Renderer::RemoveCanvasLayer(11);
	Renderer::AddCanvasLayer(11);
	Renderer::GetCanvasLayer(11)->OnAttach(serotonin.Primitive());

	ProteanLinker2D* serotoninPRL = serotonin.Primitive()->Fickler().ProteanLinker();
	AnimationTrack<Position2D, KF_Assign<Position2D>> animTrack1(2.0f);
	animTrack1.target = &serotoninPRL->Position();
	animTrack1.callback = make_functor_ptr<true>([](ProteanLinker2D* trf) { trf->SyncP(); }, serotoninPRL);
	animTrack1.SetOrInsert(KF_Assign<Position2D>(0.0f, Position2D{ 0.0f, 0.0f }));
	animTrack1.SetOrInsert(KF_Assign<Position2D>(0.5f, Position2D{ 100.0f, 0.0f }));
	animTrack1.SetOrInsert(KF_Assign<Position2D>(1.0f, Position2D{ 0.0f, 0.0f }));
	animTrack1.SetOrInsert(KF_Assign<Position2D>(1.5f, Position2D{ -100.0f, 0.0f }));
	animTrack1.SetOrInsert(KF_Assign<Position2D>(2.0f, Position2D{ 0.0f, 0.0f }));

	for (;;)
	{
		drawTime = static_cast<real>(glfwGetTime());
		deltaDrawTime = drawTime - prevDrawTime;
		prevDrawTime = drawTime;
		totalDrawTime += deltaDrawTime;

		_frame_start();
		
		*child.Fickler().Rotation() = -Pulsar::totalDrawTime;
		*child2.Fickler().Rotation() = -Pulsar::totalDrawTime;
		*grandchild.Fickler().Rotation() = Pulsar::totalDrawTime;
		*grandchild2.Fickler().Rotation() = -Pulsar::totalDrawTime;
		*root.Fickler().Rotation() = Pulsar::totalDrawTime;
		*child.Fickler().Position() += 5 * Pulsar::deltaDrawTime;
		*child2.Fickler().Scale() *= 1.0f / (1.0f + 0.1f * Pulsar::deltaDrawTime);
		root.Fickler().SyncT();

		serotonin.OnUpdate();
		animTrack1.OnUpdate();

		Renderer::OnDraw();
		glfwPollEvents();
		if (glfwWindowShouldClose(window))
			break;
	}
}
