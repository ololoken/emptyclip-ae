/******************************************************************************
* Copyright (c) 2021 Alan Witkowski
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

namespace ae {

// Holds an OpenGL texture array
class _TextureArray {

	public:

		_TextureArray(const glm::ivec2 &Size, int Layers);
		~_TextureArray();

		void AddTexture(const std::string &Path, GLfloat RepeatMode=GL_REPEAT, GLint MagFilter=GL_LINEAR, GLint MinFilter=GL_LINEAR_MIPMAP_LINEAR);

		GLuint ID;
		glm::ivec2 Size;
		int Count;

	private:
};

}
