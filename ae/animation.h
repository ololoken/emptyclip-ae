/******************************************************************************
* Copyright (c) 2024 Alan Witkowski
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
#include <ae/component.h>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <vector>
#include <string>

namespace ae {

// Forward Declarations
class _Texture;

// Holds an animation reel
struct _Reel {
	std::string ID;
	const _Texture *Texture;
	glm::vec2 TextureScale;
	glm::ivec2 FrameSize;
	double FramePeriod;
	int FramesPerLine;
	int StartFrame;
	int EndFrame;
	int DefaultFrame;
	int RepeatType;
};

// Classes
class _Animation : public ae::_Component {

	public:

		// State
		enum PlayType {
			STOPPED,
			PLAYING,
			PAUSED
		};

		// Repeat modes
		enum RepeatType {
			STOP,
			WRAP,
			BOUNCE
		};

		_Animation(ae::_BaseObject *ParentObject);
		~_Animation() override;

		void Update(double FrameTime) override;
		void Play(size_t Reel, double Speed=1.0);
		void Stop();
		void CalculateTextureCoords();

		bool IsPlaying() const { return Mode == PLAYING; }
		bool IsStopped() const { return Mode == STOPPED; }

		std::vector<const ae::_Reel *> Reels;
		glm::vec4 TextureCoords{0.0f};
		double Timer{0.0};
		double FramePeriod{1.0};
		size_t Reel{0};
		int Mode{STOPPED};
		int Frame{0};
		int LastFrame{-1};
		int Direction{1};

};

}
