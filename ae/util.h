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
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

namespace ae {

const char *LoadFileIntoMemory(const char *Path);
std::string RemoveExtension(const std::string &Path);
std::string TrimString(const std::string &String);
void TokenizeString(const std::string &String, std::vector<std::string> &Tokens, char Delimiter=' ');
int MakeDirectory(const std::string &Path);

void StartTimer();
void PrintTimer(const std::string &Message="", bool Reset=false);

template <class T>
inline T ToNumber(const std::string &String) {
	T Number = 0;

	std::stringstream Buffer(String);
	Buffer >> Number;

	return Number;
}

inline double Round1(double Number) { return std::round(Number * 10) / 10.0; }
inline double Round2(double Number) { return std::round(Number * 100) / 100.0; }
inline double Round3(double Number) { return std::round(Number * 1000) / 1000.0; }

inline double RoundUp1(double Number) { return std::ceil(Number * 10) / 10.0; }
inline double RoundUp2(double Number) { return std::ceil(Number * 100) / 100.0; }
inline double RoundUp3(double Number) { return std::ceil(Number * 1000) / 1000.0; }

inline double RoundDown1(double Number) { return std::floor(Number * 10) / 10.0; }
inline double RoundDown2(double Number) { return std::floor(Number * 100) / 100.0; }
inline double RoundDown3(double Number) { return std::floor(Number * 1000) / 1000.0; }

typedef double RoundFunction(double);

template<typename T> void FormatSI(std::stringstream &Buffer, T Number, RoundFunction *RoundFunction=RoundDown2);

}
