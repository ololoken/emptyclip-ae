/******************************************************************************
* Copyright (c) 2024 Alan Witkowski
*
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
*    claim that you wrote the original software. If you use this software
*    in a product, an acknowledgment in the product documentation would be
*    appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
*    misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*******************************************************************************/
#pragma once

// Libraries
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <string>
#include <unordered_map>
#include <vector>

struct SDL_Cursor;

namespace ae {

// Forward Declarations
class _Atlas;
class _Element;
class _Font;
class _Mesh;
class _Music;
class _Program;
class _Shader;
class _Sound;
class _Texture;
class _TextureArray;
class _TileMap;
struct _Reel;
struct _Style;
struct _TextureSettings;

struct _Layer {
	int Layer{0};
	int DepthTest{0};
	int DepthMask{0};
	int EditorOnly{0};
};

// Classes
class _Assets {

	public:

		void Init();
		void Close();

		void LoadColors(const std::string &Path);
		void LoadTextureDirectory(const std::string &Path, const _TextureSettings &TextureSettings);
		void LoadTexturePack(const std::string &Path, const _TextureSettings &TextureSetting);
		void LoadAtlasDirectory(const std::string &Path, const glm::ivec2 &Size, float Padding);
		void LoadSounds(const std::string &Path);
		void LoadSoundPack(const std::string &Path);
		void LoadMusic(const std::string &Path);
		void LoadMeshDirectory(const std::string &Path);
		void LoadReels(const std::string &Path, bool SetNameOnly=false);
		void LoadAnimations(const std::string &Path);
		void LoadFonts(const std::string &Path, bool Preload=false);
		void LoadLayers(const std::string &Path);
		void LoadPrograms(const std::string &Path);
		void LoadStyles(const std::string &Path);
		void LoadCursor(const std::string &ID, const std::string &Path, const glm::ivec2 &Offset);
		void LoadUI(const std::string &Path, bool CalculateBounds=true);
		void SaveUI(const std::string &Path);

		std::unordered_map<std::string, _Font *> Fonts;
		std::unordered_map<std::string, _Layer> Layers;
		std::unordered_map<std::string, const _Texture *> Textures;
		std::unordered_map<std::string, _Atlas *> Atlases;
		std::unordered_map<std::string, _TextureArray *> TextureArrays;
		std::unordered_map<std::string, _TileMap *> TileMaps;
		std::unordered_map<std::string, const _Mesh *> Meshes;
		std::unordered_map<std::string, _Program *> Programs;
		std::unordered_map<std::string, const _Reel *> Reels;
		std::unordered_map<std::string, std::vector<const _Reel *> > Animations;
		std::unordered_map<std::string, glm::vec4> Colors;
		std::unordered_map<std::string, _Style *> Styles;
		std::unordered_map<std::string, _Sound *> Sounds;
		std::unordered_map<std::string, _Music *> Music;
		std::unordered_map<std::string, _Element *> Elements;
		std::unordered_map<std::string, SDL_Cursor *> Cursors;

	private:

		std::unordered_map<std::string, const _Shader *> Shaders;
};

extern _Assets Assets;

}
