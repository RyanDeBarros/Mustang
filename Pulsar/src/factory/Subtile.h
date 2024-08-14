#include "Tile.h"

struct Subtile : public Tile
{
	int x;
	int y;
	unsigned int w;
	unsigned int h;
	Subtile(Tile* const tile, int x, int y, unsigned int w, unsigned int h)
		: x(x), y(y), w(w), h(h)
	{
		m_Filepath = tile->GetFilepath();
		m_Width = tile->GetWidth();
		m_Height = tile->GetHeight();
		m_BPP = tile->GetBPP();
		auto buffer_size = m_Width * m_Height * m_BPP;
		m_ImageBuffer = new unsigned char[buffer_size];
		memcpy_s(m_ImageBuffer, buffer_size, tile->GetImageBuffer(), buffer_size);
	}
	Subtile(const Subtile&) = delete;
	Subtile(Subtile&&) = delete;
	~Subtile()
	{
		if (m_ImageBuffer)
		{
			delete[] m_ImageBuffer;
			m_ImageBuffer = nullptr;
		}
	}
};
