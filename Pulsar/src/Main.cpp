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
#include "render/ActorPrimitive.h"
#include "render/RectRender.h"
#include "factory/UniformLexiconFactory.h"
#include "render/ActorTesselation.h"

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
	GLFWwindow* window = glfwCreateWindow((int)_RendererSettings::initial_window_width, (int)_RendererSettings::initial_window_height, "Mustang Engine", nullptr, nullptr);
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

	// Load shaders
	ShaderHandle shaderStandard32;
	if (loadShader(_RendererSettings::standard_shader32_assetfile, shaderStandard32) != LOAD_STATUS::OK)
		ASSERT(false);

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

	// Load renderables
	Renderable renderable;
	if (loadRenderable("res/assets/renderable.toml", renderable, true) != LOAD_STATUS::OK)
		ASSERT(false);

	Renderer::AddCanvasLayer(CanvasLayerData(0, _RendererSettings::standard_vertex_pool_size, _RendererSettings::standard_index_pool_size));

	// Create actors
	RectRender* actor1 = new RectRender(Transform2D{ glm::vec2(-500.0f, 300.0f), -1.0f, glm::vec2(0.8f, 1.2f) }, textureFlag, shaderStandard32);
	RectRender* actor2 = new RectRender(Transform2D{ glm::vec2(400.0f, -200.0f), 0.25f, glm::vec2(0.7f, 0.7f) }, textureSnowman, shaderStandard32);
	RectRender* actor3 = new RectRender(Transform2D{ glm::vec2(0.0f, 0.0f), 0.0f, glm::vec2(1.0f, 1.0f) }, textureTux, shaderStandard32);

	Renderer::GetCanvasLayer(0)->OnAttach(actor1);
	Renderer::GetCanvasLayer(0)->OnAttach(actor2);

	Renderer::GetCanvasLayer(0)->OnSetZIndex(actor1, 1);

	Renderer::GetCanvasLayer(0)->OnAttach(actor3);

	Renderer::AddCanvasLayer(CanvasLayerData(-1, _RendererSettings::standard_vertex_pool_size, _RendererSettings::standard_index_pool_size));

	ActorPrimitive2D* actor4 = new ActorPrimitive2D(renderable, Transform2D{ glm::vec2(-200.0f, 0.0f), 0.0f, glm::vec2(800.0f, 800.0f) });
	Renderer::GetCanvasLayer(-1)->OnAttach(actor4);
	//actor4->SetPosition(0.0f, 0.0f);

	actor1->SetScale(16.0f, 16.0f);
	Renderer::GetCanvasLayer(0)->OnSetZIndex(actor3, -1);

	actor3->SetPivot(0.0f, 0.0f);
	//actor3->SetPosition(0.0f, 0.0f);
	actor3->SetPosition(-_RendererSettings::initial_window_width * 0.5f, -_RendererSettings::initial_window_height * 0.5f);
	actor3->SetScale(0.3f, 0.3f);
	//actor2->SetRotation(0.0f);
	//actor2->SetPosition(0.0f, 0.0f);
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

	if (tessel.ActorRef().index() == 0)
	{
		std::get<ActorPrimitive2D* const>(tessel.ActorRef())->SetModulationPerPoint({
			glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
			glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
			glm::vec4(0.0f, 0.0f, 0.0f, 0.0f),
			glm::vec4(0.0f, 0.0f, 0.0f, 0.0f),
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

	//Renderer::GetCanvasLayer(0)->OnDetach(&tesselDiagonal);
	//Renderer::GetCanvasLayer(0)->OnAttach(&tesselVertical);
	//Renderer::GetCanvasLayer(0)->OnAttach(&tessel);
	//Renderer::GetCanvasLayer(0)->OnAttach(actor3);
	
	//w = static_cast<float>(TextureFactory::GetWidth(textureSnowman));
	//h = static_cast<float>(TextureFactory::GetHeight(textureSnowman));
	//actor2->CropToRect({ 0.3f * w, 0.4f * h, 0.4f * w, 0.55f * h }, static_cast<int>(w), static_cast<int>(h));
	actor2->CropToRelativeRect({ 0.3f, 0.4f, 0.4f, 0.55f });
	
	// TODO DON'T create atlas directly. use AtlasFactory instead. Otherwise, when actor's desctructor is called, all the tiles referencing it will have hanging pointers. Therefore, there should be communication between AtlasFactory and TileFactory. Perhaps shared_ptr?
	//std::vector<TileHandle> tiles{ TextureFactory::GetTileHandle(textureSnowman), TextureFactory::GetTileHandle(textureTux) };
	//for (int i = 0; i < 20; i++)
		//tiles.push_back(TextureFactory::GetTileHandle(textureFlag));
	//Atlas atlas(tiles, 4096, 4096);
	
	//saveAtlas(atlas, "res/textures/atlas.png", "");

	//TextureHandle atlasTexture = TextureFactory::GetHandle(atlas);
	//RectRender* actor5 = new RectRender();
	//actor5->SetTextureHandle(atlasTexture);
	//actor5->SetShaderHandle(shaderStandard32);
	//actor5->SetPivot(0.5, 0.5);
	
	Renderer::GetCanvasLayer(0)->OnDetach(actor1);
	Renderer::GetCanvasLayer(0)->OnDetach(actor2);
	Renderer::GetCanvasLayer(0)->OnDetach(actor3);
	Renderer::GetCanvasLayer(0)->OnDetach(&tesselDiagonal);
	Renderer::GetCanvasLayer(-1)->OnDetach(actor4);

	//Renderer::GetCanvasLayer(0)->OnAttach(actor5);
	//actor5->SetScale(0.5, 0.5);
	//actor2->SetTextureHandle(textureAtlas);
	//actor2->SetModulation({ 1.0f, 1.0f, 1.0f, 1.0f });
	//actor2->ResetTransformUVs();
	//actor2->SetScale(0.25f, 0.25f);
	//actor5->SetScale(0.5f, 0.5f);


	// TODO default constructor of RectRender should only need to accept textureHandle. initial transform should be {}. initial shader handle should not be 0, but _RendererSettings::default_rect_render_shader_handle, which would be set in Renderer::Init() as shaderStandard32 (or shaderStandard8).
	//RectRender* renderTiles = new RectRender[6]{ RectRender({}, tex_dirtTL, shaderStandard32), RectRender({}, tex_dirtTR, shaderStandard32), RectRender({}, tex_grassSingle, shaderStandard32), RectRender({}, tex_grassTL, shaderStandard32), RectRender({}, tex_grassTE, shaderStandard32), RectRender({}, tex_grassTR, shaderStandard32) };
	TileHandle tile_dirtTL(TextureFactory::GetTileHandle(tex_dirtTL)), tile_dirtTR(TextureFactory::GetTileHandle(tex_dirtTR)), tile_grassSingle(TextureFactory::GetTileHandle(tex_grassSingle)), tile_grassTL(TextureFactory::GetTileHandle(tex_grassTL)), tile_grassTE(TextureFactory::GetTileHandle(tex_grassTE)), tile_grassTR(TextureFactory::GetTileHandle(tex_grassTR));
	std::vector<TileHandle> tiles = { tile_dirtTL, tile_dirtTR, tile_grassSingle, tile_grassTL, tile_grassTE, tile_grassTR };
	Atlas tileAtlas(tiles, -1, -1, 1);

	saveAtlas(tileAtlas, "res/textures/atlas.png", "");

	RectRender atlasRender1 = tileAtlas.SampleSubtile(3, Texture::nearest_settings, shaderStandard32);
	RectRender atlasRender2 = tileAtlas.SampleSubtile(4, Texture::nearest_settings, shaderStandard32);
	// TODO use tesselation instead of copying
	RectRender atlasRender3 = atlasRender2;
	RectRender atlasRender4 = atlasRender2;
	RectRender atlasRender5 = tileAtlas.SampleSubtile(5, Texture::nearest_settings, shaderStandard32);
	Renderer::GetCanvasLayer(0)->OnAttach(&atlasRender1);
	Renderer::GetCanvasLayer(0)->OnAttach(&atlasRender2);
	Renderer::GetCanvasLayer(0)->OnAttach(&atlasRender3);
	Renderer::GetCanvasLayer(0)->OnAttach(&atlasRender4);
	Renderer::GetCanvasLayer(0)->OnAttach(&atlasRender5);

	float side = 5.0f;

	atlasRender1.MultScale(side, side);
	atlasRender2.MultScale(side, side);
	atlasRender3.MultScale(side, side);
	atlasRender4.MultScale(side, side);
	atlasRender5.MultScale(side, side);

	float uvw = atlasRender1.GetUVWidth() * side;

	atlasRender1.SetPosition(-2 * uvw, 0);
	atlasRender2.SetPosition(-uvw, 0);
	atlasRender3.SetPosition(0, 0);
	atlasRender4.SetPosition(uvw, 0);
	atlasRender5.SetPosition(2 * uvw, 0);

	for (;;)
	{
		time = glfwGetTime();
		deltaTime = time - prevTime;
		prevTime = time;
		totalTime += deltaTime;
		// OnUpdate here

		// Render here
		Renderer::OnDraw();
		glfwPollEvents();
		if (glfwWindowShouldClose(window))
			break;
	}

	//delete[] renderTiles;
	delete actor1, actor2, actor3, actor4;
	//delete actor1, actor2, actor3, actor4, actor5;
}
