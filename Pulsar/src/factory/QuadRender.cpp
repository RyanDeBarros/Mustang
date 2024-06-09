#include "QuadRender.h"

#include <string>

#include "RendererSettings.h"
#include "AssetLoader.h"
#include "Logger.h"

Renderable QuadRender::quad_renderable;

QuadRender::QuadRender(const Transform2D& transform, ZIndex z, bool visible)
	: ActorPrimitive2D(quad_renderable, transform, z, visible)
{
}

void QuadRender::DefineQuadRenderable()
{
	LOAD_STATUS load_status = loadRenderable(_RendererSettings::quad_renderable_filepath.c_str(), quad_renderable);
	if (load_status != LOAD_STATUS::OK)
	{
		Logger::LogErrorFatal("Could not load quad renderable. Load Status = " + std::to_string(static_cast<int>(load_status)));
	}
}
