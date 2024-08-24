#pragma once

#include "typedefs.h"
#include "utils/Array.inl"
#include "factory/Texture.h"

class FramesArray
{
	Array<TextureHandle> m_Frames;
	float m_Framelength;
	unsigned short m_Index = 0;

public:
	FramesArray(const char* gif_filepath, const TextureSettings& settings = {}, float framelength = 0.0f, unsigned short starting_index = 0);
	FramesArray(Array<TextureHandle>&& frames, float framelength = 0.0f, unsigned short starting_index = 0);

	inline void Select(unsigned short i) { if (i < m_Frames.Size()) m_Index = i; }
	inline TextureHandle CurrentTexture() const { return m_Frames[m_Index]; }
	inline unsigned short CurrentIndex() const { return m_Index; }
	inline unsigned short Size() const { return static_cast<unsigned short>(m_Frames.Size()); }
	inline float& FrameLength() { return m_Framelength; }
	inline float FrameLength() const { return m_Framelength; }

};
