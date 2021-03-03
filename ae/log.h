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
#include <fstream>
#include <iostream>
#include <ctime>

namespace ae {

// Log file class
class _LogFile {

	public:

		_LogFile() : ToStdOut(true), PrependDate(true), TokenCount(0) { }
		~_LogFile() {
			Close();
		}

		// Open log file
		void Open(const char *Filename) {
			File.open(Filename, std::ios::app);
		}

		// Close file
		void Close() {
			File.close();
			File.clear();
		}

		// Handles most types
		template <typename Type>
		_LogFile &operator<<(const Type &Value) {

			// Get date
			char Buffer[64];
			if(PrependDate && !TokenCount)
				GetDateString(Buffer);

			// Output to stdout
			if(ToStdOut) {
				if(PrependDate && !TokenCount)
					std::clog << Buffer << " - ";

				std::clog << Value;
			}

			// Output to file
			if(File.is_open()) {
				if(PrependDate && !TokenCount)
					File << Buffer << " - ";

				File << Value;
			}

			// Update token count
			TokenCount++;

			return *this;
		}

		// Handles endl
		_LogFile &operator<<(std::ostream &(*Value)(std::ostream &)) {
			if(ToStdOut)
				std::clog << Value;

			if(File.is_open())
				File << Value;

			// Reset token count
			TokenCount = 0;

			return *this;
		}

		bool ToStdOut;
		bool PrependDate;

	private:

		void GetDateString(char *Buffer);

		std::ofstream File;
		int TokenCount;
};

// Get ISO 8601 timestamp
inline void _LogFile::GetDateString(char *Buffer) {
	time_t Now = time(nullptr);
	tm *LocalTime = localtime(&Now);
	strftime(Buffer, 64, "%Y-%m-%dT%H:%M:%S%z", LocalTime);
}

}
