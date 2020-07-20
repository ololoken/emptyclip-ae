/******************************************************************************
* Copyright (c) 2020 Alan Witkowski
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
#include <ae/framebuffer.h>
#include <stdexcept>

namespace ae {

// Constructor
_Framebuffer::_Framebuffer(const glm::ivec2 &Size) :
	ID(0),
	TextureID(0),
	RenderBufferID(0) {

	// Generate framebuffer
	glGenFramebuffers(1, &ID);
	glBindFramebuffer(GL_FRAMEBUFFER, ID);

	// Generate framebuffer texture
	glGenTextures(1, &TextureID);
	glBindTexture(GL_TEXTURE_2D, TextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Size.x, Size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TextureID, 0);

	// Generate render buffer
	glGenRenderbuffers(1, &RenderBufferID);
	glBindRenderbuffer(GL_RENDERBUFFER, RenderBufferID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, Size.x, Size.y);
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw std::runtime_error("glCheckFramebufferStatus not ready");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Destructor
_Framebuffer::~_Framebuffer() {
	glDeleteBuffers(1, &ID);
	glDeleteTextures(1, &TextureID);
	glDeleteRenderbuffers(1, &RenderBufferID);
}

// Resize framebuffer
void _Framebuffer::Resize(const glm::ivec2 &Size) {
	if(!TextureID)
		return;

	glBindTexture(GL_TEXTURE_2D, TextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Size.x, Size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
}

// Enable
void _Framebuffer::Use() {
	if(!ID)
		return;

	glBindFramebuffer(GL_FRAMEBUFFER, ID);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

}
