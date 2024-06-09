#pragma once

#include <unordered_map>

#include "Typedefs.h"
#include "Texture.h"

class TextureFactory
{
	static TextureHandle handle_cap;
	static std::unordered_map<TextureHandle, struct TextureElement>* factory;
	static Texture* Get(TextureHandle);

	TextureFactory() = delete;
	~TextureFactory() = delete;

public:
	static void Init();
	static void Terminate();
	static TextureHandle GetHandle(const char* filepath, TextureSettings settings = TextureSettings());
	static void Bind(TextureHandle handle, TextureSlot slot);
	static void Unbind(TextureSlot slot);
};
