#pragma once

#include <stdexcept>

#include "Handles.inl"
#include "../Tile.h"

struct NonantLines_Absolute
{
	int column_l = 0;
	int column_r = 0;
	int row_b = 0;
	int row_t = 0;

	bool valid() const { return column_r >= column_l && row_t >= row_b; }
};

struct NonantLines_Relative
{
	float column_l = 0.0f;
	float column_r = 1.0f;
	float row_b = 0.0f;
	float row_t = 1.0f;

	bool valid() const { return column_r >= column_l && row_t >= row_b; }
};

class NonantTile
{
	unsigned char* sharedBuffer = nullptr;
	TileHandle tTL = 0, tTM = 0, tTR = 0, tCL = 0, tCM = 0, tCR = 0, tBL = 0, tBM = 0, tBR = 0;
	int width = 0, height = 0, bpp = 0;
	NonantLines_Absolute lines;

	void CreateSharedBuffer(const Tile& tile);
	void SetTileProps(TileHandle th, int offset, const struct _SubbufferBounds& bounds) const;
	void RenewSharedBuffer(const NonantLines_Absolute& new_lines);

public:
	NonantTile(const Tile& original_tile, const NonantLines_Absolute& pos);
	NonantTile(const Tile& original_tile, const NonantLines_Relative& pos);
	NonantTile(const NonantTile&) = delete; // TODO copy constructor and copy assignment operator
	NonantTile(NonantTile&&) noexcept;
	NonantTile& operator=(NonantTile&&) noexcept;
	~NonantTile();

	void Reconfigure(const NonantLines_Absolute& pos);
	void Reconfigure(const NonantLines_Relative& pos);

	int GetWidth() const { return width; }
	int GetHeight() const { return height; }
	int GetBPP() const { return bpp; }
	NonantLines_Absolute GetAbsoluteLines() const { return lines; }
	unsigned char const* GetSharedBuffer() const { return sharedBuffer; }
	TileHandle GetTopLeft() const { return tTL; }
	TileHandle GetTopMiddle() const { return tTM; }
	TileHandle GetTopRight() const { return tTR; }
	TileHandle GetCenterLeft() const { return tCL; }
	TileHandle GetCenterMiddle() const { return tCM; }
	TileHandle GetCenterRight() const { return tCR; }
	TileHandle GetBottomLeft() const { return tBL; }
	TileHandle GetBottomMiddle() const { return tBM; }
	TileHandle GetBottomRight() const { return tBR; }

	const TileHandle& GetTileAtIndex(int i) const
	{
		switch (i)
		{
		case 0:
			return tBL;
		case 1:
			return tBM;
		case 2:
			return tBR;
		case 3:
			return tCL;
		case 4:
			return tCM;
		case 5:
			return tCR;
		case 6:
			return tTL;
		case 7:
			return tTM;
		case 8:
			return tTR;
		default:
			throw std::out_of_range("Nonant tile index must be in range 0-9.");
		}
	}

	int ColumnPos(int i) const
	{
		if (i == 0)
			return 0;
		else if (i == 1)
			return lines.column_l;
		else if (i == 2)
			return lines.column_r;
		else
			return width;
	}

	int RowPos(int i) const
	{
		if (i == 0)
			return 0;
		else if (i == 1)
			return lines.row_b;
		else if (i == 2)
			return lines.row_t;
		else
			return height;
	}

private:
	void DeleteBuffer() const;
	TileHandle& GetTileAtIndex(int i)
	{
		switch (i)
		{
		case 0:
			return tBL;
		case 1:
			return tBM;
		case 2:
			return tBR;
		case 3:
			return tCL;
		case 4:
			return tCM;
		case 5:
			return tCR;
		case 6:
			return tTL;
		case 7:
			return tTM;
		case 8:
			return tTR;
		default:
			throw std::out_of_range("Nonant tile index must be in range 0-9.");
		}
	}
};
