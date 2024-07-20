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
	RectRender* actor1 = new RectRender(textureFlag, { {-500.0f, 300.0f}, -1.0f, {0.8f, 1.2f} });
	RectRender* actor2 = new RectRender(textureSnowman, { {400.0f, -200.0f}, 0.25f, {0.7f, 0.7f} });
	RectRender* actor3 = new RectRender(textureTux, { {0.0f, 0.0f}, 0.0f, {1.0f, 1.0f} });

	Renderer::GetCanvasLayer(0)->OnAttach(actor1);
	Renderer::GetCanvasLayer(0)->OnAttach(actor2);
	Renderer::GetCanvasLayer(0)->OnSetZIndex(actor1, 1);
	Renderer::GetCanvasLayer(0)->OnAttach(actor3);
	Renderer::AddCanvasLayer(-1);

	Renderable renderable;
	if (loadRenderable("res/assets/renderable.toml", renderable, true) != LOAD_STATUS::OK)
		ASSERT(false);
	ActorPrimitive2D* actor4 = new ActorPrimitive2D(renderable, { {-200.0f, 0.0f}, 0.0f, {800.0f, 800.0f} });
	Renderer::GetCanvasLayer(-1)->OnAttach(actor4);

	actor1->SetScale(16.0f, 16.0f);
	Renderer::GetCanvasLayer(0)->OnSetZIndex(actor3, -1);

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
	Renderer::GetCanvasLayer(0)->OnDetach(actor3);

	ActorTesselation2D tessel(actor3);
	Renderer::GetCanvasLayer(0)->OnAttach(&tessel);

	float w = actor3->GetWidth() * actor3->GetTransform().scale.x;
	float h = actor3->GetHeight() * actor3->GetTransform().scale.y;
	tessel.RectVectorRef() = { {{0.0f, 0.0f}, 0.0f, {1.0f, 1.0f}}, {{w, 0.0f}, 0.0f, {1.0f, 1.0f}}, {{2 * w, 0.0f}, 0.5f, {1.0f, 1.0f}} };
	Renderer::GetCanvasLayer(0)->OnSetZIndex(&tessel, 10);

	ActorPrimitive2D* const actor_ref = dynamic_cast<ActorPrimitive2D* const>(tessel.ActorRef());
	if (actor_ref)
	{
		actor_ref->SetModulationPerPoint({
			{1.0f, 1.0f, 1.0f, 1.0f},
			{1.0f, 1.0f, 1.0f, 1.0f},
			{0.0f, 0.0f, 0.0f, 0.0f},
			{0.0f, 0.0f, 0.0f, 0.0f},
			});
	}

	ActorTesselation2D tesselVertical(&tessel);
	Renderer::GetCanvasLayer(0)->OnDetach(&tessel);
	Renderer::GetCanvasLayer(0)->OnAttach(&tesselVertical);
	tesselVertical.RectVectorRef() = { {{0.0f, 0.0f}, 0.0f, {1.0f, 1.0f}}, {{0.0f, -h}, 0.0f, {1.0f, 1.0f}}, {{0.0f, -2 * h}, 0.5f, {1.0f, 1.0f}} };

	ActorTesselation2D tesselDiagonal(&tesselVertical);
	Renderer::GetCanvasLayer(0)->OnDetach(&tesselVertical);
	Renderer::GetCanvasLayer(0)->OnAttach(&tesselDiagonal);
	tesselDiagonal.RectVectorRef() = { {{0.0f, 0.0f}, 0.0f, {1.0f, 1.0f}}, {{-0.5f * w, 0.5 * h}, -0.8f, {0.75f, -0.75f}} };

	actor2->CropToRelativeRect({ 0.3f, 0.4f, 0.4f, 0.55f });

	//TileHandle tile_dirtTL(TextureFactory::GetTileHandle(tex_dirtTL)), tile_dirtTR(TextureFactory::GetTileHandle(tex_dirtTR)), tile_grassSingle(TextureFactory::GetTileHandle(tex_grassSingle)), tile_grassTL(TextureFactory::GetTileHandle(tex_grassTL)), tile_grassTE(TextureFactory::GetTileHandle(tex_grassTE)), tile_grassTR(TextureFactory::GetTileHandle(tex_grassTR));
	//std::vector<TileHandle> tiles = { tile_dirtTL, tile_dirtTR, tile_grassSingle, tile_grassTL, tile_grassTE, tile_grassTR };
	//TileHandle tileAtlas = TileFactory::GetAtlasHandle(tiles, -1, -1, 1);
	//const Atlas* atlas = dynamic_cast<const Atlas*>(TileFactory::GetConstTileRef(tileAtlas));
	//if (!atlas)
	//	ASSERT(false);
	//if (!saveAtlas(atlas, "res/textures/atlas.png", "res/assets/atlas_asset.toml"))
	//	ASSERT(false);

	TileHandle tileAtlas;
	if (loadAtlas("res/assets/atlas_asset.toml", tileAtlas) != LOAD_STATUS::OK)
		ASSERT(false);

	Atlas* atlas = dynamic_cast<Atlas*>(TileFactory::GetTileRef(tileAtlas));
	if (!atlas)
		ASSERT(false);

	TileMap tilemap(tileAtlas);

	if (!tilemap.SetOrdering(std::vector<size_t>{0, 1, 2, 3, 4, 5}))
		ASSERT(false);

	// TODO tilemap asset file that saves these:
	tilemap.SetTransform({ {100.0f, 200.0f}, -0.5f, { 5.0f, 8.0f } });
	tilemap.Insert(3, -2, 0);
	tilemap.Insert(4, -1, 0);
	tilemap.Insert(4, 0, -1);
	tilemap.Insert(4, 1, 0);
	tilemap.Insert(5, 2, 0);

	Renderer::GetCanvasLayer(0)->OnAttach(&tilemap);
	Renderer::GetCanvasLayer(0)->OnSetZIndex(&tilemap, 10);

	LocalTransformer2D flags(actor4->GetTransformRef(), actor1);

	Renderer::ChangeCanvasLayerIndex(-1, 1);

	Renderer::AddCanvasLayer(10);
	DebugPolygon poly({ {0.0f, 0.0f}, {100.0f, 0.0f}, {50.0f, 200.0f}, {150.0f, 200.0f}, {0.0f, 400.0f}, {100.0f, 400.0f} }, {}, { 0.1f, 0.2f, 1.0f, 1.0f }, GL_TRIANGLE_STRIP);
	Renderer::GetCanvasLayer(10)->OnAttach(&poly);
	//poly.visible = false;
	Renderer::GetCanvasLayer(10)->OnSetZIndex(&poly, 5);
	Renderer::RemoveCanvasLayer(0);
	Renderer::RemoveCanvasLayer(1);

	float side = static_cast<float>(_RendererSettings::initial_window_height);

	DebugCircle circ(100.0f);
	Renderer::GetCanvasLayer(10)->OnAttach(&circ);
	//circ.SetScale(2.0f, 5.0f);
	//circ.SetRotation(0.3);
	circ.SetRadius(_RendererSettings::initial_window_height * 0.5f);
	circ.SetColor({ 0.8f, 0.8f, 0.8f, 0.4f });

	RectRender rect(*actor2);
	Renderer::AddCanvasLayer(-3);
	Renderer::GetCanvasLayer(-3)->OnAttach(&rect);
	rect.CropToRelativeRect({ 0.0f, 0.0f, 1.0f, 1.0f });
	rect.SetTransform({ {}, 0.0f, {side / rect.GetUVWidth(), side / rect.GetUVHeight()} });

	DebugPoint pnt({}, { 0.0f, 0.4f, 0.1f, 0.7f }, side);
	Renderer::GetCanvasLayer(10)->OnAttach(&pnt);
	//pnt.SetInnerRadius(0.25f);
	pnt.SetInnerRadius(pnt.InnerRadiusFromBorderWidth(side * 0.25f));

	DebugBatcher debug_batcher;
	auto p_circ = std::make_shared<DebugCircle>(circ);
	auto p_pnt = std::make_shared<DebugPoint>(pnt);
	auto p_poly = std::make_shared<DebugPolygon>(poly);
	debug_batcher.PushBackAll({ p_circ, p_pnt, p_poly });
	Renderer::RemoveCanvasLayer(10);
	Renderer::AddCanvasLayer(10);
	Renderer::GetCanvasLayer(10)->OnAttach(&debug_batcher);
	debug_batcher.ChangeZIndex(p_circ->GetDebugModel(), -1);

	Renderer::RemoveCanvasLayer(10);
	Renderer::RemoveCanvasLayer(-3);

	ParticleWaveData<> wave1{
		1.5f,
		std::shared_ptr<DebugPolygon>(new DebugCircle(3.0f)),
		CumulativeFunc<>([](float t) { return t < 0.6f ? PowerFunc(2000.0f, 0.5f)(t) : PowerFunc(2000.0f, 0.5f)(0.6f); }),
		[](float t) { return 0.4f - t * 0.05f; },
		[](float t, unsigned short i, unsigned int ti) { return std::vector<Particles::Characteristic>{
			{ [](Particle& p) {
				static float mt = 0.5f;
				static float imt = 1.0f / (1.0f - mt);
				if (p.t() < mt)
					p.m_Shape->SetColor({ 1.0f, p.t(), 0.0f, 1.0f });
				else
					p.m_Shape->SetColor({ 1.0f, p.t(), 0.0f, 1.0f - (p.t() - mt) * imt });
			}},
			{ [t](Particle& p, const std::shared_ptr<Particles::CHRSeedData>& data) {
				if (p.t() == 0.0f)
				{
					p.m_Transformer.SetLocalPosition(glm::vec2{ glm::cos(data->s[0] * 2 * glm::pi<float>()), glm::sin(data->s[0] * 2 * glm::pi<float>()) } *20.0f * data->s[1]);
					glm::vec2 vel = glm::normalize(glm::vec2{ rng() * 2.0f - 1.0f, rng() * 2.0f - 1.0f }) * glm::vec2{ 200.0f, 100.0f };
					data->s[0] = rng();
					vel *= (1.0f + data->s[0] * 0.25f) * (1.0f - 0.4f * glm::pow(t, 0.2f + data->s[0] * 0.25f));
					data->s[0] = vel.x;
					data->s[1] = vel.y;
				}
				else
				{
					p.m_Transformer.OperateLocalPosition([&](glm::vec2& pos) { pos += glm::vec2{ data->s[0], data->s[1] } *p.dt(); });
				}
			}, 2}
		}; }
	};
	float p2width = 400.0f;
	float p2height = 400.0f;
	ParticleWaveData<> wave2{
		1.5f,
		std::shared_ptr<DebugPolygon>(new DebugPolygon({ {0, 0}, {0, 1}, {1, 1}, {1, 0} }, {}, {}, GL_TRIANGLE_FAN)),
		CumulativeFunc<>(LinearFunc(p2height * 0.5f)),
		[](float t) { return 1.5f; },
		[=](real t, unsigned short i, unsigned int ti) { return std::vector<Particles::Characteristic>{
			{ [=](Particle& p, const std::shared_ptr<Particles::CHRSeedData>& data) {
				if (p.t() == 0.0f)
				{
					data->s[1] = static_cast<float>(std::rand() % ti);
					data->s[2] = std::rand() % 2 == 0 ? 1 : -1;
				}
				real mt = std::fmod(p.t() + data->s[0], 1.0f);
				if (mt < 0.5f)
					p.m_Shape->SetColor({ 2.0f * mt, 2.0f * mt, 1.0f, 1.0f });
				else
					p.m_Shape->SetColor({ 2.0f * (1.0f - mt), 2.0f * (1.0f - mt), 1.0f, 1.0f });
				if (mt < 0.5f)
					p.m_Transformer.SetLocalScale({ 1.0f + 2.0f * mt * p2width, 1.0f });
				else
					p.m_Transformer.SetLocalScale({ 1.0f + 2.0f * (1.0f - mt) * p2width, 1.0f});
				if (data->s[2] > 0)
				{
					if (mt < 0.5f)
						p.m_Transformer.SetLocalPosition({ -0.5f * p2width, 2.0f * static_cast<unsigned int>(data->s[1]) - 0.5f * p2height });
					else
						p.m_Transformer.SetLocalPosition({ (p2width + 1.0f) * (mt - 0.5f) * 2.0f - 0.5f * p2width, 2.0f * static_cast<unsigned int>(data->s[1]) - 0.5f * p2height });
				}
				else
				{
					if (mt < 0.5f)
						p.m_Transformer.SetLocalPosition({ -(p2width - 1.0f) * (mt - 0.5f) * 2.0f - 0.5f * p2width, 2.0f * static_cast<unsigned int>(data->s[1]) - 0.5f * p2height });
					else
						p.m_Transformer.SetLocalPosition({ -0.5f * p2width, 2.0f * static_cast<unsigned int>(data->s[1]) - 0.5f * p2height});
				}
				p.m_Transformer.SyncGlobalWithLocal();
			}, 3}
		}; }
	};

	ParticleSystem<> psys({ wave1, wave2 });
	Renderer::AddCanvasLayer(11);
	Renderer::GetCanvasLayer(11)->OnAttach(&psys);
	Logger::NewLine();

	psys.Pause();

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

	delete actor1, actor2, actor3, actor4;
}
