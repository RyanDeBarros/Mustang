#pragma once

#include "registry/compound/NonantTile.h"
#include "RectRender.h"

// TODO IMPORTANT: NonantRender creates a nine-patch image buffer. Instead, a much better option would be to just have the single image buffer,
// as well as a single RectRender that tesselates, in which each patch uses different UVs. In fact, completely remove NonantTile.
// This will also make it viable to animate the lines.
class NonantRender : public FickleActor2D
{
	NonantTile ntile; // Create NonantTileRegistry and NonantTileHandle, since NonantTile's own buffers.
	RectRender rects[9];	// TODO array causes size of NonantRender to be > 1500 bytes.
							// Create an alternative to RectRender, since lots of things like shader/pivot are redundant in the array.
							// Also, things like cropping are unnecessary for NonantRender, at least for the individual patches.
	float nonantWidth;
	float nonantHeight;
	glm::vec2 pivot = { 0.5f, 0.5f };

public:
	NonantRender(NonantTile&& ntile, TextureVersion texture_version = 0, const TextureSettings& texture_settings = { MinFilter::Nearest, MagFilter::Nearest },
		ShaderHandle shader = ShaderRegistry::HANDLE_CAP, ZIndex z = 0, bool modulatable = true, bool visible = true);
	NonantRender(const NonantRender&) = delete; // TODO
	NonantRender(NonantRender&&) = delete; // TODO

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

	const NonantTile& NTile() const { return ntile; }

	glm::vec2 GetPivot() const { return pivot; }
	void SetPivot(const glm::vec2& pivot);
	float GetNonantWidth() const { return nonantWidth; }
	float GetNonantHeight() const { return nonantHeight; }
	void SetNonantWidth(float width);
	void SetNonantHeight(float height);
	void SetNonantSize(const glm::vec2& size);

private:
	void RecallibratePosition();
};
