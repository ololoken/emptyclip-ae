/******************************************************************************
* Copyright (c) 2023 Alan Witkowski
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

#include <ae/opengl.h>
#include <glm/vec2.hpp>
#include <string>

struct SDL_Surface;

namespace ae {

struct _TextureSettings {
	_TextureSettings() : SetNameOnly(false), Mipmaps(false), Nearest(false) {}

	float Anisotropy{1.0f};
	int WrapMode{1};
	bool SetNameOnly : 1;
	bool Mipmaps : 1;
	bool Nearest : 1;
};

// Classes
class _Texture {

	public:

		enum WrapModeType {
			REPEAT,
			CLAMP_TO_EDGE,
			CLAMP_TO_BORDER,
		};

		_Texture(const std::string &Path) : Name(Path) {}
		_Texture(const std::string &Path, const _TextureSettings &TextureSettings);
		_Texture(const std::string &Path, FILE *FileHandle, const _TextureSettings &TextureSettings);
		_Texture(unsigned char *Data, const glm::ivec2 &Size, int InternalFormat, GLenum Format);
		~_Texture();

		void UpdateAnisotropicFiltering(float Value) const;

		// Info
		std::string Name;
		GLuint ID{0};
		bool Mipmaps{false};

		// Dimensions
		glm::ivec2 Size{0};

	private:

		void Load(SDL_Surface *Image, const _TextureSettings &TextureSettings);

};

}
