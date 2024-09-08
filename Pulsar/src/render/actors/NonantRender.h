#pragma once

#include "registry/compound/NonantTile.h"
#include "RectRender.h"

class NonantRender : public FickleActor2D
{
	NonantTile ntile;
	RectRender rects[9];

public:
	NonantRender(NonantTile&& ntile, TextureVersion texture_version = 0, const TextureSettings& texture_settings = { MinFilter::Nearest, MagFilter::Nearest },
		ShaderHandle shader = ShaderRegistry::standard_shader, ZIndex z = 0, FickleType fickle_type = FickleType::Protean, bool visible = true);

	bool IsVisible() const { return rects[0].IsVisible(); }
	void SetVisible(bool visible)
	{
		rects[0].SetVisible(visible);
		rects[1].SetVisible(visible);
		rects[2].SetVisible(visible);
		rects[3].SetVisible(visible);
		rects[4].SetVisible(visible);
		rects[5].SetVisible(visible);
		rects[6].SetVisible(visible);
		rects[7].SetVisible(visible);
		rects[8].SetVisible(visible);
	}

	void RequestDraw(class CanvasLayer* canvas_layer) override;
	void Reconfigure(const NonantLines_Absolute& lines);
	void Reconfigure(const NonantLines_Relative& lines);
	void Recallibrate();

	// TODO SetWidth() SetHeight() SetSize() that scales inner tiles only.

private:
	void RecallibrateFickler();
};
