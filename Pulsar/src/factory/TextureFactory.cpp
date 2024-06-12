#include "TextureFactory.h"

#include "Logger.h"
#include "Macros.h"

// TODO remove TextureElement. Just use map of TextureHandle to Texture*, and use texture->equivalent(filepath, settings) to check for existence without constructing.
struct TextureElement
{
	Texture* texture;
	char* filepath;
	TextureSettings* settings;
	TextureElement()
		: texture(nullptr), filepath(nullptr), settings(nullptr)
	{}
	TextureElement(TextureElement&& move) noexcept
		: texture(move.texture), filepath(move.filepath), settings(move.settings)
	{
		move.texture = nullptr;
		move.filepath = nullptr;
		move.settings = nullptr;
	}
	TextureElement(const char* filepath_, TextureSettings settings_)
		: texture(new Texture(filepath_, settings_))
	{
		size_t length = strlen(filepath_);
		filepath = new char[length + 1];
		strcpy_s(filepath, length + 1, filepath_);
		filepath[length] = '\0';
		settings = new TextureSettings(settings_);
	}
	~TextureElement()
	{
		delete texture;
		delete[] filepath;
		delete settings;
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

TextureHandle TextureFactory::GetHandle(const char* filepath, TextureSettings settings)
{
	if (!factory)
	{
		null_factory();
		return 0;
	}
	for (const auto& [handle, element] : *factory)
	{
		if (settings == *element.settings && strcmp(filepath, element.filepath) == 0)
			return handle;
	}
	TextureElement element(filepath, settings);
	if (element.texture->IsValid())
	{
		TextureHandle handle = handle_cap++;
		factory->emplace(handle, TextureElement(std::move(element)));
		return handle;
	}
	else
		return 0;
}

void TextureFactory::Bind(const TextureHandle& handle, const TextureSlot& slot)
{
	Texture* texture = Get(handle);
	if (texture)
		texture->Bind(slot);
	else
		Logger::LogWarning("Failed to bind texture at handle (" + std::to_string(handle) + ") to slot (" + std::to_string(slot) + ").");
}

void TextureFactory::Unbind(const TextureSlot& slot)
{
	if (!factory)
	{
		null_factory();
		return;
	}
	TRY(glActiveTexture(GL_TEXTURE0 + slot));
	TRY(glBindTexture(GL_TEXTURE_2D, 0));
}
