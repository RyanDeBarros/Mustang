#pragma once

#include <unordered_map>

#include "Typedefs.h"
#include "Texture.h"

class TextureFactory
{
	static TextureHandle handle_cap;
	static std::unordered_map<TextureHandle, struct TextureElement>* factory;
	static Texture* Get(TextureHandle);
public:
	static void Init();
	static void Terminate();
	static TextureHandle GetHandle(const char* filepath, TextureSettings settings = TextureSettings(), GLint lod_level = 0);
	static void Bind(TextureHandle handle, TextureSlot slot);
	static void Unbind(TextureSlot slot);
};
