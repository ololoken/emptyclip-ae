/******************************************************************************
* Copyright (c) 2025 Alan Witkowski
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
#include <unordered_map>
#include <vector>

namespace ae {

// Class for getting a list of files in a directory
class _Files {

	public:

		_Files() {}
		_Files(const std::string &Path);
		void Load(const std::string &Path, bool PrependPath=false);

		std::vector<std::string> Nodes;
		std::string Path;
};

// Class for reading packed file
class _FilePack {

	public:

		struct _File {
			_File() {}
			_File(const std::string &Name, int Size, int Offset) : Name(Name), Size(Size), Offset(Offset) {}

			std::string Name;
			int Size{0};
			int Offset{0};
		};

		_FilePack(const std::string &Path);
		void Load(const std::string &Path);

		std::unordered_map<std::string, _File> Data;
		std::string Path;
		int BodyOffset{0};

};

}
