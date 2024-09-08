#include "NonantTile.h"

#include "../TileRegistry.h"
#include "utils/CommonMath.h"
#include "utils/Meta.inl"

struct _SubbufferBounds
{
	int LW, UW, LH, UH;
};

static _SubbufferBounds subbounds(int i, const NonantLines_Absolute& lines, int width, int height)
{
	switch (i)
	{
	case 0:
		return { 0, lines.column_l, 0, lines.row_b };
	case 1:
		return { lines.column_l, lines.column_r, 0, lines.row_b };
	case 2:
		return { lines.column_r, width, 0, lines.row_b };
	case 3:
		return { 0, lines.column_l, lines.row_b, lines.row_t };
	case 4:
		return { lines.column_l, lines.column_r, lines.row_b, lines.row_t };
	case 5:
		return { lines.column_r, width, lines.row_b, lines.row_t };
	case 6:
		return { 0, lines.column_l, lines.row_t, height };
	case 7:
		return { lines.column_l, lines.column_r, lines.row_t, height };
	case 8:
		return { lines.column_r, width, lines.row_t, height };
	default:
		return {};
	}
}

static int subbounds_area(const _SubbufferBounds& bounds, int bpp)
{
	return (bounds.UW - bounds.LW) * (bounds.UH - bounds.LH) * bpp;
}

static int sb_offset(int i, const NonantLines_Absolute& lines, int width, int height, int bpp)
{
	int offset = 0;
	switch (i)
	{
	case 8:
		offset += subbounds_area(subbounds(7, lines, width, height), bpp);
		[[fallthrough]];
	case 7:
		offset += subbounds_area(subbounds(6, lines, width, height), bpp);
		[[fallthrough]];
	case 6:
		offset += subbounds_area(subbounds(5, lines, width, height), bpp);
		[[fallthrough]];
	case 5:
		offset += subbounds_area(subbounds(4, lines, width, height), bpp);
		[[fallthrough]];
	case 4:
		offset += subbounds_area(subbounds(3, lines, width, height), bpp);
		[[fallthrough]];
	case 3:
		offset += subbounds_area(subbounds(2, lines, width, height), bpp);
		[[fallthrough]];
	case 2:
		offset += subbounds_area(subbounds(1, lines, width, height), bpp);
		[[fallthrough]];
	case 1:
		offset += subbounds_area(subbounds(0, lines, width, height), bpp);
	}
	return offset;
}

