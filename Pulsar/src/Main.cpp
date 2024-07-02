#include "Main.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

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

static void run(GLFWwindow*);

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
	Renderer::Init();
	Renderer::FocusWindow(window);
	run(window);
	return 0;
}

void Pulsar::Terminate()
{
	Renderer::Terminate();
	glfwTerminate();
}

void run(GLFWwindow* window)
{
	double time = glfwGetTime();
	double deltaTime = 0;
	double prevTime = time;
	double totalTime = 0;

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
	
	Logger::LogInfo(std::to_string(std::get<GLint>(*const_cast<Uniform*>(UniformLexiconFactory::GetValue(1, "u_inttest")))));
	auto u = UniformLexiconFactory::GetValue(1, "u_float3test");
	if (u)
		Logger::LogInfo(STR(std::get<glm::vec3>(*u)));
	UniformLexiconFactory::SetValue(1, "u_float3test", glm::vec3(0.3, 0.6, 0.2));
	UniformLexiconFactory::DefineNewValue(1, "u_float3test", glm::vec3(1.0, 0.0, 1.0));
	UniformLexiconFactory::SetValue(1, "u_float3test", glm::vec3(0.3, 0.6, 0.2));
	u = UniformLexiconFactory::GetValue(1, "u_float3test");
	if (u)
		Logger::LogInfo(STR(std::get<glm::vec3>(*u)));

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

	tilemap.SetTransform({ {100.0f, 200.0f}, -0.5f, { 5.0f, 8.0f } });
	tilemap.Insert(3, -2, 0);
	tilemap.Insert(4, -1, 0);
	tilemap.Insert(4, 0, -1);
	tilemap.Insert(4, 1, 0);
	tilemap.Insert(5, 2, 0);

	Renderer::GetCanvasLayer(0)->OnAttach(&tilemap);
	Renderer::GetCanvasLayer(0)->OnDetach(&tesselDiagonal);
	Renderer::GetCanvasLayer(0)->OnDetach(actor1);
	Renderer::GetCanvasLayer(0)->OnDetach(actor2);
	Renderer::GetCanvasLayer(0)->OnDetach(actor3);
	Renderer::GetCanvasLayer(-1)->OnDetach(actor4);

	for (;;)
	{
		time = glfwGetTime();
		deltaTime = time - prevTime;
		prevTime = time;
		totalTime += deltaTime;
		// OnUpdate here

		Renderer::OnDraw();
		glfwPollEvents();
		if (glfwWindowShouldClose(window))
			break;
	}

	delete actor1, actor2, actor3, actor4;
}
