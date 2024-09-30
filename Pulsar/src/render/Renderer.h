#pragma once

#include <unordered_map>

#include "CanvasLayer.h"
#include "registry/Shader.h"
#include "registry/Texture.h"
#include "registry/Tile.h"
#include "registry/UniformLexicon.h"

class Window;
class ShaderRegistry;
class TextureRegistry;
class TileRegistry;
class UniformLexiconRegistry;

class Renderer
{
	static std::map<CanvasIndex, CanvasLayer> layers;

	static ShaderRegistry* shaders;
	static TextureRegistry* textures;
	static TileRegistry* tiles;
	static UniformLexiconRegistry* uniform_lexicons;

public:
	static void Init();
	static void Terminate();
	static void OnDraw();
	static void FocusWindow(WindowHandle);
	static void _SetClearColor();
	static void AddCanvasLayer(const CanvasLayerData&);
	static void RemoveCanvasLayer(CanvasIndex);
	static CanvasLayer* GetCanvasLayer(CanvasIndex);
	static void ChangeCanvasLayerIndex(CanvasIndex old_index, CanvasIndex new_index);

	static ShaderRegistry& Shaders() { return *shaders; }
	static TextureRegistry& Textures() { return *textures; }
	static TileRegistry& Tiles() { return *tiles; }
	static UniformLexiconRegistry& UniformLexicons() { return *uniform_lexicons; }
};
