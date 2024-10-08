﻿#include "VendorInclude.h"
#include "Pulsar.h"

#include <functional>
#include <glm/ext/scalar_constants.hpp>
#include <stb/stb_truetype.h>
#include <stb/stb_image_write.h>

#include "PulsarSettings.h"
#include "Logger.inl"
#include "Macros.h"
#include "AssetLoader.h"
#include "render/Renderer.h"
#include "render/actors/TileMap.h"
#include "render/actors/shapes/DebugPoint.h"
#include "render/actors/shapes/DebugCircle.h"
#include "render/actors/shapes/DebugRect.h"
#include "render/actors/particles/ParticleSystem.h"
#include "render/actors/particles/ParticleSubsystemArray.h"
#include "render/actors/particles/ParticleSubsystemRegistry.h"
#include "render/actors/particles/Characteristics.h"
#include "utils/Constants.h"
#include "render/actors/anim/AnimActorPrimitive.h"
#include "render/actors/anim/AnimationPlayer.inl"
#include "render/actors/anim/KeyFrames.inl"
#include "render/transform/YSorter.h"
#include "render/actors/NonantRender.h"
#include "platform/WindowManager.h"
#include "platform/InputManager.h"
#include "utils/Strings.h"
#include "IO.h"
#include "render/Font.h"

real Pulsar::drawTime;
real Pulsar::deltaDrawTime;
real Pulsar::prevDrawTime;
real Pulsar::totalDrawTime;

static bool glfw_initialized = false;;

bool Pulsar::GLFWInitialized()
{
	return glfw_initialized;
}

void Pulsar::CreateWindow(WindowHandle handle, const char* title, GLFWmonitor* monitor, GLFWwindow* share)
{
	WindowManager::RegisterWindow(handle, Window(static_cast<unsigned int>(PulsarSettings::initial_window_width()),
		static_cast<unsigned int>(PulsarSettings::initial_window_height()), title, monitor, share));
}

static void glfw_error_callback(int error, const char* description)
{
	Logger::LogErrorFatal(std::string("GLFW err(") + std::to_string(error) + "): " + description);
}

int Pulsar::StartUp(const char* title)
{
	if (!PulsarSettings::_loaded())
		return -1;
	if (!glfwInit())
		return -1;
	glfw_initialized = true;
	glfwSetErrorCallback(glfw_error_callback);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	std::srand(static_cast<unsigned int>(time(0)));

	CreateWindow(0, title);
	Logger::LogInfo("Welcome to Pulsar Renderer! GL_VERSION:");
	PULSAR_TRY(Logger::LogInfo(reinterpret_cast<const char*>(glGetString(GL_VERSION))));
	Renderer::Init();
	Renderer::FocusWindow(0);
	
	// TODO also use:
	// glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureSlots);
	// TODO Logger functions should have a flush mode that either buffers messages are flushes them.
	return 0;
}

