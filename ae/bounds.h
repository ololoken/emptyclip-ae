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

// Libraries
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

// Bounds struct
struct _Bounds {
	_Bounds() : Start(0.0f, 0.0f), End(0.0f, 0.0f) { }
	_Bounds(const glm::vec2 &Start, const glm::vec2 &End) : Start(Start), End(End) { }
	_Bounds(const glm::vec4 &Bounds) : Start(Bounds[0], Bounds[1]), End(Bounds[2], Bounds[3]) { }
	bool Inside(const glm::vec2 &Point) { return Point.x >= Start.x && Point.y >= Start.y && Point.x < End.x && Point.y < End.y; }

	glm::vec2 Start;
	glm::vec2 End;
};
