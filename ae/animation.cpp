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
_Animation::_Animation(ae::_BaseObject *ParentObject) : _Component(ParentObject) {
}

// Destructor
_Animation::~_Animation() {
}

// Update
void _Animation::Update(double FrameTime) {
	if(Reels.empty())
		return;

	// Update timer
	Timer += FrameTime;

	// Update frame
	if(Mode == PLAYING && Timer >= FramePeriod) {
		Timer -= FramePeriod;
		Frame += Direction;
		if(Frame > Reels[Reel]->EndFrame) {
			switch(Reels[Reel]->RepeatType) {
				case STOP:
					Frame = Reels[Reel]->EndFrame;
					Direction = 0;
					Mode = STOPPED;
				break;
				case BOUNCE:
					Frame = Reels[Reel]->EndFrame - 1;
					Direction = -Direction;
				break;
			}
		}
		else if(Frame < Reels[Reel]->StartFrame) {
			switch(Reels[Reel]->RepeatType) {
				case STOP:
					Frame = Reels[Reel]->StartFrame;
					Direction = 0;
					Mode = STOPPED;
				break;
				case BOUNCE:
					Frame = Reels[Reel]->StartFrame + 1;
					Direction = -Direction;
				break;
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
void _Animation::Play(size_t Reel, double Speed) {
	if(Mode == PLAYING)
		return;

	if(Reels.empty())
		return;

	Mode = PLAYING;
	this->Reel = Reel;
	Frame = Reels[Reel]->DefaultFrame;
	FramePeriod = Reels[Reel]->FramePeriod / Speed;
	Timer = 0;
	Direction = 1;
	LastFrame = -1;
}

// Stop
void _Animation::Stop() {
	if(Reels.empty())
		return;

	Mode = STOPPED;
	Frame = Reels[Reel]->DefaultFrame;
}

// Calculate where in the texture to draw the current frame
void _Animation::CalculateTextureCoords() {
	if(Reels.empty() || !Reels[Reel]->Texture || !Reels[Reel]->Texture->ID)
		return;

	int FrameX = Frame % (Reels[Reel]->FramesPerLine);
	int FrameY = Frame / (Reels[Reel]->FramesPerLine);

	TextureCoords[0] = Reels[Reel]->TextureScale.x * FrameX;
	TextureCoords[1] = Reels[Reel]->TextureScale.y * FrameY;
	TextureCoords[2] = TextureCoords[0] + Reels[Reel]->TextureScale.x;
	TextureCoords[3] = TextureCoords[1] + Reels[Reel]->TextureScale.y;
}

}
