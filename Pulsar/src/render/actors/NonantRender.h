#pragma once

#include "RectRender.h"

// TODO put somewhere else
struct UVBounds
{
	float x1, x2, y1, y2;

	UVBounds(float x1, float x2, float y1, float y2) : x1(x1), x2(x2), y1(y1), y2(y2) {}

	template<char i>
	glm::vec2 get() const requires (i >= 0 && i <= 4)
	{
		if constexpr (i == 0) return { x1, y1 };
		else if constexpr (i == 1) return { x2, y1 };
		else if constexpr (i == 2) return { x2, y2 };
		else if constexpr (i == 3) return { x1, y2 };
		else static_assert(false);
	}

	template<char i>
	float getX() const requires (i >= 0 && i <= 4)
	{
		if constexpr (i == 0 || i == 3) return x1;
		else if constexpr (i == 1 || i == 2) return x2;
		else static_assert(false);
	}

	template<char i>
	float getY() const requires (i >= 0 && i <= 4)
	{
		if constexpr (i == 0 || i == 1) return y1;
		else if constexpr (i == 2 || i == 3) return y2;
		else static_assert(false);
	}
};

struct NonantLines
{
	float col_l_width;
	float col_r_width;
	float row_b_height;
	float row_t_height;

	NonantLines(float col_l_width, float col_r_width, float row_b_height, float row_t_height)
		: col_l_width(col_l_width), col_r_width(col_r_width), row_b_height(row_b_height), row_t_height(row_t_height) {}
	NonantLines(float col_l_width, float col_r_width, float row_b_height, float row_t_height, int width, int height)
		: col_l_width(col_l_width * width), col_r_width(col_r_width * width),
		row_b_height(row_b_height * height), row_t_height(row_t_height * height) {}
	
	template<char i>
	UVBounds UVs(float width, float height) const requires (i >= 0 && i <= 8);
};

class NonantRender : public RectRender
{
	float nonantWidth;
	float nonantHeight;
	float nonantXF = 1.0f, nonantYF = 1.0f;

public:
	NonantRender(TextureHandle texture, const NonantLines& lines, const glm::vec2& pivot = { 0.5f, 0.5f },
		ShaderHandle shader = ShaderRegistry::HANDLE_CAP, ZIndex z = 0, bool modulatable = true, bool visible = true);
	NonantRender(const NonantRender&) = delete; // TODO
	NonantRender(NonantRender&&) = delete; // TODO

	NonantLines lines;

	void RequestDraw(class CanvasLayer* canvas_layer) override;

	void SetPivot(float x, float y) override { m_Pivot = { x, y }; }
	void SetPivot(const glm::vec2& pivot) override { m_Pivot = pivot; }
	
	float GetNonantWidth() const { return nonantWidth; }
	float GetNonantHeight() const { return nonantHeight; }
	void SetNonantWidth(float width);
	void SetNonantHeight(float height);

private:
	template<char i>
	void Draw(class CanvasLayer* canvas_layer, Stride stride, const PackedTransform2D& prior) requires(i >= 0 && i <= 8);
	template<char i>
	UVBounds SetUVs(Stride stride) requires (i >= 0 && i <= 8);
	template<char i>
	Transform2D CallibrateTransform(const UVBounds& uvs) requires (i >= 0 && i <= 8);
};
