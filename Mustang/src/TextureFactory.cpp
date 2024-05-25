#include "TextureFactory.h"

#include "Logger.h"
#include "Utility.h"

struct TextureElement
{
	Texture* texture;
	char* filepath;
	TextureSettings* settings;
	GLint* lod_level;
	TextureElement()
		: texture(nullptr), filepath(nullptr), settings(nullptr), lod_level(nullptr)
	{}
	TextureElement(TextureElement&& move) noexcept
		: texture(move.texture), filepath(move.filepath), settings(move.settings), lod_level(move.lod_level)
	{
		move.texture = nullptr;
		move.filepath = nullptr;
		move.settings = nullptr;
		move.lod_level = nullptr;
	}
	TextureElement(const char* filepath_, TextureSettings settings_, GLint lod_level_)
		: texture(new Texture(filepath_, settings_, lod_level_))
	{
		size_t length = strlen(filepath_);
		filepath = new char[length + 1];
		strcpy_s(filepath, length + 1, filepath_);
		filepath[length] = '\0';
		settings = new TextureSettings(settings_);
		lod_level = new GLint(lod_level_);
	}
	~TextureElement()
	{
		delete texture;
		delete[] filepath;
		delete settings;
		delete lod_level;
	}
};

static void null_factory()
{
	Logger::LogErrorFatal("TextureFactory is not initialized. Call TextureFactory::Init() before application loop.");
}

TextureHandle TextureFactory::handle_cap;
std::unordered_map<TextureHandle, struct TextureElement>* TextureFactory::factory;

Texture* TextureFactory::Get(TextureHandle handle)
{
	if (!factory)
	{
		null_factory();
		return nullptr;
	}
	auto iter = factory->find(handle);
	if (iter != factory->end())
		return iter->second.texture;
	else
	{
		Logger::LogWarning("Texture handle (" + std::to_string(handle) + ") does not exist in TextureFactory.");
		return nullptr;
	}
}

void TextureFactory::Init()
{
	handle_cap = 1;
	factory = new std::unordered_map<TextureHandle, TextureElement>();
}

void TextureFactory::Terminate()
{
	if (factory)
	{
		factory->clear();
		delete factory;
	}
}

TextureHandle TextureFactory::GetHandle(const char* filepath, TextureSettings settings, GLint lod_level)
{
	if (!factory)
	{
		null_factory();
		return 0;
	}
	for (const auto& [handle, element] : *factory)
	{
		if (lod_level = *element.lod_level && settings == *element.settings && strcmp(filepath, element.filepath) == 0)
			return handle;
	}
	TextureElement element(filepath, settings, lod_level);
	if (element.texture->IsValid())
	{
		TextureHandle handle = handle_cap++;
		factory->emplace(handle, TextureElement(std::move(element)));
		return handle;
	}
	else
		return 0;
}

void TextureFactory::Bind(TextureHandle handle, TextureSlot slot)
{
	Texture* texture = Get(handle);
	if (texture)
		texture->Bind(slot);
	else
		Logger::LogWarning("Failed to bind texture at handle (" + std::to_string(handle) + ") to slot (" + std::to_string(slot) + ").");
}

void TextureFactory::Unbind(TextureSlot slot)
{
	if (!factory)
	{
		null_factory();
		return;
	}
	TRY(glActiveTexture(GL_TEXTURE0 + slot));
	TRY(glBindTexture(GL_TEXTURE_2D, 0));
}
