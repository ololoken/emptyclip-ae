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
#include <ae/texture_array.h>
#include <SDL_image.h>
#include <stdexcept>

namespace ae {

// Constructor
_TextureArray::_TextureArray(const glm::ivec2 &Size, int Layers) :
	Size(Size),
	Count(0) {

	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D_ARRAY, ID);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, Size.x, Size.y, Layers, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
}

// Destructor
_TextureArray::~_TextureArray() {
	if(ID)
		glDeleteTextures(1, &ID);
}

// Add texture to array
void _TextureArray::AddTexture(const std::string &Path, GLfloat RepeatMode) {

	// Open file
	SDL_Surface *Image = IMG_Load(Path.c_str());
	if(!Image)
		throw std::runtime_error("Error loading image: " + Path + " with error: " + IMG_GetError());

	// Check size
	if(Size.x != Image->w || Size.y != Image->h)
		throw std::runtime_error("Bad texture size for " + Path);

	// Determine OpenGL format
	GLint ColorFormat;
	switch(Image->format->BitsPerPixel) {
		case 32:
			ColorFormat = GL_RGBA;
		break;
		default:
			throw std::runtime_error("Unsupported bpp " + std::to_string(Image->format->BitsPerPixel) + " for texture " + Path);
	}

	// Copy pixel data to texture array slice
	glBindTexture(GL_TEXTURE_2D_ARRAY, ID);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, RepeatMode);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, RepeatMode);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, Count, Size.x, Size.y, 1, (GLenum)ColorFormat, GL_UNSIGNED_BYTE, Image->pixels);
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	Count++;

	// Close
	SDL_FreeSurface(Image);
}

}
