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
#include <ae/input.h>
#include <ae/state.h>
#include <framework.h>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>

namespace ae {

_Input Input;

const std::string MouseButtonNames[] = {
	"",
	"LMB",
	"MMB",
	"RMB",
	"MB4",
	"MB5",
	"MB6",
	"MB7",
	"MB8",
	"MB9",
	"MB10",
	"MB11",
	"MB12",
	"MB13",
	"MB14",
	"MB15",
	"MB16",
	"MB17",
	"MB18",
	"MB19",
	"MB20",
};

// Update input state
void _Input::Update(double FrameTime) {

	// Update state
	KeyState = SDL_GetKeyboardState(nullptr);
	MouseState = SDL_GetMouseState(&Mouse.x, &Mouse.y);
}

// Returns the name of a key
const char *_Input::GetKeyName(int Key) {
	return SDL_GetScancodeName((SDL_Scancode)Key);
}

// Returns the name of a mouse button
const std::string &_Input::GetMouseButtonName(uint32_t Button) {
	if(Button >= sizeof(MouseButtonNames))
		return MouseButtonNames[0];

	return MouseButtonNames[Button];
}

// Returns true if a mod key is down
bool _Input::ModKeyDown(int Key) {
	return SDL_GetModState() & Key;
}

// Returns true if a mouse button is down
bool _Input::MouseDown(uint32_t Button) {
	return MouseState & SDL_BUTTON(Button);
}

}
