/******************************************************************************
* Copyright (c) 2020 Alan Witkowski
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
#include <cstddef>

namespace ae {

// Classes
class _Buffer {

	public:

		_Buffer(std::size_t InitialSize=32);
		_Buffer(const char *ExistingBuffer, std::size_t Length);
		~_Buffer();

		// Write data
		template<typename T> T *Write(const T &Value) {
			AlignAndExpand(sizeof(T));

			T *Address = (T *)&Data[CurrentByte];
			*Address = Value;

			CurrentByte += sizeof(T);
			return Address;
		}

		// Read data
		template<typename T> T Read() {
			AlignBitIndex();

			T Value = *(T *)(&Data[CurrentByte]);
			CurrentByte += sizeof(T);

			return Value;
		}

		void WriteBit(bool Value);
		void WriteString(const char *Value);

		bool ReadBit();
		const char *ReadString();

		const char *GetData() const { return Data; }
		char &operator[](std::size_t Index) { return Data[Index]; }
		char operator[](std::size_t Index) const { return Data[Index]; }

		void Shrink();
		void SetAllocatedSize(std::size_t Size) { AllocatedSize = Size; }
		std::size_t GetAllocatedSize() const { return AllocatedSize; }
		std::size_t GetCurrentSize() const { return CurrentByte + (CurrentBit != 0); }
		bool End() const { return CurrentByte == AllocatedSize; }

		void StartRead() { CurrentByte = 0; CurrentBit = 0; }

	private:

		void Resize(std::size_t NewSize);
		void AlignBitIndex();
		void AlignAndExpand(std::size_t NewWriteSize);

		char *Data;
		std::size_t AllocatedSize, CurrentByte;
		unsigned char CurrentBit;
};

}
