/******************************************************************************
* Copyright (c) 2019 Alan Witkowski
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
#include <ae/atlas.h>
#include <ae/texture.h>
#include <fstream>

namespace ae {

// Constructor
_Atlas::_Atlas(const _Texture *Texture, const glm::vec2 &Size, float Padding) :
	Texture(Texture),
	Size(Size),
	TexelSize(1.0f / (glm::vec2)Texture->Size),
	TextureSizeInTexels(Size / (glm::vec2)Texture->Size),
	Padding(Padding) {

	Columns = (uint32_t)(Texture->Size.x / Size.x);
}

// Destructor
_Atlas::~_Atlas() {
}

// Load tile map that describes tile hierarchy and id map
void _Atlas::LoadTileMap(const std::string &Path) {

	// Load file
	std::ifstream File(Path.c_str(), std::ios::in);
	if(!File)
		throw std::runtime_error("Error loading: " + Path);

	// Skip header
	File.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	// Read table
	while(!File.eof() && File.peek() != EOF) {
		_TileData TileData;

		// Read data
		std::getline(File, TileData.ID, '\t');
		File >> TileData.Index >> TileData.Hierarchy;
		File.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		// Check for duplicates
		if(TileMap.find(TileData.ID) != TileMap.end())
			throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " - Duplicate entry: " + TileData.ID);

		TileMap[TileData.ID] = TileData;
		TileMapIndex[TileData.Index] = &TileMap[TileData.ID];
	}

	File.close();
}

// Returns coords given a texture index
glm::vec4 _Atlas::GetTextureCoords(uint32_t Index) const {
	float X = Index % Columns;
	float Y = Index / Columns;

	float TexelOffsetX = TexelSize.x + X * (Size.x + Padding * 2.0f) * TexelSize.x;
	float TexelOffsetY = TexelSize.y + Y * (Size.y + Padding * 2.0f) * TexelSize.y;

	return glm::vec4(TexelOffsetX, TexelOffsetY, TexelOffsetX + TextureSizeInTexels.x, TexelOffsetY + TextureSizeInTexels.y);
}

}
