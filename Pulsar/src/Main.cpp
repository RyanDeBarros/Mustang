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
#include "render/AtlasPrimitive.h"

void run(GLFWwindow*);

void window_refresh_callback(GLFWwindow* window)
{
	Renderer::OnDraw();
	TRY(glFinish()); // important, this waits until rendering result is actually visible, thus making resizing less ugly
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// TODO
	// If _RendererSettings::resize_mode is set to SCALE_IGNORE_ASPECT_RATIO, don't add anything.
	// If it is set to SCALE_KEEP_ASPECT_RATIO, call new Renderer function that will scale objects as usual without stretching their aspect ratios.
	// If it is set to NO_SCALE_KEEP_SIZE, call new Renderer function that will not scale objects - only display more of the scene.
	TRY(glViewport(0, 0, width, height));
}

int main()
{
	return Pulsar::StartUp();
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
	Renderer::Terminate();
	glfwTerminate();
	return 0;
}

void run(GLFWwindow* window)
{
	double time = glfwGetTime();
	double deltaTime = 0;
	double prevTime = time;
	double totalTime = 0;

	// Load shaders
	ShaderHandle shaderStandard32, shaderAtlas32;
	if (loadShader(_RendererSettings::standard_shader32_assetfile, shaderStandard32) != LOAD_STATUS::OK)
		ASSERT(false);
	if (loadShader(_RendererSettings::standard_atlas_shader32_assetfile, shaderAtlas32) != LOAD_STATUS::OK)
		ASSERT(false);

	// Load textures
	TextureHandle textureSnowman, textureTux, textureFlag;
	if (loadTexture("res/assets/snowman.toml", textureSnowman) != LOAD_STATUS::OK)
		ASSERT(false);
	if (loadTexture("res/assets/tux.toml", textureTux) != LOAD_STATUS::OK)
		ASSERT(false);
	if (loadTexture("res/assets/flag.toml", textureFlag, true) != LOAD_STATUS::OK)
		ASSERT(false);

	// Load renderables
	Renderable renderable;
	if (loadRenderable("res/assets/renderable.toml", renderable, true) != LOAD_STATUS::OK)
		ASSERT(false);

	Renderer::AddCanvasLayer(CanvasLayerData(0, _RendererSettings::standard_vertex_pool_size, _RendererSettings::standard_index_pool_size));

	// Create actors

	RectRender* actor1 = new RectRender(Transform2D{ glm::vec2(-500.0f, 300.0f), -1.0f, glm::vec2(0.8f, 1.2f) }, textureFlag, shaderStandard32);

	AtlasPrimitive* actor2 = new AtlasPrimitive(Transform2D{ glm::vec2(400.0f, -200.0f), 0.25f, glm::vec2(0.7f, 0.7f) }, textureSnowman, shaderStandard32);
	
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

	TextureFactory::SetSettings(actor1->GetTextureHandle(), { MinFilter::Linear, MagFilter::Linear, TextureWrap::ClampToEdge, TextureWrap::ClampToEdge });


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

	Renderer::GetCanvasLayer(0)->OnDetach(&tesselDiagonal);
	//Renderer::GetCanvasLayer(0)->OnAttach(&tesselVertical);
	//Renderer::GetCanvasLayer(0)->OnAttach(&tessel);
	//Renderer::GetCanvasLayer(0)->OnAttach(actor3);
	
	actor2->SetShaderHandle(shaderAtlas32);
	actor2->SetAtlasTexel({0.0f, 0.0f, 1.0f, 1.0f}, TextureFactory::GetWidth(textureSnowman), TextureFactory::GetHeight(textureSnowman));

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

	delete actor1, actor2, actor3, actor4;
}
