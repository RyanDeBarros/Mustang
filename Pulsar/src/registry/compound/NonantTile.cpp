#include "NonantTile.h"

#include "../TileRegistry.h"
#include "utils/CommonMath.h"

static void copy_subtile(TileHandle& th, unsigned char* shared_buffer, unsigned char const* original_buffer, int& sb_offset, int bpp, int og_stride,
	int lower_bound_w, int upper_bound_w, int lower_bound_h, int upper_bound_h)
{
	int w = upper_bound_w - lower_bound_w;
	int h = upper_bound_h - lower_bound_h;
	int stride = w * bpp;
	// TODO use regular memcpy over memcpy_s more often
	for (size_t r = 0; r < h; ++r)
		memcpy(shared_buffer + sb_offset + r * stride, original_buffer + lower_bound_w * bpp + (r + lower_bound_h) * og_stride, stride);
	th = TileRegistry::RegisterTile(Tile(shared_buffer + sb_offset, w, h, bpp, TileDeletionPolicy::FROM_EXTERNAL));
	sb_offset += stride * h;
}

void NonantTile::CreateSharedBuffer(const Tile& tile)
{
	if (unsigned char const* original_buffer = tile.GetImageBuffer())
	{
		bpp = tile.GetBPP();
		width = tile.GetWidth();
		height = tile.GetHeight();
		int og_stride = bpp * width;

		sharedBuffer = new unsigned char[og_stride * height];

		int sb_offset = 0;
		copy_subtile(tBL, sharedBuffer, original_buffer, sb_offset, bpp, og_stride, 0, lines.column_l, 0, lines.row_b);
		copy_subtile(tBM, sharedBuffer, original_buffer, sb_offset, bpp, og_stride, lines.column_l, lines.column_r, 0, lines.row_b);
		copy_subtile(tBR, sharedBuffer, original_buffer, sb_offset, bpp, og_stride, lines.column_r, width, 0, lines.row_b);
		copy_subtile(tCL, sharedBuffer, original_buffer, sb_offset, bpp, og_stride, 0, lines.column_l, lines.row_b, lines.row_t);
		copy_subtile(tCM, sharedBuffer, original_buffer, sb_offset, bpp, og_stride, lines.column_l, lines.column_r, lines.row_b, lines.row_t);
		copy_subtile(tCR, sharedBuffer, original_buffer, sb_offset, bpp, og_stride, lines.column_r, width, lines.row_b, lines.row_t);
		copy_subtile(tTL, sharedBuffer, original_buffer, sb_offset, bpp, og_stride, 0, lines.column_l, lines.row_t, height);
		copy_subtile(tTM, sharedBuffer, original_buffer, sb_offset, bpp, og_stride, lines.column_l, lines.column_r, lines.row_t, height);
		copy_subtile(tTR, sharedBuffer, original_buffer, sb_offset, bpp, og_stride, lines.column_r, width, lines.row_t, height);
	}
}

NonantTile::NonantTile(const Tile& tile, const NonantLines_Absolute& pos)
{
	if (pos.valid())
	{
		lines.column_l = clamp(0, tile.GetWidth(), pos.column_l);
		lines.column_r = clamp(0, tile.GetWidth(), pos.column_r);
		lines.row_b = clamp(0, tile.GetHeight(), pos.row_b);
		lines.row_t = clamp(0, tile.GetHeight(), pos.row_t);
		CreateSharedBuffer(tile);
	}
}

NonantTile::NonantTile(const Tile& tile, const NonantLines_Relative& pos)
{
	if (pos.valid())
	{
		lines.column_l = static_cast<int>(tile.GetWidth() * clamp(0.0f, 1.0f, pos.column_l));
		lines.column_r = static_cast<int>(tile.GetWidth() * clamp(0.0f, 1.0f, pos.column_r));
		lines.row_b = static_cast<int>(tile.GetHeight() * clamp(0.0f, 1.0f, pos.row_b));
		lines.row_t = static_cast<int>(tile.GetHeight() * clamp(0.0f, 1.0f, pos.row_t));
		CreateSharedBuffer(tile);
	}
}

NonantTile::NonantTile(NonantTile&& other) noexcept
	: sharedBuffer(other.sharedBuffer), tTL(other.tTL), tTM(other.tTM), tTR(other.tTR), tCL(other.tCL), tCM(other.tCM), tCR(other.tCR), tBL(other.tBL), tBM(other.tBM), tBR(other.tBR)
{
	other.sharedBuffer = nullptr;
}

NonantTile& NonantTile::operator=(NonantTile&& other) noexcept
{
	if (this == &other)
		return *this;
	if (sharedBuffer)
		DeleteBuffer();
	sharedBuffer = other.sharedBuffer;
	other.sharedBuffer = nullptr;
	tTL = other.tTL;
	tTM = other.tTM;
	tTR = other.tTR;
	tCL = other.tCL;
	tCM = other.tCM;
	tCR = other.tCR;
	tBL = other.tBL;
	tBM = other.tBM;
	tBR = other.tBR;
	return *this;
}

NonantTile::~NonantTile()
{
	if (sharedBuffer)
	{
		DeleteBuffer();
		sharedBuffer = nullptr;
	}
}

void NonantTile::RenewSharedBuffer(const NonantLines_Absolute& new_lines)
{
	// TODO
}

void NonantTile::Reconfigure(const NonantLines_Absolute& pos)
{
	if (pos.valid())
	{
		NonantLines_Absolute new_lines;
		new_lines.column_l = clamp(0, width, pos.column_l);
		new_lines.column_r = clamp(0, width, pos.column_r);
		new_lines.row_b = clamp(0, height, pos.row_b);
		new_lines.row_t = clamp(0, height, pos.row_t);
		RenewSharedBuffer(new_lines);
	}
}

void NonantTile::Reconfigure(const NonantLines_Relative& pos)
{
	if (pos.valid())
	{
		NonantLines_Absolute new_lines;
		new_lines.column_l = static_cast<int>(width * clamp(0.0f, 1.0f, pos.column_l));
		new_lines.column_r = static_cast<int>(width * clamp(0.0f, 1.0f, pos.column_r));
		new_lines.row_b = static_cast<int>(height * clamp(0.0f, 1.0f, pos.row_b));
		new_lines.row_t = static_cast<int>(height * clamp(0.0f, 1.0f, pos.row_t));
		RenewSharedBuffer(new_lines);
	}
}

void NonantTile::DeleteBuffer() const
{
	delete[] sharedBuffer;
	TileRegistry::DestroyTile(tTL);
	TileRegistry::DestroyTile(tTM);
	TileRegistry::DestroyTile(tTR);
	TileRegistry::DestroyTile(tCL);
	TileRegistry::DestroyTile(tCM);
	TileRegistry::DestroyTile(tCR);
	TileRegistry::DestroyTile(tBL);
	TileRegistry::DestroyTile(tBM);
	TileRegistry::DestroyTile(tBR);
}
