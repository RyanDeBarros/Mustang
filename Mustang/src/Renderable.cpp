#include "Renderable.h"

#include "Utility.h"

Renderable::Renderable(const char* asset_filepath)
	: m_RID(0), m_Zindex(0)
{
	LoadAsset(asset_filepath);
}

Renderable::~Renderable()
{
}

void Renderable::OnDraw()
{
}

void Renderable::LoadAsset(const char* asset_filepath)
{
	const char* dot = strrchr(asset_filepath, '.');
	ASSERT(dot && dot != asset_filepath && strcmp(dot + 1, "mass") == 0);

	const char* content = Utility::ReadFile(asset_filepath);
	// Load content
}
