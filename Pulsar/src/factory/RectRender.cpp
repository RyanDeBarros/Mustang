#include "RectRender.h"

#include <string>

#include "RendererSettings.h"
#include "AssetLoader.h"
#include "Logger.h"

Renderable RectRender::rect_renderable;

RectRender::RectRender(const Transform2D& transform, ZIndex z, bool visible)
	: ActorPrimitive2D(rect_renderable, transform, z, visible)
{
}

void RectRender::DefineRectRenderable()
{
	LOAD_STATUS load_status = loadRenderable(_RendererSettings::rect_renderable_filepath.c_str(), rect_renderable);
	if (load_status != LOAD_STATUS::OK)
	{
		Logger::LogErrorFatal("Could not load quad renderable. Load Status = " + std::to_string(static_cast<int>(load_status)));
	}
}
