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
#include <ae/type.h>
#include <unordered_map>
#include <vector>
#include <limits>
#include <stdexcept>

namespace ae {

// Classes
template<class T> class _Manager {

	public:

		~_Manager();

		// Updates
		void Update(double FrameTime);

		// Object management
		T *Create();
		T *CreateWithID(NetworkIDType ID);
		T *GetObject(NetworkIDType ID);
		void Clear();

		// Storage
		std::vector<T *> Objects;

	private:

		// IDs
		std::unordered_map<NetworkIDType, T *> IDMap;
		NetworkIDType NextID{0};

};

// Destructor
template <class T>
_Manager<T>::~_Manager() {

	// Delete objects
	for(auto &Object : Objects)
		delete Object;
}

// Update
template <class T>
void _Manager<T>::Update(double FrameTime) {

	// Update objects
	bool Delete = false;
	size_t ObjectCount = Objects.size();
	for(size_t i = 0; i < ObjectCount; i++) {
		Objects[i]->Update(FrameTime);
		if(Objects[i]->Deleted)
			Delete = true;
	}

	if(!Delete)
		return;

	// Clean up deleted objects
	for(size_t i = Objects.size() - 1; i < Objects.size(); i--) {
		if(!Objects[i]->Deleted)
			continue;

		IDMap[Objects[i]->NetworkID] = nullptr;
		delete Objects[i];
		Objects.erase(Objects.begin() + (int)i);
	}
}

// Generate object with new network id
template <class T>
T *_Manager<T>::Create() {

	// Search for an empty slot
	for(NetworkIDType i = 0; i < std::numeric_limits<NetworkIDType>::max(); i++) {
		if(!IDMap[NextID]) {
			T *Object = new T;
			Object->NetworkID = NextID;

			Objects.push_back(Object);
			IDMap[NextID] = Object;

			return Object;
		}

		NextID++;
	}

	throw std::runtime_error(std::string(__func__) + " ran out of object ids");
}

// Create object with existing id
template <class T>
T *_Manager<T>::CreateWithID(NetworkIDType ID) {
	T *Object = new T;
	Object->NetworkID = ID;

	Objects.push_back(Object);
	IDMap[ID] = Object;

	return Object;
}

// Get object from manager
template <class T>
T *_Manager<T>::GetObject(NetworkIDType ID) {

	auto Iterator = IDMap.find(ID);
	if(Iterator == IDMap.end())
		return nullptr;

	return Iterator->second;
}

// Delete all objects and reset
template <class T>
void _Manager<T>::Clear() {

	for(auto Object : Objects)
		delete Object;

	IDMap.clear();
	Objects.clear();
	NextID = 0;
}

}
