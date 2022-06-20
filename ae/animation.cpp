/******************************************************************************
* Copyright (c) 2022 Alan Witkowski
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
#include <ae/animation.h>
#include <ae/texture.h>
#include <ae/graphics.h>
#include <ae/assets.h>

namespace ae {

// Constructor
_Animation::_Animation(ae::_BaseObject *Parent) :
	_Component(Parent),
	Timer(0.0),
	FramePeriod(1.0),
	Reel(0),
	Mode(STOPPED),
	Frame(0),
	LastFrame(-1),
	Direction(1) {
}

// Destructor
_Animation::~_Animation() {
}

// Update
void _Animation::Update(double FrameTime) {
	if(Templates.size() == 0)
		return;

	// Update timer
	Timer += FrameTime;

	// Update frame
	if(Mode == PLAYING && Timer >= FramePeriod) {
		Timer = 0;
		Frame += Direction;
		if(Frame > Templates[Reel]->EndFrame) {
			if(Templates[Reel]->RepeatType == BOUNCE) {
				Frame = Templates[Reel]->EndFrame - 1;
				Direction = -Direction;
			}
		}
		else if(Frame < Templates[Reel]->StartFrame) {
			if(Templates[Reel]->RepeatType == BOUNCE) {
				Frame = Templates[Reel]->StartFrame + 1;
				Direction = -Direction;
			}
		}
	}

	// Update texture coordinates
	if(Frame != LastFrame) {
		CalculateTextureCoords();
		LastFrame = Frame;
	}
}

// Play an animation
void _Animation::Play(std::size_t Reel) {
	if(Mode == PLAYING)
		return;

	Mode = PLAYING;
	this->Reel = Reel;
	Frame = Templates[Reel]->DefaultFrame;
	Timer = 0;
	Direction = 1;
}

// Stop
void _Animation::Stop() {
	Mode = STOPPED;
	Frame = Templates[Reel]->DefaultFrame;
}

// Calculate where in the texture to draw the current frame
void _Animation::CalculateTextureCoords() {
	if(!Templates[Reel]->Texture || !Templates[Reel]->Texture->ID)
		return;

	int FrameX = Frame % (Templates[Reel]->FramesPerLine);
	int FrameY = Frame / (Templates[Reel]->FramesPerLine);

	TextureCoords[0] = Templates[Reel]->TextureScale.x * FrameX;
	TextureCoords[1] = Templates[Reel]->TextureScale.y * FrameY;
	TextureCoords[2] = TextureCoords[0] + Templates[Reel]->TextureScale.x;
	TextureCoords[3] = TextureCoords[1] + Templates[Reel]->TextureScale.y;
}

}
