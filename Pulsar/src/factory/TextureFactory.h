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

	friend class Renderer;
	static void Init();
	static void Terminate();

public:
	static TextureHandle GetHandle(const char* filepath, TextureSettings settings = TextureSettings());
	static void Bind(const TextureHandle& handle, const TextureSlot& slot);
	static void Unbind(const TextureSlot& slot);
	inline static int GetWidth(const TextureHandle& handle) { Texture* texture = Get(handle); return texture ? texture->GetWidth() : 0; }
	inline static int GetHeight(const TextureHandle& handle) { Texture* texture = Get(handle); return texture ? texture->GetHeight() : 0; }
};
