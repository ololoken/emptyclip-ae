/******************************************************************************
* Copyright (c) 2018 Alan Witkowski
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
#include <string>
#include <vector>
#include <glm/vec4.hpp>

namespace ae {

// Forward Declarations
class _Element;
class _Program;
class _Font;
struct _Style;

// Console class
class _Console {

	// Console message
	struct _Message {
		std::string Text;
		glm::vec4 Color;
	};

	public:

		_Console(const _Program *Program, const _Font *Font);
		~_Console();

		void Update(double FrameTime);
		void Render(double BlendFactor);

		void Toggle();
		void UpdateSize();
		void AddMessage(const std::string &Text, const glm::vec4 &Color=glm::vec4(1.0f));

	private:

		// Messages
		std::vector<_Message> Messages;

		// UI
		_Element *Element;
		_Style *Style;
		const _Font *Font;
		float FontHeight;

};

}