void Pulsar::Terminate()
{
	Renderer::Terminate();
	glfwTerminate();
	glfw_initialized = false;
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

static std::function<void(void)> _frame_exec;

void Pulsar::Run()
{
	prevDrawTime = drawTime = static_cast<real>(glfwGetTime());
	deltaDrawTime = totalDrawTime = 0;
	_post_init();

	// Load textures
	TextureHandle textureSnowman, textureTux, textureFlag;
	if (Loader::loadTexture("res/assets/snowman.toml", textureSnowman) != LOAD_STATUS::OK)
		PULSAR_ASSERT(false);
	if (Loader::loadTexture("res/assets/tux.toml", textureTux) != LOAD_STATUS::OK)
		PULSAR_ASSERT(false);
	if (Loader::loadTexture("res/assets/flag.toml", textureFlag) != LOAD_STATUS::OK)
		PULSAR_ASSERT(false);
	//if (loadTexture("res/assets/atlas.toml", textureAtlas) != LOAD_STATUS::OK)
	//	PULSAR_ASSERT(false);
	TextureHandle tex_dirtTL = Renderer::Textures().GetHandle({ "res/textures/dirtTL.png" });
	TextureHandle tex_dirtTR = Renderer::Textures().GetHandle({ "res/textures/dirtTR.png" });
	TextureHandle tex_grassSingle = Renderer::Textures().GetHandle({ "res/textures/grassSingle.png" });
	TextureHandle tex_grassTL = Renderer::Textures().GetHandle({ "res/textures/grassTL.png" });
	TextureHandle tex_grassTE = Renderer::Textures().GetHandle({ "res/textures/grassTE.png" });
	TextureHandle tex_grassTR = Renderer::Textures().GetHandle({ "res/textures/grassTR.png" });

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
		PULSAR_ASSERT(false);
	ActorPrimitive2D actor4(renderable);
	set_ptr(actor4.Fickler().Transform(), { {-200.0f, 0.0f}, 0.0f, {800.0f, 800.0f} });
	actor4.Fickler().SyncT();
	Renderer::GetCanvasLayer(-1)->OnAttach(&actor4);

	set_ptr(actor1.Fickler().Scale(), { 16.0f, 16.0f });
	actor1.Fickler().SyncRS();
	Renderer::GetCanvasLayer(0)->OnSetZIndex(&actor3, -1);

	actor3.SetPivot(0.0f, 0.0f);
	set_ptr(actor3.Fickler().Position(), PulsarSettings::initial_window_rel_pos(-0.5f, 0.5f));
	set_ptr(actor3.Fickler().Scale(), { 0.3f, 0.3f });
	actor3.Fickler().SyncT();
	actor2.SetModulation(glm::vec4(0.7f, 0.7f, 1.0f, 1.0f));
	actor3.SetModulationPerPoint({
		glm::vec4(1.0f, 0.5f, 0.5f, 1.0f),
		glm::vec4(0.5f, 1.0f, 0.5f, 1.0f),
		glm::vec4(0.5f, 0.5f, 1.0f, 1.0f),
		glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)
		});

	Renderer::Textures().SetSettings(actor1.GetTextureHandle(), Texture::linear_settings);

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
		PULSAR_ASSERT(false);
	std::unique_ptr<ParticleEffect> psys(psys_raw);

	set_ptr(psys->Fickler().Scale(), glm::vec2{ PulsarSettings::initial_window_width() / p2width, PulsarSettings::initial_window_height() / p2height } *0.75f);
	psys->Fickler().SyncRS();

	Renderer::AddCanvasLayer(11);
	Renderer::GetCanvasLayer(11)->OnAttach(psys.get());

	TileMap* tilemap_ini;
	if (Loader::loadTileMap("res/assets/tilemap.toml", tilemap_ini) != LOAD_STATUS::OK)
		PULSAR_ASSERT(false);
	std::unique_ptr<TileMap> tilemap(tilemap_ini);
	set_ptr(tilemap->Fickler().Transform(), { {100.0f, 200.0f}, 0.3f, {5.0f, 8.0f} });
	tilemap->Fickler().SyncT();
	tilemap->Insert(4, 0, 1);
	Renderer::GetCanvasLayer(11)->OnAttach(tilemap.get());
	// dangerous to set scale without checking if non-null, but the fickle type of selector is held constant, so scale is known to be non-null.
	*actor3.Fickler().Scale() *= 2.0f;
	actor3.Fickler().SyncRS();

	Renderer::Textures().SetSettings(textureFlag, Texture::nearest_settings);

	RectRender root(textureFlag);
	RectRender child(textureSnowman);
	RectRender grandchild(textureTux);
	RectRender child2(textureSnowman);
	RectRender grandchild2(textureTux);

	child.Fickler().AttachUnsafe(grandchild.Fickler());
	set_ptr(grandchild.Fickler().Scale(), { 0.25f, 0.25f });
	set_ptr(grandchild.Fickler().Position(), { 300.0f, -100.0f });
	set_ptr(child.Fickler().Scale(), { 0.25f, 0.25f });
	child.Fickler().SyncT();
	child2.Fickler().AttachUnsafe(grandchild2.Fickler());
	set_ptr(grandchild2.Fickler().Scale(), { 0.25f, 0.25f });
	set_ptr(grandchild2.Fickler().Position(), { 300.0f, -100.0f });
	set_ptr(child2.Fickler().Scale(), { 0.25f, 0.25f });
	child2.Fickler().SyncT();

	root.Fickler().AttachUnsafe(child.Fickler());
	root.Fickler().AttachUnsafe(child2.Fickler());
	set_ptr(root.Fickler().Scale(), { 10.0f, 10.0f });
	root.Fickler().SyncT();
	for (const auto& child : root.Fickler().Transformer()->children)
	{
		child->Position() = { 0.0f, -30.0f };
		child->Scale() *= 0.2f;
		child->Sync();
	}

	Renderer::GetCanvasLayer(11)->OnAttach(&root);
	Renderer::GetCanvasLayer(11)->OnAttach(&child2);
	Renderer::GetCanvasLayer(11)->OnAttach(&grandchild2);
	Renderer::GetCanvasLayer(11)->OnAttach(&child);
	Renderer::GetCanvasLayer(11)->OnAttach(&grandchild);

	DebugRect rect(PulsarSettings::initial_window_width() * 0.5f, PulsarSettings::initial_window_height() * 1.0f, true, { 1.0f, 0.5f }, 1);
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

	tux.SetModulationPerPoint({ Colors::WHITE, Colors::BLUE, Colors::RED * Colors::HALF_TRANSPARENT_WHITE, Colors::LIGHT_GREEN });

	// TODO FramesArray can use up a lot of unique texture handles. Add option to use textures directly without using texture registry.
	AnimActorPrimitive2D serotonin(new RectRender(0), { FramesArray("res/textures/serotonin.gif") }, 1.0f / 60.0f, false, 1.5f);
	serotonin.Primitive()->z = 10;
	//Renderer::RemoveCanvasLayer(11);
	//Renderer::AddCanvasLayer(11);
	Renderer::GetCanvasLayer(11)->OnAttach(serotonin.Primitive());

	AnimationPlayer<AnimActorPrimitive2D> animPlayer1;
	animPlayer1.SetTarget(&serotonin);
	animPlayer1.SetPeriod(2.0f);
	animPlayer1.SetSpeed(0.5f);
	float ap1frames = 0.0f;

	Fickler2D* serotoninPRL = &serotonin.Primitive()->Fickler();
	using AnimTrack1T = AnimationTrack<AnimActorPrimitive2D, Position2D, KF_Assign<Position2D>, Interp::Linear>;
	AnimTrack1T animTrack1;
	animTrack1.getProperty = [](AnimActorPrimitive2D* anim) { return anim->Primitive()->Fickler().Position(); };
	animTrack1.callback = make_functor<true>([](Fickler2D* trf) { trf->SyncP(); }, serotoninPRL);
	animTrack1.SetOrInsert(KF_Assign<Position2D>(0.0f, { 0.0f, 100.0f }));
	animTrack1.SetOrInsert(KF_Assign<Position2D>(0.5f, { 300.0f, 0.0f }));
	animTrack1.SetOrInsert(KF_Assign<Position2D>(1.0f, { 0.0f, -100.0f }));
	animTrack1.SetOrInsert(KF_Assign<Position2D>(1.5f, { -300.0f, 0.0f }));
	animTrack1.SetOrInsert(KF_Assign<Position2D>(2.0f, { 0.0f, 100.0f }));
	animPlayer1.tracks.push_back(CopyPtr(animTrack1));

	using AnimTrack2T = AnimationTrack<AnimActorPrimitive2D, Modulate, KF_Assign<Modulate>, Interp::Linear>;
	AnimTrack2T animTrack2;
	animTrack2.getProperty = [](AnimActorPrimitive2D* anim) { return anim->Primitive()->Fickler().Modulate(); };
	animTrack2.callback = make_functor<true>([](Fickler2D* trf) { trf->SyncM(); }, serotoninPRL);
	animTrack2.SetOrInsert(KF_Assign<Modulate>(0.0f, Colors::WHITE));
	animTrack2.SetOrInsert(KF_Assign<Modulate>(0.5f, Colors::BLUE));
	animTrack2.SetOrInsert(KF_Assign<Modulate>(1.0f, Colors::GREEN));
	animTrack2.SetOrInsert(KF_Assign<Modulate>(1.5f, Colors::RED));
	animTrack2.SetOrInsert(KF_Assign<Modulate>(2.0f, Colors::WHITE));
	animPlayer1.tracks.push_back(CopyPtr(animTrack2));
	animPlayer1.SyncTracks();

	AnimationPlayer<void> animPlayerEvents;
	animPlayerEvents.SetPeriod(2.0f);
	animPlayerEvents.SetSpeed(1.3f);

	using AnimEventTrackT = AnimationTrack<void, void, KF_Event<void>, Interp::Constant>;
	AnimEventTrackT animEventTrack;
	animEventTrack.SetOrInsert(KF_Event<void>(0.0f, [](void*) { Logger::LogInfo("0 seconds!"); }));
	animEventTrack.SetOrInsert(KF_Event<void>(0.5f, [](void*) { Logger::LogInfo("0.5 seconds!"); }));
	animEventTrack.SetOrInsert(KF_Event<void>(1.0f, [](void*) { Logger::LogInfo("1 seconds!"); }));
	animEventTrack.SetOrInsert(KF_Event<void>(1.5f, [](void*) { Logger::LogInfo("1.5 seconds!"); }));
	animEventTrack.SetOrInsert(KF_Event<void>(2.0f, [](void*) { Logger::LogInfo("2 seconds!"); }));
	animPlayerEvents.tracks.push_back(CopyPtr(animEventTrack));

	Renderer::GetCanvasLayer(11)->OnDetach(&root);
	Renderer::GetCanvasLayer(11)->OnDetach(&child2);
	Renderer::GetCanvasLayer(11)->OnDetach(&grandchild2);
	Renderer::GetCanvasLayer(11)->OnDetach(&child);
	Renderer::GetCanvasLayer(11)->OnDetach(&grandchild);
	Renderer::GetCanvasLayer(11)->OnDetach(serotonin.Primitive());

	YSorter ysorter(10);
	ysorter.PushBackAll({ &root, &child2, &grandchild2, &child, &grandchild, serotonin.Primitive() });
	Renderer::GetCanvasLayer(11)->OnAttach(&ysorter);

	Renderer::RemoveCanvasLayer(11);
	Renderer::AddCanvasLayer(11);

	TextureHandle ogntexture = Renderer::Textures().GetHandle(TextureConstructArgs_filepath("res/textures/panel.png", Texture::nearest_settings));
	//NonantRender nonant(NonantTile(*Renderer::Tiles().Get(ogntile), NonantLines_Absolute{ 6, 26, 6, 26 }));
	//NonantRender nonant(ogntile, NonantLines(6, 6, 6, 6));
	// TODO 0, 0, 0, 0 for Lines seems to not actually mean 0, 0, 0, 0, but instead seems to do 1, 1, 1, 1, i.e. 1-pixel border all around.
	NonantRender nonant(ogntexture, NonantLines(14.5f, 14.5f, 14.5f, 14.5f));

	RectRender og_nonant(ogntexture);

	// TODO function on FickleActor2D that does sync automatically when setting position, rotation, etc.
	set_ptr(og_nonant.Fickler().Scale(), { 10.0f, 10.0f });
	set_ptr(og_nonant.Fickler().Position(), { -250.0f, 250.0f });
	og_nonant.Fickler().SyncT();

	Renderer::GetCanvasLayer(11)->OnAttach(&og_nonant);
	Renderer::GetCanvasLayer(11)->OnAttach(&nonant);
	set_ptr(nonant.Fickler().Transform(), { { 150.0f, -300.0f }, 0.0f, { 10.0f, 10.0f } });
	nonant.Fickler().SyncT();
	//nonant.SetPivot({ 0.3f, 0.3f });
	
	Renderer::GetCanvasLayer(11)->OnAttach(tilemap.get());
	//Renderer::GetCanvasLayer(11)->OnAttach(psys.get());

	//WindowManager::GetWindow(0)->SetCursor(Cursor(StandardCursor::CROSSHAIR));
	//Tile t("res/textures/flag.png", 1.0f, false);
	//Cursor cursor = Cursor(t.GetImageBuffer(), t.GetWidth(), t.GetHeight());
	Cursor cursor(StandardCursor::CROSSHAIR);
	//cursor.mouseMode = MouseMode::VIRTUAL;
	WindowManager::GetWindow(0)->SetCursor(std::move(cursor));

	InputManager::Instance().DispatchMouseButton().Connect(InputBucket::MouseButton(0, Input::MouseButton::LEFT, Input::Action::PRESS),
		[](const InputEvent::MouseButton& event) {
			Logger::LogInfo(STR(WindowManager::GetWindow(0)->GetCursorPos()));
			});

	// TODO font registry?
	//Font* font = Renderer::Fonts().Emplace(FontConstructArgs("res/raw-fonts/Roboto-BoldItalic.ttf", 96.0f, u8""));
	//Font* font2 = Renderer::Fonts().Emplace(FontConstructArgs("res/raw-fonts/Roboto-Regular.ttf", 48.0f, Fonts::COMMON));
	FontFamily font_family("res/assets/fonts/Roboto.toml");
	//Font* font1 = font_family.GetFont("bold-italic", FontConstructorArgs(96.0f, u8""));
	Font* font1 = font_family.GetFont("bold-italic", 96.0f);
	//Font* font2 = font_family.GetFont("regular", FontConstructorArgs(48.0f, Font::COMMON));
	Font* font2 = font_family.GetFont("regular", 48.0f);
	TextRender text_render = font2->GetTextRender();
	text_render.format.horizontal_align = TextRender::HorizontalAlign::CENTER;
	text_render.format.vertical_align = TextRender::VerticalAlign::MIDDLE;
	text_render.format.min_width = 500;
	text_render.format.min_height = 500;
	text_render.text = U"Hello,\n World\t!é.........\r\n\rNext Line!!\nx xx  xxx\tx\n\n\n¿last line?";
	//text_render.text = U"Whereas recognition of the inherent dignity 😂水"; // NOTE roboto does not support emojis/kanji
	text_render.WarnInvalidCharacters();
	//text_render.text = U"ΣπΦ";
	//text_render.text = "hello";
	text_render.UpdateBounds();
	//Renderer::GetCanvasLayer(11)->Clear();
	//psys->z = -100;
	//Renderer::GetCanvasLayer(11)->OnAttach(psys.get());

	DebugRect text_background(text_render.OuterWidth(), text_render.OuterHeight(), true, {0.0f, 1.0f});
	text_background.Fickler().modulatable->self.Sync({0.0f, 0.0f, 0.6f, 0.6f});
	Renderer::GetCanvasLayer(11)->OnAttach(&text_background);
	Renderer::GetCanvasLayer(11)->OnAttach(&text_render);

	text_render.pivot = {0.5f, 0.5f};
	text_background.SetPivot({0.5f, 0.5f});
	
	_frame_exec = [&]() {
		drawTime = static_cast<real>(glfwGetTime());
		deltaDrawTime = drawTime - prevDrawTime;
		prevDrawTime = drawTime;
		totalDrawTime += deltaDrawTime;

		_frame_start();

		nonant.SetNonantWidth(nonant.GetUVWidth() + 10 * glm::sin(totalDrawTime));
		nonant.SetNonantHeight(nonant.GetUVHeight() + 10 * glm::cos(totalDrawTime));

		*child.Fickler().Rotation() = -Pulsar::totalDrawTime;
		*child2.Fickler().Rotation() = -Pulsar::totalDrawTime;
		*grandchild.Fickler().Rotation() = Pulsar::totalDrawTime;
		*grandchild2.Fickler().Rotation() = -Pulsar::totalDrawTime;
		*root.Fickler().Rotation() = Pulsar::totalDrawTime;
		*child.Fickler().Position() += 5 * Pulsar::deltaDrawTime;
		*child2.Fickler().Scale() *= 1.0f / (1.0f + 0.1f * Pulsar::deltaDrawTime);
		root.Fickler().SyncT();

		serotonin.OnUpdate();
		animPlayer1.OnUpdate();
		//animPlayerEvents.OnUpdate();

		ap1frames += deltaDrawTime;
		if (animPlayer1.isInReverse)
		{
			if (ap1frames > 3.0f)
			{
				ap1frames = unsigned_fmod(ap1frames, 3.0f);
				animPlayer1.isInReverse = !animPlayer1.isInReverse;
			}
		}
		else
		{
			if (ap1frames > 2.0f)
			{
				ap1frames = unsigned_fmod(ap1frames, 2.0f);
				animPlayer1.isInReverse = !animPlayer1.isInReverse;
			}
		}

		Renderer::OnDraw();
	};

	// TODO put run() in Window?
	drawTime = static_cast<real>(glfwGetTime());
	deltaDrawTime = drawTime - prevDrawTime;
	prevDrawTime = drawTime;
	totalDrawTime += deltaDrawTime;
	Window& window = *WindowManager::GetWindow(0);
	window._ForceRefresh();

	for (glfwPollEvents(); window.ShouldNotClose(); glfwPollEvents())
		_ExecFrame();
}

void Pulsar::_ExecFrame()
{
	_frame_exec();
}
