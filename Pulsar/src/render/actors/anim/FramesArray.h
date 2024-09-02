#pragma once

#include "utils/Array.inl"
#include "registry/Texture.h"
#include "Handles.inl"

class FramesArray
{
	Array<TileHandle> m_Frames;
	unsigned short m_Index = 0;

public:
	FramesArray() : m_Frames(), m_Index(0) {}
	FramesArray(const char* gif_filepath, const TextureSettings& settings = {}, unsigned short starting_index = 0, bool temporary_buffer = true);
	FramesArray(Array<TextureHandle>&& frames, unsigned short starting_index = 0);
	
	void Select(unsigned short i) { if (i < m_Frames.Size()) m_Index = i; }
	TextureHandle CurrentTexture() const { return m_Frames[m_Index]; }
	unsigned short CurrentIndex() const { return m_Index; }
	unsigned short Size() const { return static_cast<unsigned short>(m_Frames.Size()); }
	const Array<TextureHandle>& Arr() const { return m_Frames; }
};
