/******************************************************************************
* Copyright (c) 2017 Alan Witkowski
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
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace ae {

// Light
struct _Light {
	_Light() : PositionID(-1), ColorID(-1), RadiusID(-1), Position(0.0f, 0.0f, 0.0f), Color(1.0f), Radius(1.0f) { }

	GLint PositionID;
	GLint ColorID;
	GLint RadiusID;
	glm::vec3 Position;
	glm::vec4 Color;
	float Radius;
};

}
