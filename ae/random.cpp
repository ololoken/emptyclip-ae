/******************************************************************************
* Copyright (c) 2023 Alan Witkowski
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
#include <ae/random.h>

namespace ae {

std::mt19937 RandomGenerator;

template<typename T> T GetRandomInt(T Min, T Max) {
	std::uniform_int_distribution<T> Distribution(Min, Max);
	return Distribution(RandomGenerator);
}

double GetRandomReal(double Min, double Max) {
	std::uniform_real_distribution<double> Distribution(Min, Max);
	return Distribution(RandomGenerator);
}

template int32_t GetRandomInt<int32_t>(int32_t Min, int32_t Max);
template int64_t GetRandomInt<int64_t>(int64_t Min, int64_t Max);
template uint32_t GetRandomInt<uint32_t>(uint32_t Min, uint32_t Max);
template uint64_t GetRandomInt<uint64_t>(uint64_t Min, uint64_t Max);

}