static void copy_subtile(TileHandle& th, unsigned char* shared_buffer, unsigned char const* original_buffer, int bpp, int offset, int og_stride,
	const _SubbufferBounds& bounds)
{
	int w = bounds.UW - bounds.LW;
	int h = bounds.UH - bounds.LH;
	int stride = w * bpp;
	// TODO use regular memcpy over memcpy_s more often
	for (size_t r = 0; r < h; ++r)
		memcpy(shared_buffer + offset + r * stride, original_buffer + bounds.LW * bpp + (r + bounds.LH) * og_stride, stride);
	th = TileRegistry::RegisterTile(Tile(shared_buffer + offset, w, h, bpp, TileDeletionPolicy::FROM_EXTERNAL));
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

		unfurl_loop<9>([this, original_buffer, og_stride](int i)
			{ copy_subtile(GetTileAtIndex(i), sharedBuffer, original_buffer, bpp,
				sb_offset(i, lines, width, height, bpp), og_stride, subbounds(i, lines, width, height)); });
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
	: sharedBuffer(other.sharedBuffer), tTL(other.tTL), tTM(other.tTM), tTR(other.tTR),
	tCL(other.tCL), tCM(other.tCM), tCR(other.tCR), tBL(other.tBL), tBM(other.tBM), tBR(other.tBR), lines(other.lines)
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
	lines = other.lines;
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

struct _RenewSubbufferRowInfo
{
	size_t sizeL = 0, sizeM = 0, sizeR = 0;
	unsigned char const* fromL = nullptr;
	unsigned char const* fromM = nullptr;
	unsigned char const* fromR = nullptr;
	int xL = 0, xM = 0, xR = 0;
	int bpp = 0, substride = 0;
	int y = 0;
};

static void renew_subbuffer_row(char index, _RenewSubbufferRowInfo& info, int new_sb_y, int row_h, unsigned char* renewal,
	_SubbufferBounds* new_subbounds, int* new_sb_offset)
{
	int wL = new_subbounds[index].UW - new_subbounds[index].LW;
	unsigned char* toL = renewal + new_sb_offset[index] + (info.xL + new_sb_y * wL) * info.bpp;

	++index;
	int wM = new_subbounds[index].UW - new_subbounds[index].LW;
	unsigned char* toM = renewal + new_sb_offset[index] + (info.xM + new_sb_y * wM) * info.bpp;

	++index;
	int wR = new_subbounds[index].UW - new_subbounds[index].LW;
	unsigned char* toR = renewal + new_sb_offset[index] + (info.xR + new_sb_y * wR) * info.bpp;

	while (info.y < row_h)
	{
		memcpy(toL, info.fromL, info.sizeL);
		memcpy(toM, info.fromM, info.sizeM);
		memcpy(toR, info.fromR, info.sizeR);
		toL += wL * info.bpp;
		toM += wM * info.bpp;
		toR += wR * info.bpp;
		info.fromL += info.substride;
		info.fromM += info.substride;
		info.fromR += info.substride;
		++info.y;
		++new_sb_y;
	}
}

static void renew_subbuffer(const NonantTile& ntile, unsigned char* renewal, int old_sb_offset,
	const _SubbufferBounds& old_subbounds, int* new_sb_offset, _SubbufferBounds* new_subbounds, const NonantLines_Absolute& new_lines)
{
	unsigned char const* subbuffer = ntile.GetSharedBuffer() + old_sb_offset;
	
	int row_b_h = clamp(old_subbounds.LH, old_subbounds.UH, new_lines.row_b);
	int row_c_h = clamp(old_subbounds.LH, old_subbounds.UH, new_lines.row_t);
	int row_t_h = old_subbounds.UH;
	int col_l_stride = ntile.GetBPP() * clamp(old_subbounds.LW, old_subbounds.UW, new_lines.column_l);
	int col_m_stride = ntile.GetBPP() * clamp(old_subbounds.LW, old_subbounds.UW, new_lines.column_r);
	int col_r_stride = ntile.GetBPP() * old_subbounds.UW;
	
	_RenewSubbufferRowInfo info;
	info.xL = old_subbounds.LW; // std::max(old_subbounds.LW - 0, 0)
	info.xM = std::max(old_subbounds.LW - new_lines.column_l, 0);
	info.xR = std::max(old_subbounds.LW - new_lines.column_r, 0);
	info.bpp = ntile.GetBPP();
	info.substride = (old_subbounds.UW - old_subbounds.LW) * info.bpp;
	info.sizeL = static_cast<size_t>(col_l_stride - old_subbounds.LW * info.bpp);
	info.sizeM = static_cast<size_t>(col_m_stride - col_l_stride);
	info.sizeR = static_cast<size_t>(col_r_stride - col_m_stride);
	info.fromL = subbuffer;
	info.fromM = subbuffer + col_l_stride - old_subbounds.LW * info.bpp;
	info.fromR = subbuffer + col_m_stride - old_subbounds.LW * info.bpp;
	info.y = old_subbounds.LH;
	
	renew_subbuffer_row(0, info, info.y, row_b_h, renewal, new_subbounds, new_sb_offset);
	renew_subbuffer_row(3, info, info.y - new_lines.row_b, row_c_h, renewal, new_subbounds, new_sb_offset);
	renew_subbuffer_row(6, info, info.y - new_lines.row_t, row_t_h, renewal, new_subbounds, new_sb_offset);
}

void NonantTile::SetTileProps(TileHandle th, int offset, const _SubbufferBounds& bounds) const
{
	Tile* tile = const_cast<Tile*>(TileRegistry::Get(th));
	tile->m_Width = bounds.UW - bounds.LW;
	tile->m_Height = bounds.UH - bounds.LH;
	tile->m_ImageBuffer = sharedBuffer + offset;
	offset += tile->m_Width * bpp * tile->m_Height;
}

void NonantTile::RenewSharedBuffer(const NonantLines_Absolute& new_lines)
{
	if (sharedBuffer)
	{
		unsigned char* renewal = new unsigned char[width * bpp * height];

		int old_sb_offset[9]{
			sb_offset(0, lines, width, height, bpp),
			sb_offset(1, lines, width, height, bpp),
			sb_offset(2, lines, width, height, bpp),
			sb_offset(3, lines, width, height, bpp),
			sb_offset(4, lines, width, height, bpp),
			sb_offset(5, lines, width, height, bpp),
			sb_offset(6, lines, width, height, bpp),
			sb_offset(7, lines, width, height, bpp),
			sb_offset(8, lines, width, height, bpp)
		};
		int new_sb_offset[9]{
			sb_offset(0, new_lines, width, height, bpp),
			sb_offset(1, new_lines, width, height, bpp),
			sb_offset(2, new_lines, width, height, bpp),
			sb_offset(3, new_lines, width, height, bpp),
			sb_offset(4, new_lines, width, height, bpp),
			sb_offset(5, new_lines, width, height, bpp),
			sb_offset(6, new_lines, width, height, bpp),
			sb_offset(7, new_lines, width, height, bpp),
			sb_offset(8, new_lines, width, height, bpp)
		};
		_SubbufferBounds old_subbounds[9]{
			subbounds(0, lines, width, height),
			subbounds(1, lines, width, height),
			subbounds(2, lines, width, height),
			subbounds(3, lines, width, height),
			subbounds(4, lines, width, height),
			subbounds(5, lines, width, height),
			subbounds(6, lines, width, height),
			subbounds(7, lines, width, height),
			subbounds(8, lines, width, height)
		};
		_SubbufferBounds new_subbounds[9]{
			subbounds(0, new_lines, width, height),
			subbounds(1, new_lines, width, height),
			subbounds(2, new_lines, width, height),
			subbounds(3, new_lines, width, height),
			subbounds(4, new_lines, width, height),
			subbounds(5, new_lines, width, height),
			subbounds(6, new_lines, width, height),
			subbounds(7, new_lines, width, height),
			subbounds(8, new_lines, width, height)
		};

		unfurl_loop<9>([this, renewal, &new_lines, &old_sb_offset, &old_subbounds, &new_subbounds, &new_sb_offset](int i)
			{ renew_subbuffer(*this, renewal, old_sb_offset[i], old_subbounds[i], new_sb_offset, new_subbounds, new_lines); });
		delete[] sharedBuffer;
		sharedBuffer = renewal;
		unfurl_loop<9>([this, &new_lines, &new_sb_offset, &new_subbounds](int i)
			{ SetTileProps(GetTileAtIndex(i), new_sb_offset[i], new_subbounds[i]); });

		lines = new_lines;
	}
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
