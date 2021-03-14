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

// Libraries
#include <chrono>
#include <thread>

namespace ae {

class _FrameLimit {

	public:

		// Constructor
		_FrameLimit(double FrameRate) : FrameRate(FrameRate), ExtraTime(0.0) { Reset(); }

		// Reset timer
		void Reset() {
			Timer = std::chrono::high_resolution_clock::now();
		}

		// Update frame rate
		void SetFrameRate(double Value) {
			FrameRate = Value;
			ExtraTime = 0.0;
			Reset();
		}

		// Limit frame rate
		void Update() {
			if(FrameRate <= 0.0)
				return;

			// Account for extra time from last frame
			double Elapsed = std::chrono::duration<int64_t, std::nano>(std::chrono::high_resolution_clock::now() - Timer).count() - ExtraTime;
			Reset();

			// Get sleep duration
			ExtraTime = 1000000000 / FrameRate - Elapsed;
			if(ExtraTime > 0)
				std::this_thread::sleep_for(std::chrono::nanoseconds((int64_t)(ExtraTime)));
			else
				ExtraTime = 0;
		}

	private:

		std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> Timer;
		double FrameRate;
		double ExtraTime;

};

}
