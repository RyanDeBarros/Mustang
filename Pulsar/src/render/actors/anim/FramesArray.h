#pragma once

#include "typedefs.h"
#include "utils/Array.inl"
#include "factory/Texture.h"

class FramesArray
{
	Array<TileHandle> m_Frames;
	unsigned short m_Index = 0;

public:
	inline FramesArray() : m_Frames(), m_Index(0) {}
	FramesArray(const char* gif_filepath, const TextureSettings& settings = {}, unsigned short starting_index = 0, bool temporary_buffer = true);
	FramesArray(Array<TextureHandle>&& frames, unsigned short starting_index = 0);
	
	inline void Select(unsigned short i) { if (i < m_Frames.Size()) m_Index = i; }
	inline TextureHandle CurrentTexture() const { return m_Frames[m_Index]; }
	inline unsigned short CurrentIndex() const { return m_Index; }
	inline unsigned short Size() const { return static_cast<unsigned short>(m_Frames.Size()); }
	inline const Array<TextureHandle>& Arr() const { return m_Frames; }
};
